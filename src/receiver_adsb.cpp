#include <iostream>

#include <cmath>
#include <complex>
#include <cstring>
#include <chrono>
#include <unistd.h>

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <signal.h>

#include "Tools/Parameters/Parameters.hpp"
#include "Tools/CTickCounter/CTickCounter.hpp"

#include "./Tools/ExportVector/ExportVector.hpp"

//
//  Definition des modules permettant d'utiliser le module Receiver (SdR)
//
#include "./Radio/Emitter/Library/EmitterLibrary.hpp"
#include "./Radio/Receiver/Library/ReceiverLibrary.hpp"
#include "./Processing/CplxModule/Library/CplxModuleLibrary.hpp"
#include "./Processing/Detector/Library/DetectorLibrary.hpp"
#include "./Backend/Library/BackendLibrary.hpp"


#include "./Frame/Frame.hpp"
#include "./Processing/Sampling/Down/DownSampling.hpp"
#include "./Processing/PPM/demod/PPM_demod.hpp"

#include "couleur.h"

using namespace std;

bool isFinished = false;

void my_ctrl_c_handler(int s){
    if( isFinished == true )
    {
        exit( EXIT_FAILURE );
    }
    isFinished = true;
}

void show(std::vector<float>& v)
{
    const uint32_t ll = v.size();
    for(uint32_t i = 0; i < ll; i += 1)
    {
        if( i%16 == 0 )
            printf("\n");
        printf("%1.3f ", v[i]);
    }
    printf("\n");
}



/*   ============================== MAIN =========================== */
/*
	4 ech = 1 symb
	1 trame = 120 symb = 480 ech
*/
int main(int argc, char* argv[])
{
    //
    // Gestion de l'appui sur CTRL+C
    //
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    uint32_t nbTramesDetectees = 0; // nbre trame ...
    uint32_t nbTotalTrames     = 0;
    uint32_t nbBonsCRCs        = 0;
    string path_n;

    int c; //getopt

    Parameters param;

    param.set("mode_radio",   "radio");
    param.set("filename",   "hackrf");

    param.set("fc",      863000000.0);
    param.set("fe",        2000000.0);

    //

    param.set("hackrf_amplifier", -1);
    param.set("hackrf_vga_gain",  -1);
    param.set("hackrf_lna_gain",  -1);
    param.set("hackrf_gain",  -1);

    //

    param.set("rtlsdr_tuner_gain",  0); // mode gain automatique

    param.set("mode_conv",        "AVX2");
    param.set("mode_corr",        "AVX2");

    param.set("payload", 60);

    param.set("verbose", false);

    param.set("ps_min", 0.75f);

    param.set("mode_inter", false);
    param.set("dump_first_frame", false);

    param.set("crystal_correct",   0);
    param.set("file_path", "received_frame.raw");

    static struct option long_options[] =
            {
                    {"verbose", no_argument,         NULL, 'v'},  // affiche temps sur chaque boucle Np + cplx => abs
                    {"seuil",   required_argument,   NULL, 's'},  // pour changer la valeur min de la correlation (synchro)
                    {"np",      required_argument,   NULL, 'n'},  // pour changer le nombre de boucle Np (ie nbre echantillon*200000) // Np = 10 => 0.5 s

                    {"conv",    required_argument,   NULL, 'c'}, // a partir d'un fichier
                    {"corr",    required_argument,   NULL, 'd'}, // a partir d'un fichier

                    {"radio",   required_argument,   NULL, 'r'}, // a partir d'un fichier
                    {"file",    required_argument,   NULL, 'F'}, // a partir d'un fichier

                    {"fc",      required_argument,   NULL, 'f'}, // changer la frequence de la porteuse
                    {"fe",      required_argument,   NULL, 'e'}, // changer la frequence echantillonnage

                    {"amplifier", required_argument,  NULL, 'A'}, // changer la frequence echantillonnage
                    {"vga_gain",  required_argument,   NULL, 'V'}, // changer la frequence echantillonnage
                    {"lna_gain",  required_argument,   NULL, 'L'}, // changer la frequence echantillonnage

                    {"payload", required_argument,   NULL, 'p'}, // changer la frequence echantillonnage

                    {"inter",   no_argument,         NULL, 'I'}, // changer la frequence echantillonnage
                    {"intra",   no_argument,         NULL, 'i'}, // changer la frequence echantillonnage
                    {"dumpf",   no_argument,         NULL, 'D'}, // changer la frequence echantillonnage
                    {"ppm",  required_argument, NULL, 'P'}, // changer la frequence echantillonnage

                    {NULL,      0,                  NULL, 0}
            };

    int option_index = 0;

    cout << "par Florian LOUPIAS - Février 2020" << endl;
    cout << "par Bertrand LE GAL - Octobre 2020" << endl;
    cout << "==================================== ADSB ====================================" << endl;
    // ============== GETOPT ================
    printf("%s",KRED);

    while ((c = getopt_long(argc, argv, "be:p:f:n:s:vt8",long_options, &option_index)) != -1) {
        switch (c) {
            case 0:
                printf ("%soption %s%s", long_options[option_index].name, KNRM, KRED);
                if (optarg)
                    printf ("%s with arg %s%s", optarg, KNRM, KRED);
                printf ("\n");
                break;

            case 'f':
                param.set("fc",   std::stod(optarg));
                break;

            case 'e' :
                param.set("fe",   std::stod(optarg));
                break;

            case 'A' :
                param.set("hackrf_amplifier",   std::stoi(optarg));
                break;

            case 'V' :
                param.set("hackrf_vga_gain",   std::stoi(optarg));
                param.set("hackrf_gain",   std::stoi(optarg));
                break;

            case 'L' :
                param.set("hackrf_lna_gain",   std::stoi(optarg));
                param.set("hackrf_gain",   std::stoi(optarg));
                break;

            case 'p':
                param.set("payload",   std::atoi(optarg));
                break;

            case 's':
                param.set("ps_min",   std::stof(optarg));
                if ( (param.toFloat("ps_min") > 1.0f) || (param.toFloat("ps_min") < 0.0f)) {
                    printf("erreur : --produit_scalaire (ps_min) doit etre compris entre 0 et 1\n");
                    exit( EXIT_FAILURE );
                }
                break;

            case 'v':
                param.set("verbose", true);
                break;

            case '?':
                break;

            case 'c':
                param.set("mode_conv", optarg);
                break;

            case 'd':
                param.set("mode_corr", optarg);
                break;

            case 'r':
                param.set("mode_radio", "radio");
                param.set("filename", optarg);
                break;

            case 'F':
                param.set("mode_radio", "file");
                param.set("filename", optarg);
                break;

            case 'I':
                param.set("mode_inter", true);
                break;

            case 'i':
                param.set("mode_inter", false);
                break;

            case 'D':
                param.set("dump_first_frame", true);
                break;

            case 'P' :
                param.set("crystal_correct",   std::atoi(optarg));
                printf("%soption crystal_correct = %d dB%s\n", KNRM, param.toInt("crystal_correct"), KRED);
                break;

            default:
                printf ("?? getopt returned character code 0%o ??\n", c);
        }
    }
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        printf ("\n");
        exit( EXIT_FAILURE );
    }
    printf("%s",KNRM);
    cout << endl;

    vector<complex<float> > buffer   ( 65536 ); // Notre buffer à nous dans le programme
    vector<complex<float> > buffer_fichier;
    vector<uint8_t        > detection( buffer.size() ); // Notre buffer à nous dans le programme
    vector<int8_t        > detection_int( buffer.size() ); // Notre buffer à nous dans le programme

    //
    // Selection du module SDR employé dans le programme
    //


    Receiver*   radio  = ReceiverLibrary::allocate  (param );
    Detector*   detect = DetectorLibrary::allocate  (param );
    CplxModule* conv   = CplxModuleLibrary::allocate(param );
    // Backend* dest = BackendLibrary::allocate( param );


    // Emitter*   radio_1  = EmitterLibrary::allocate  (param );

    Frame f( param.toInt("payload") );

    printf("(II) Launching the emitter application :\n");
    printf("(II) -> Modulation frequency    : %4d MHz\n",    (uint32_t)(param.toDouble("fc"     )/1000000.0));
    printf("(II) -> Symbol sampling freq.   : %4d MHz\n",    (uint32_t)(param.toDouble("fe"     )/1000000.0));
    printf("(II)\n");

    const int32_t crystal_correct_ppm = param.toInt("crystal_correct");
    if( crystal_correct_ppm != 0 )
    {
        double freq_hz        = param.toDouble("fc");
        double sample_rate_hz = param.toDouble("fe");

        sample_rate_hz = (uint32_t)((double)sample_rate_hz * (1000000.0 - crystal_correct_ppm)/1000000.0+0.5);
        freq_hz        = freq_hz * (1000000.0 - crystal_correct_ppm)/1000000.0;

        param.set("fc",           freq_hz);
        param.set("fe",    sample_rate_hz);

        printf("(II) -> Corrected modulation frequency    : %4d MHz\n",    (uint32_t)(param.toDouble("fc"     )/1000000.0));
        printf("(II) -> Corrected symbol sampling freq.   : %4d MHz\n",    (uint32_t)(param.toDouble("fe"     )/1000000.0));
        printf("(II)\n");
    }

    printf("(II) ADSB-like configuration parameters :\n");
    printf("(II) -> ADSB-like payload length  : %d bytes\n", f.payload_size());
    printf("(II) -> ADSB-like frame length    : %d bits\n",  f.frame_bits());
    printf("(II)\n");
    printf("(II) HackRF module configuration :\n");
//    printf("(II) -> VGA transmitter gain value : %d\n",      param.toInt("tx_gain"));
    printf("(II) -> HackRF antenna parameter   : enable\n" );
    printf("(II) -> HackRF amplifier parameter : disable\n");


    //
    // Selection du module de conversion employé dans le programme
    //


    //
    // Selection du module de conversion employé dans le programme
    //

    radio->initialize();
    radio->start_engine();




    auto start = std::chrono::system_clock::now();




    // On selectionne le module d'emission en fonction des choix de l'utilisateur
    //



    //
    // Selection du module de conversion employé dans le programme
    //

#ifdef _TIME_PROFILE_
    double timer_reception     = 0.0;
    double timer_conv_cplx     = 0.0;
    double timer_detecteur     = 0.0;
    double timer_extraction    = 0.0;
    double timer_down_sampling = 0.0;
    double timer_demodulator   = 0.0;
    double timer_parsing       = 0.0;
#endif

    //
    // Selection du module de conversion employé dans le programme
    //

    CTickCounter timer;

    const float ps_min = param.toFloat("ps_min");
    uint32_t loop_counter = 0;
    const bool verbose          = param.toBool("verbose");
    const bool dump_first_frame = param.toBool("dump_first_frame");

    FILE* stream = fopen("received_frame.cu8", "wb");
    bool firstFrame = true;
    bool emptySlots = true;

    if (stream == nullptr){
        fprintf(stderr, "ReceiverFileRAW::initialize() error during file openning (%s) !\n", "received_frame.cu8");
        exit( EXIT_FAILURE );
    }
    while( radio->alive() && (isFinished == false) )
    {
        timer.start_loading();
        radio->reception(buffer, 0);
        timer.stop_loading();

        timer.start_conversion();
        std::vector<float> buffer_abs;
        conv->execute( &buffer, &buffer_abs );
        timer.stop_conversion();

        //
        // Patch pour le debug du recepteur...
        //
        if( detection.size() != buffer_abs.size() )
            detection.resize( buffer_abs.size() );
            // detection_int.resize( buffer_abs.size() );


        // ============== detection & decodage ================


        std::vector<float> buffer_detect;
        if( param.toBool("mode_inter") == true )
        {
            timer.start_detection();
//            radio->reception(buffer, 0);
            detect->execute(&buffer_abs, &buffer_detect);
            timer.stop_detection();
        }

        uint32_t k = 0;

        const bool mode_inter = param.toBool("mode_inter");
        while ( k <= (buffer_abs.size() - f.frame_bits()) ) {

            float s;
            if( mode_inter == false ) {
                timer.start_detection();
                float *addr = buffer_abs.data() + k;
                detect->execute(addr);
                s = detect->getValue(0);
                timer.stop_detection();
            }else{
                s = buffer_detect[k];
            }

            if (s > ps_min)
            {
                detection[k] = 250;
                // detection_int[k]= 250 -128;
                timer.start_decoding();
                nbTramesDetectees +=1;    // On vient de detecter qqchose

                std::vector<uint8_t> buff_5( 4 * f.frame_bits() );
                for (uint32_t j=0; j < buff_5.size(); j += 1){
                    int32_t v = buffer_abs[k+j];
                    v = (v > +127) ? +127 : v;
                    v = (v < -127) ? -127 : v;
                    buff_5[j]   = v;
                } // BUG BUG BUG ce devrait etre 255 et non 127 !

                std::vector<uint8_t> buff_6(2 * f.frame_bits());
                std::vector<uint8_t> buff_7(8 + 8 * (2 + 16 + 4));

                DownSampling down(2);
                down.execute( buff_5, buff_6 );

                PPM_demod ppd;
                ppd.execute(buff_6, buff_7);

                const bool isOK = f.fill_frame_bits( buff_7 );
                if( isOK )
                {
                    nbTotalTrames += 1;        // C'est bien une trame ADSB-like
                    bool valCRC    = f.validate_crc();
                    nbBonsCRCs    += valCRC;

                    if( verbose == true )
                    {
                        //printf("%6d : %1.3f : ", k, s);
                        printf("%1.3f : ", s);
                        f.dump_frame();
                    }

                    const int fsize = 2 * 2 * f.frame_bits();
                    for(int q = 0; q < fsize - 1; q += 1)
                        detection[k+q] = valCRC ? 250 : 160 ;
                        // detection_int[k+q] = valCRC ? 122 : 32 ;}

                    k += fsize - 1; // On saute tous les bits qui composaient notre trame...
                    // dest->execute( &f );
                }else{
                    detection[k] = 0;
                    // detection_int[k] = 0;

                }
#if 0
                printf("(DD) nbTramesDetectees = %d | nbTotalTrames = %4d  |  nbBonsCRCs = %4d\n", nbTramesDetectees, nbTotalTrames, nbBonsCRCs);
#endif
                timer.stop_decoding();
            }else{
                if( k >= detection.size() )
                {
                    printf("Oups a bug : (buffer_abs.size() = %lu) (f.frame_bits() = %d) (detection.size() = %lu) (k = %d)\n", buffer_abs.size(), f.frame_bits(), detection.size(), k);
                    exit( 0 );
                }
                detection[k] = 0;
                // detection_int[k] = 0;

            }
            k++;
        }

        if( dump_first_frame == true && loop_counter == 0 )
        {
            green();
            printf("(II) Dumping the data set currently processed by the receiver (%lu samples)\n", buffer_abs.size());
            black();
            ExportVector::SaveToCS8(buffer, "Capture.IQSamples.cs8" );
            ExportVector::SaveToU8 (buffer_abs,"Capture.Module.u8");
            ExportVector::SaveToU8 (detection, "Capture.Detect.u8");
            exit( 0 );
        }
        loop_counter += 1;
        if( emptySlots == true || (firstFrame == true) )
        {
            const uint32_t length = detection.size() / 32;
            uint8_t* buff = new uint8_t[length];
            for(uint32_t i = 0; i < length; i += 1)
                buff[i] = (rand()%8) - 4;
            fwrite(buff, length, sizeof(uint8_t), stream);
            delete[]  buff;
        }

        fwrite(detection.data(), detection.size(), sizeof(uint8_t), stream);

        if( emptySlots == true )
        {
            const uint32_t length = detection.size() / 32;
            uint8_t* buff = new uint8_t[length];
            for(uint32_t i = 0; i < length; i += 1)
                buff[i] = (rand()%8) - 4;
            fwrite(buff, length, sizeof(uint8_t), stream);
            delete[]  buff;
        }

        firstFrame = false;

    }
    int8_t buff[4096];
    for(uint32_t i = 0; i < 4096; i += 1)
    buff[i] = (rand()%8) - 4;
    fwrite(buff, 4096, sizeof(int8_t), stream);
    // On ajoute quelques données dans le fichier

    if (stream == nullptr){
        fprintf(stderr, "ReceiverFileRAW::close() error during file close operation (%s) !\n", "received_frame.cu8");
        exit( EXIT_FAILURE );
    }
    if (stream != nullptr){
        fclose( stream );
        stream = nullptr;
    }

    printf("\n================================================================\n");
    std::cout << "loading    : " << timer.loading()     << std::endl;
    std::cout << "conversion : " << timer.conversion()  << std::endl;
    std::cout << "decoding   : " << timer.detection()   << std::endl;
    std::cout << "criterion  : " << timer.decoding()   << std::endl;
    printf("================================================================\n");

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    printf("\n");
    std::cout << "nombre de trames detectees : " << nbTramesDetectees << std::endl;
    std::cout << "nombre de trames like      : " << nbTotalTrames     << std::endl;
    std::cout << "nombre de trames bon crc   : " << nbBonsCRCs        << std::endl;
    printf("\n");
    std::cout << "Nombre de trames emises  (frames)  = " << nbBonsCRCs                                  << std::endl;

    printf("%s", KGRN);
    std::cout << "Temps total d'émission   (seconds) = " << elapsed.count()                             << std::endl;
    printf("%s", KNRM);

    std::cout << "Trames par seconde       (frames)  = " << (nbBonsCRCs/elapsed.count())                    << std::endl;
    std::cout << "Débit emis par seconde   (bits)    = " << (nbBonsCRCs/elapsed.count()*f.frame_bits())     << std::endl;
    std::cout << "Débit utile par seconde  (bytes)   = " << (nbBonsCRCs/elapsed.count()*f.payload_size())   << std::endl;
    std::cout << "Débit utile par seconde  (bits)    = " << (nbBonsCRCs/elapsed.count()*f.payload_size()*8) << std::endl;
    printf("\n");
    std::cout << "Temps total : " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << std::endl;
#ifdef _TIME_PROFILE_
    std::cout << " -  timer_reception     = " << (timer_reception/1000/1000)     << " ms" << std::endl;
    std::cout << " -  timer_conv_cplx     = " << (timer_conv_cplx/1000/1000)     << " ms" << std::endl;
    std::cout << " -  timer_detecteur     = " << (timer_detecteur/1000/1000)     << " ms" << std::endl;
    std::cout << " -  timer_extraction    = " << (timer_extraction/1000/1000)    << " ms" << std::endl;
    std::cout << " -  timer_down_sampling = " << (timer_down_sampling/1000/1000) << " ms" << std::endl;
    std::cout << " -  timer_demodulator   = " << (timer_demodulator/1000/1000)   << " ms" << std::endl;
    std::cout << " -  timer_parsing       = " << (timer_parsing/1000/1000)       << " ms" << std::endl;
#endif
    printf("\n");

    delete radio;
    delete detect;
    delete conv;

    return 0;
}
