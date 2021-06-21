#include <chrono>
#include <getopt.h>
#include <csignal>
#include <unistd.h>
#include <fstream>

#include "./Frame/Frame.hpp"
#include "./Frame/FECFrame.hpp"

#include "./Tools/Parameters/Parameters.hpp"

#include "./Processing/PPM/mod/PPM_mod.hpp"
#include "./Processing/Sampling/Up/UpSampling.hpp"
#include "./Processing/IQ/Insertion/IQ_Insertion.hpp"

#include "./Radio/Emitter/Library/EmitterLibrary.hpp"

#include "./Frontend/Library/FrontendLibrary.hpp"

#include "./Chains/ADBS-like/Encoder/Encoder_ADBS_like_chain.hpp"
#include "./Chains/ADBS-like-fec/Encoder/Encoder_ADBS_FEC_chain.hpp"

#include "couleur.h"

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

    param.set("frontend",       "BinaryFile");
    param.set("frontend_opt",   "../images/file.bin");

    param.set("mode_radio", "radio");
    param.set("filename",   "hackrf");

    param.set("fc",      433000000.0);
    param.set("fe",        1000000.0);
    param.set("fe_real",   1000000.0);

    param.set("surEch",     1);
    param.set("tx_gain",   24);
    param.set("antenna",    1);
    param.set("amplifier",  0);

    param.set("payload",   60);

    param.set("sleep_time",   100000);

    param.set("verbose",   false);

    param.set("max_frames",   1000000);

    param.set("crystal_correct",   0);

    param.set("crystal_correct",   0);

    param.set("enable_fec",       false);

    static struct option long_options[] =
            {
                    {"radio",       required_argument, NULL, 'r'}, // a partir d'un fichier
                    {"file",        required_argument, NULL, 'f'}, // a partir d'un fichier
                    {"file-stream", required_argument, NULL, 'F'}, // a partir d'un fichier

                    {"fc",       required_argument, NULL, 'c'}, // changer la frequence de la porteuse
                    {"fe",       required_argument, NULL, 'e'}, // changer la frequence echantillonnage

                    {"payload",     required_argument, NULL, 'p'}, // changer la frequence de la porteuse

                    {"Frontend",    required_argument, NULL, 'k'}, // changer la frequence de la porteuse
                    {"frontend_opt",    required_argument, NULL, 'l'}, // changer la frequence de la porteuse

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

    std::vector<int8_t> buffer(200000); // Notre buffer à nous dans le programme

    cout << "par Bertrand LE GAL - Octobre 2020" << endl;
    cout << "==================================== ADSB ====================================" << endl;
    printf("%s", KRED);

    while ((c = getopt_long(argc, argv, "be:p:f:n:s:vt8k:l:", long_options, &option_index)) != -1) {
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

            case 'k':
                param.set("frontend", optarg);
                break;
            case 'l':
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

            case 'Z' :
                param.set("enable_fec", true);
                break;

            case 'z' :
                param.set("enable_fec", false);
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
    Emitter*   radio = EmitterLibrary::allocate( param );

    radio->initialize();
    radio->start_engine();
    radio->set_txvga_gain( param.toInt("tx_gain") );
    usleep( param.toInt("sleep_time") );

    Frontend* source = FrontendLibrary::allocate( param );

    //
    // On cree les modules qui vont nous être utile pour réaliser la communication
    //

//    Frame    f( param.toInt("payload") );
//    PPM_mod ppm;
//    UpSampling up( 2 * param.toInt("surEch") ); // 2 necessaire pour le recepteur x fois pour le DAC
//    IQ_Insertion iqi;


    //
    // On cree les buffers que lesquels les differents processing vont etre appliqués
    //

//    std::vector<uint8_t> buff_1( f.frame_bits() );
//    std::vector<int8_t>  buff_2( f.frame_bits() );
//    std::vector<int8_t>  buff_3( 2 * f.frame_bits() );
//    std::vector<int8_t>  buff_4( 4 * f.frame_bits() );


    //
    // On affiche les informations dans le terminal afin de simplifier le replay
    //

    const uint32_t sleep_time = param.toInt("sleep_time");
    const uint32_t payload    = param.toInt("payload");
    const uint32_t verbose    = param.toInt("verbose");

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

    //
    // On ajoute ces données ici
    //
    std::vector<int8_t>  buff_4( 8192 );

    for(uint32_t i = 0; i < buff_4.size(); i += 1)
        buff_4[i] = rand()%5 - 3;

    for(uint32_t i = 0; i < 4; i += 1)
        radio->emission ( buff_4 );

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
    // On ouvre l'image que l'utilisateur souhaite transmettre et on affiche ses propriétés
    //



    //
    // On lance le processus de communication
    //

    auto start = std::chrono::system_clock::now(); // This and "end"'s type is std::chrono::time_point

    uint32_t nFrames = 0;

    FECFrame F( param.toInt("payload") );
    Encoder_chain* enc_chain;
    if( param.toBool("enable_fec") == false ) {
        enc_chain = new Encoder_ADBS_like_chain( F.size_frame() );
        std::cout << "(II) # of input  samples Encoder_ADBS_like_chain (pld) = " << enc_chain->ibuffer_size() << endl;
        std::cout << "(II) # of output samples Encoder_ADBS_like_chain (I/Q) = " << enc_chain->obuffer_size() << endl;
    }else {
        enc_chain = new Encoder_ADBS_FEC_chain( F.size_frame() );
        std::cout << "(II) # of input  samples Encoder_ADBS_FEC_chain (pld) = " << enc_chain->ibuffer_size() << endl;
        std::cout << "(II) # of output samples Encoder_ADBS_FEC_chain (I/Q) = " << enc_chain->obuffer_size() << endl;
    }
    buff_4.resize( enc_chain->obuffer_size() );

    uint64_t data_pos = 0;
    while( isFinished == false )
    {
        source->execute( &F ); // On fill les donnees de la trames avec des données de l'image
        if (verbose == 1){
            F.dump();
        }

        isFinished = !source->is_alive();

        enc_chain->execute(F.data(), &buff_4);
        radio->emission ( buff_4 );

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

    radio->stop_engine();
    radio->close();
    // delete source;
    delete radio;

	return 0;
}
