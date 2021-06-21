#include <chrono>
#include <getopt.h>
#include <csignal>
#include <unistd.h>
#include <fstream>


#include <chrono>
#include <string>
#include <memory>
#include <ctime>
#include <cwchar>

using std::chrono::system_clock;
 
void current_time(uint8_t buf[24])
{
    system_clock::time_point tp = system_clock::now();
    time_t raw_time = system_clock::to_time_t(tp);
    struct tm *timeinfo = std::localtime(&raw_time);
//    wchar_t buf[24] = { 0 };
//  "%F %X," can also be used in standard c++, but not in VC2017
    std::wcsftime((wchar_t*)buf, 24, L"%Y-%m-%d %H:%M:%S,", timeinfo);
//    return std::wstring(buf);
}



#include "../../src/Tools/Parameters/Parameters.hpp"

#include "../../src/Processing/CRC32b/InsertCRC32b/InsertCRC32b.hpp"
#include "../../src/Processing/PPM/mod/PPM_mod.hpp"
#include "../../src/Processing/Sampling/Up/UpSampling.hpp"
#include "../../src/Processing/IQ/Insertion/IQ_Insertion.hpp"
#include "../../src/Processing/DataPacking/BitUnpacking/BitUnpacking.hpp"
#include "../../src/Processing/ADSBSynchro/InsertADSBSynchro/InsertADSBSynchro.hpp"

#include "../../src/Radio/Emitter/Radio/HackRF/EmitterHackRF.hpp"


#include "../../src/couleur.h"

bool isFinished = false;

void my_ctrl_c_handler(int s){
    if( isFinished == true )
    {
        exit( EXIT_FAILURE );
    }
    isFinished = true;
}



using namespace std;

/*   ============================== MAIN =========================== */
/*
	4 ech = 1 symb
	1 trame = 120 symb = 480 ech
*/
int main(int argc, char* argv[])
{
    //
    //  On gere manuellement le CTRL-C afin de quitter proprement le programme
    //  apres avoir eteint la radio logicielle...
    //

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    int c; //getopt

    Parameters param;
    param.set("mode_radio", "radio");
    param.set("filename",   "usrp");

    param.set("fc",      433000000.0);
    param.set("fe",        4000000.0);
    param.set("fe_real",   4000000.0);

    param.set("surEch",     1);
    param.set("tx_gain",   24);
    param.set("antenna",    1);
    param.set("amplifier",  0);

    param.set("payload",   11);

    param.set("sleep_time",   100000);

    param.set("verbose",   false);


    param.set("crystal_correct",   0);

    static struct option long_options[] =
            {
                    {"radio",       required_argument, NULL, 'r'}, // a partir d'un fichier
                    {"file",        required_argument, NULL, 'f'}, // a partir d'un fichier
                    {"file-stream", required_argument, NULL, 'F'}, // a partir d'un fichier

                    {"fc",       required_argument, NULL, 'c'}, // changer la frequence de la porteuse
                    {"fe",       required_argument, NULL, 'e'}, // changer la frequence echantillonnage

                    {"payload",     required_argument, NULL, 'p'}, // changer la frequence de la porteuse

                    {"bmp_file",    required_argument, NULL, 'b'}, // changer la frequence de la porteuse

                    {"verbose",           no_argument, NULL, 'v'}, // changer la frequence de la porteuse
                    {"sleep",       required_argument, NULL, 's'}, // changer la frequence echantillonnage
                    {"payload",     required_argument, NULL, 'p'}, // changer la frequence de la porteuse
                    {"tx_gain",     required_argument, NULL, 'g'}, // changer la frequence echantillonnage
                    {"up_sampling", required_argument, NULL, 'U'}, // changer la frequence echantillonnage

                    {"max_frames",  required_argument, NULL, 'm'}, // changer la frequence echantillonnage
                    {"ppm",         required_argument, NULL, 'P'}, // changer la frequence echantillonnage

                    {"fec",         no_argument, NULL, 'Z'},
                    {"no-fec",      no_argument, NULL, 'z'},

                    {NULL, 0,                    NULL, 0}
            };

    int option_index = 0;

    cout << "par Bertrand LE GAL - Octobre 2020" << endl;
    cout << "==================================== ADSB ====================================" << endl;
    printf("%s", KRED);

    while ((c = getopt_long(argc, argv, "be:p:f:n:s:vt8", long_options, &option_index)) != -1) {
        //int this_option_optind = optind ? optind : 1;
        switch (c) {
            case 0:
                printf("%soption %s%s", long_options[option_index].name, KNRM, KRED);
                if (optarg)
                    printf("%s with arg %s%s", optarg, KNRM, KRED);
                printf("\n");
                break;

            case 'c':
                param.set("fc",   std::stod(optarg));
                printf("%soption fc = %f Hz%s\n", KNRM, param.toFloat("fc"), KRED);
                break;

            case 'e' :
                param.set("fe",   std::stod(optarg));
                param.set("fe_real",  param.toDouble("surEch") * param.toDouble("fe"));
                break;

            case 'U' :
                param.set("surEch",   std::atoi(optarg));
                param.set("fe_real",  param.toDouble("surEch") * param.toDouble("fe"));
                break;

            case '?':
                break;

            case 'r':
                param.set("mode_radio",   "radio");
                param.set("filename",   optarg);
                break;

            case 'b':
                param.set("frontend",    "BMPFile");
                param.set("frontend_opt", optarg);
                break;

            case 'f':
                param.set("mode_radio",   "file");
                param.set("filename",   optarg);
                break;

            case 'F':
                param.set("mode_radio",   "file-stream");
                param.set("filename",   optarg);
                break;

            case 'v' :
                param.set("verbose",   1);
                printf("%soption verbose = 1%s\n", KNRM, KRED);
                break;

            case 'p' :
                param.set("payload",   std::atoi(optarg));
                printf("%soption payload = %d%s\n", KNRM, param.toInt("payload"), KRED);
                break;

            case 'm' :
                param.set("max_frames",   std::atoi(optarg));
                printf("%soption max_frames = %d%s\n", KNRM, param.toInt("max_frames"), KRED);
                break;

            case 's' :
                param.set("sleep_time",   std::atoi(optarg));
                printf("%soption sleep = %d us%s\n", KNRM, param.toInt("sleep_time"), KRED);
                break;

            case 'g' :
                param.set("tx_gain",   std::atoi(optarg));
                printf("%soption tx_gain = %d dB%s\n", KNRM, param.toInt("tx_gain"), KRED);
                break;

            case 'P' :
                param.set("crystal_correct",   std::atoi(optarg));
                printf("%soption crystal_correct = %d dB%s\n", KNRM, param.toInt("crystal_correct"), KRED);
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
    }
    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s\n", argv[optind++]);
    }
    printf("%s", KNRM);
    cout << endl;


    //
    // On selectionne le module d'emission en fonction des choix de l'utilisateur
    //
    EmitterHackRF radio( param.toDouble("fc"), param.toDouble("fe_real") );
    radio.initialize    ();
    radio.start_engine  ();
    radio.set_txvga_gain( param.toInt("tx_gain") );
    usleep( 2000 );
    

    const uint32_t payload = param.toInt("payload");

    vector<uint8_t> vec_dat (               payload            );
    vector<uint8_t> vec_crc (           (payload + 3)       );
    vector<uint8_t> vec_bits(      (8 * (payload + 3))      );
    vector<uint8_t> vec_sync(     ((8 * (payload + 3)) + 8) );
    vector< int8_t> vec_ppm ( 2 * ((8 * (payload + 3)) + 8) );
    vector< int8_t> vec_up  ( 4 * ((8 * (payload + 3)) + 8) );
    vector< int8_t> vec_iq  ( 8 * ((8 * (payload + 3)) + 8) );

    InsertCRC32b      i_crc;
    BitUnpacking      i_bp;
    InsertADSBSynchro i_sync;
    PPM_mod           i_ppm;
    UpSampling        i_up(2);
    IQ_Insertion      i_iq;

    const uint32_t sleep_time = param.toInt("sleep_time");

    printf("(II) Launching the emitter application :\n");
    printf("(II) -> Modulation frequency    : %4d MHz\n",    (uint32_t)(param.toDouble("fc"     )/1000000.0));
    printf("(II) -> Symbol sampling freq.   : %4d MHz\n",    (uint32_t)(param.toDouble("fe"     )/1000000.0));
    printf("(II) -> Real sampling frequency : %4d MHz\n",    (uint32_t)(param.toDouble("fe_real")/1000000.0));
    printf("(II) -> Sampling factor for DAC : %2dx\n",       param.toInt("surEch"));
    printf("(II)\n");

    const int32_t crystal_correct_ppm = param.toInt("crystal_correct");
    if( crystal_correct_ppm != 0 )
    {
        double freq_hz        = param.toDouble("fc");
        double sample_rate_hz = param.toDouble("fe");

        sample_rate_hz = (uint32_t)((double)sample_rate_hz * (1000000.0 - crystal_correct_ppm)/1000000.0+0.5);
        freq_hz        = freq_hz * (1000000.0 - crystal_correct_ppm)/1000000.0;

        double real_rate_hz = param.toDouble("surEch") * sample_rate_hz;

        param.set("fc",           freq_hz);
        param.set("fe",    sample_rate_hz);
        param.set("fe_real", real_rate_hz);

        printf("(II) -> Corrected modulation frequency    : %4d MHz\n",    (uint32_t)(param.toDouble("fc"     )/1000000.0));
        printf("(II) -> Corrected symbol sampling freq.   : %4d MHz\n",    (uint32_t)(param.toDouble("fe"     )/1000000.0));
        printf("(II) -> Corrected real sampling frequency : %4d MHz\n",    (uint32_t)(param.toDouble("fe_real")/1000000.0));
        printf("(II)\n");
    }

    printf("(II) ADSB-like configuration parameters :\n");
//    printf("(II) -> ADSB-like payload length  : %d bytes\n", f.payload_size());
//    printf("(II) -> ADSB-like frame length    : %d bits\n",  f.frame_bits());
    printf("(II) -> Time delay between frames : %d us\n",    param.toInt("sleep_time"));
    printf("(II)\n");
    printf("(II) HackRF module configuration :\n");
    printf("(II) -> VGA transmitter gain value : %d\n",      param.toInt("tx_gain"));
    printf("(II) -> VGA transmitter gain value : %d\n",      param.toInt("tx_gain"));
    printf("(II) -> HackRF antenna parameter   : disable\n" );
    printf("(II) -> HackRF amplifier parameter : disable\n");
    printf("(II)\n");

    //
    // On lance le processus de communication
    //

    auto start = std::chrono::system_clock::now(); // This and "end"'s type is std::chrono::time_point

    int nFrames = 0;
    uint64_t data_pos = 0;
    while( isFinished == false )
    {
        current_time( vec_dat.data() );
        
        i_crc.execute (vec_dat,  vec_crc );
        i_bp.execute  (vec_crc,  vec_bits);
        i_sync.execute(vec_bits, vec_sync);
        i_ppm.execute (vec_sync, vec_ppm );
        i_up.execute  (vec_ppm,  vec_up  );
        i_iq.execute  (vec_up,   vec_iq  );

        radio.emission ( vec_iq );

        usleep( sleep_time );

        nFrames += 1;
    }


    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    usleep( 200000 ); // On attend un peu pour etre certain que la derniere trame
                      // a bien été transmise avant que l'on coupe l'emetteur

    std::cout << std::endl;
    std::cout << "Nombre de trames emises  (frames)  = " << nFrames << std::endl;
    std::cout << "Temps total d'émission   (seconds) = " << elapsed.count() << std::endl;
    std::cout << "Trames par seconde       (frames)  = " << (nFrames/elapsed.count()) << std::endl;
//    std::cout << "Débit emis par seconde   (bits)    = " << (nFrames/elapsed.count()*f.frame_bits()) << std::endl;
    std::cout << "Débit utile par seconde  (bytes)   = " << (nFrames/elapsed.count()*payload) << std::endl;
    std::cout << "Débit utile par seconde  (bits)    = " << (nFrames/elapsed.count()*payload*8) << std::endl;

    radio.stop_engine();
    radio.close();

	return 0;
}
