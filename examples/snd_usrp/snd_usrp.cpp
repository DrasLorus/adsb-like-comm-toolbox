

#include <chrono>
#include <getopt.h>
#include <csignal>
#include <unistd.h>
#include <fstream>
#include <bitset>

#include <chrono>
#include <string>
#include <memory>
#include <ctime>
#include <cwchar>

#include "../../src/Tools/ExportVector/ExportVector.hpp"

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

#include "../../src/Radio/Emitter/Library/EmitterLibrary.hpp"


#include "../../src/couleur.h"

bool isFinished = false;

void my_ctrl_c_handler(int s){
    if( isFinished == true )
    {
        exit( EXIT_FAILURE );
    }
    isFinished = true;
}

void CRC( const uint8_t trame[88], uint8_t otrame[112] )
{
    bitset<25> crc      = 0b0000000000000000000000000;
    bitset<25> polynome = 0b1001000000101111111111111;

    for(int q = 0; q < 25; q++){
        crc[q] = trame[q];
    }

    for (int q = 25; q < 88; q++){
        if ( crc[0] == 1){
            crc = crc xor polynome;
        }
        crc = crc >> 1;
        crc[24] = trame[q];
    }
    for (int q = 88; q < 112; q++){
        if ( crc[0] == 1){
            crc = crc xor polynome;
        }
        crc = crc >> 1;
        crc[24] = 0;
    }
    if ( crc[0] == 1){
        crc = crc xor polynome;
    }
    crc = crc >> 1;
    for (int q = 0; q < 88; q++) otrame[     q] = trame[q];
    for (int q = 0; q < 24; q++) otrame[88 + q] = crc[q];
}

bool CRC( const uint8_t trame[120] ){

    bitset<25> crc      = 0b0000000000000000000000000;
    bitset<25> polynome = 0b1001000000101111111111111;

    for(int q = 8; q < 33; q++){
        crc[q - 8] = trame[q];
    }

    for (int q=33; q < 120; q++){
        if ( crc[0] == 1){
            crc = crc xor polynome;
        }
        crc = crc >> 1;
        crc[24] = trame[q];
    }

    if ( crc[0] == 1){
        crc = crc xor polynome;
    }
    if (crc == 0x00000000)
        return true;
    else
        return false;
}

const uint32_t CRC_POLY = 0x00FFF409;
//const uint32_t CRC_POLY = 0x902FFF00;
uint32_t CRC_LUT[256];

void init_crc_lut(){
    uint32_t i,j;
    uint32_t crc;

    for(i = 0; i < 256; i++){
        crc = i << 16;
        for(j = 0; j < 8; j++){
            if(crc & 0x800000){
                crc = ((crc << 1) ^ CRC_POLY) & 0xffffff;
            }
            else{
                crc  = (crc << 1) & 0xffffff;
            }

        }
        CRC_LUT[i] = crc & 0xffffff;
    }
}

uint32_t check_crc(const uint8_t *msg, const uint32_t length)
{
    uint32_t i;
    uint32_t crc=0;
    for(i = 0; i < length; i++){
        crc = CRC_LUT[ ((crc >> 16) ^ msg[i]) & 0xff] ^ (crc << 8);
    }
    return crc &0xffffff;
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
#if 0
    uint8_t xtrame[112] =  {
            1,0,0,0,1,1,0,1,0,1,0,0,0,0,0,0,0,1,1,0,1,0,
            1,1,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
            0,1,0,1,1,0,1,0,0,1,1,0,0,1,1,1,1,0,0,0,1,1,
            0,1,0,1,0,0,1,1,0,1,0,0,1,0,0,0,1,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    uint8_t trame[88];
    uint8_t otrame[120];

    for(int i = 0; i <  88;  i+= 1)  trame[i] = i%2;
    for(int i = 0; i < 120;  i+= 1) otrame[i] =   0;

    CRC(xtrame, otrame + 8);

//    otrame[8] = !otrame[8];

    if( CRC(otrame) == false )
    {
        std::cout << "CRC check failed !" << std::endl;
    }

    init_crc_lut();

    uint32_t ok = check_crc(otrame + 8, 112);

    if( ok != 0 )
    {
        std::cout << "CRC+ check failed !" << std::endl;
        printf("0x%8.8X\n", ok);
    }

    for(int i = 0; i <  88;  i+= 1) printf("%d", xtrame[i  ]); printf("\n");
    for(int i = 0; i < 112;  i+= 1) printf("%d", otrame[i+8]); printf("\n");
    exit( EXIT_FAILURE );
#endif

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    int c; //getopt

    Parameters param;
    param.set("mode_radio", "radio");
    param.set("filename",   "hackrf");

    param.set("fc",      2400000000.0);
    param.set("fe",        4000000.0);
    param.set("fe_real",   4000000.0);

    param.set("surEch",     1);
    param.set("tx_gain",   32);
    param.set("antenna",    1);
    param.set("amplifier",  1);

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
    Emitter* radio = EmitterLibrary::allocate( param );
//    EmitterHackRF radio( param.toDouble("fc"), param.toDouble("fe_real") );
    radio->initialize    ();
    radio->start_engine  ();
    radio->set_txvga_gain( param.toInt("tx_gain") );
    usleep( 2000 );
    

    const uint32_t payload = param.toInt("payload");

    vector<uint8_t> vec_dat (               payload            );  //  11 octets
    vector<uint8_t> vec_bits(      (8 * (payload    ))      );  //  88 bits
    vector<uint8_t> vec_crc (      (8 * (payload + 3))      );  // 112 bits
    vector<uint8_t> vec_sync(     ((8 * (payload + 3)) + 8) );  // 120 bits
    vector< int8_t> vec_ppm ( 2 * ((8 * (payload + 3)) + 8) );  // 240 bits
    vector< int8_t> vec_up  ( 4 * ((8 * (payload + 3)) + 8) );  // 480 bits
    vector< int8_t> vec_iq  ( 8 * ((8 * (payload + 3)) + 8) );  // 480 symbols (I/Q)
    vector< int8_t> vec_zr  ( 8 * ((8 * (payload + 3)) + 8) );  // 480 symbols (I/Q)

    for(int i = 0; i < vec_zr.size(); i += 1)
        vec_zr[i] = 0;

    InsertCRC32b      i_crc;
    BitUnpacking      i_bp;
    InsertADSBSynchro i_sync;
    PPM_mod           i_ppm;
    UpSampling        i_up(2);
    IQ_Insertion      i_iq;

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
    printf("(II) -> Time delay between frames : %d us\n",    param.toInt("sleep_time"));
    printf("(II)\n");
    printf("(II) HackRF module configuration :\n");
    printf("(II) -> Transmitter gain value : %d\n",      param.toInt("tx_gain"));
    printf("(II) -> Antenna parameter      : disable\n" );
    printf("(II) -> Amplifier parameter    : disable\n");
    printf("(II)\n");

    //
    // On lance le processus de communication
    //

    auto start = std::chrono::system_clock::now(); // This and "end"'s type is std::chrono::time_point

    int nFrames = 0;
    while( isFinished == false )
    {
        for(int i = 0; i < vec_dat.size(); i += 1)
            vec_dat[i] = 1;
        vec_dat[vec_dat.size() - 1] = nFrames;

//        for(int i = 0; i < vec_dat.size(); i += 1)
//            printf("%d ", vec_dat[i]); printf("\n");

        i_bp.execute  (vec_dat,  vec_bits);

//        for(int i = 0; i < vec_bits.size(); i += 1)
//            printf("%d ", vec_bits[i]); printf("\n");

        CRC           (vec_bits.data(),  vec_crc.data());

//        for(int i = 0; i < vec_crc.size(); i += 1)
//            printf("%d ", vec_crc[i]); printf("\n");

        i_sync.execute(vec_crc, vec_sync);
        if( CRC(vec_sync.data()) == false )
        {
            std::cout << "CRC check failed !" << std::endl;
            exit( EXIT_FAILURE );
        }

//        for(int i = 0; i < vec_sync.size(); i += 1)
//            printf("%d ", vec_sync[i]); printf("\n");

        i_ppm.execute (vec_sync, vec_ppm );

//        for(int i = 0; i < vec_ppm.size(); i += 1)
//            printf("%d ", vec_ppm[i]); printf("\n");

        i_up.execute  (vec_ppm,  vec_up  );

//        for(int i = 0; i < vec_up.size(); i += 1)
//            printf("%d ", vec_up[i]); printf("\n");

        i_iq.execute  (vec_up,   vec_iq  );

//        for(int i = 0; i < vec_iq.size(); i += 1)
//            printf("%d ", vec_iq[i]); printf("\n");

//        printf("(II) -> Radio transmission\n");
//        ExportVector::SaveToS8(vec_iq, "vec_iq.data");
        radio->emission ( vec_zr );
        radio->emission ( vec_iq );
        radio->emission ( vec_zr );

        usleep( 100000 );
//        exit( 0 );
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
    std::cout << "Débit utile par seconde  (bytes)   = " << (nFrames/elapsed.count()*payload) << std::endl;
    std::cout << "Débit utile par seconde  (bits)    = " << (nFrames/elapsed.count()*payload*8) << std::endl;

    radio->stop_engine();
    radio->close();
    delete radio;

	return 0;
}
