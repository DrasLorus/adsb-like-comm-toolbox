#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#include <iostream>

#include <cmath>
#include <complex>
#include <cstring>
#include <chrono>

#include <vector>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <signal.h>

#include "../../src/Tools/Parameters/Parameters.hpp"
#include "../../src/Tools/CTickCounter/CTickCounter.hpp"
#include "../../src/Tools/Conversion/cvt_float_u8.hpp"

#include "../../src/Backend/Library/BackendLibrary.hpp"
#include "../../src/Radio/Receiver/Library/ReceiverLibrary.hpp"

//
//  CplxModule des nombres complexes => module flottant
//

#include "./Processing/CplxModule/Library/CplxModuleLibrary.hpp"
#include "./Processing/Detector/Library/DetectorLibrary.hpp"


//
//  Correlateur permettant de détecter le prologue des trames ADSB
//

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

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

	uint32_t nbTramesDetectees = 0; // nbre trame ...
    uint32_t nbTotalTrames     = 0;
    uint32_t nbBonsCRCs        = 0;

	int c; //getopt

    Parameters param;

    param.set("mode_radio",   "radio");
    param.set("filename",   "hackrf");

    param.set("backend",       "BMPFile");
    param.set("backend_opt",   "image.bmp");

    param.set("fc",      863000000.0);
    param.set("fe",        2000000.0);

    param.set("hackrf_amplifier", -1);
    param.set("hackrf_vga_gain",  -1);
    param.set("hackrf_lna_gain",  -1);
    param.set("rtlsdr_tuner_gain",  0); // mode gain automatique

    param.set("mode_conv",  "AVX2"); // scalar
    param.set("mode_corr",  "AVX2"); // scalar

    param.set("payload", 60);

    param.set("verbose", 0);

    param.set("ps_min", 0.75f);

    param.set("mode_inter",       false);
    param.set("dump_first_frame", false);

    param.set("crystal_correct",   0);

    param.set("enable_fec",       false);

	static struct option long_options[] =
	{
		{"verbose", no_argument,         NULL, 'v'},  // affiche temps sur chaque boucle Np + cplx => abs
		{"seuil",   required_argument,   NULL, 's'},  // pour changer la valeur min de la correlation (synchro)
		{"np",      required_argument,   NULL, 'n'},  // pour changer le nombre de boucle Np (ie nbre echantillon*200000) // Np = 10 => 0.5 s

		{"conv",    required_argument,   NULL, 'c'}, // a partir d'un fichier
		{"corr",    required_argument,   NULL, 'd'}, // a partir d'un fichier

		{"radio",       required_argument,   NULL, 'r'}, // a partir d'un fichier
        {"file",        required_argument,   NULL, 'F'}, // a partir d'un fichier
        {"file-stream", required_argument,   NULL, 'Q'}, // a partir d'un fichier

		{"fc",      required_argument,   NULL, 'f'}, // changer la frequence de la porteuse
        {"fe",      required_argument,   NULL, 'e'}, // changer la frequence echantillonnage

        {"payload", required_argument,   NULL, 'p'}, // changer la frequence echantillonnage

        {"amplifier", required_argument,  NULL, 'A'}, // changer la frequence echantillonnage
        {"vga_gain", required_argument,   NULL, 'V'}, // changer la frequence echantillonnage
        {"lna_gain", required_argument,   NULL, 'L'}, // changer la frequence echantillonnage

        {"inter",   no_argument,         NULL, 'I'}, // changer la frequence echantillonnage
        {"intra",   no_argument,         NULL, 'i'}, // changer la frequence echantillonnage
        {"dumpf",   no_argument,         NULL, 'D'}, // changer la frequence echantillonnage
        {"ppm",  required_argument, NULL, 'P'}, // changer la frequence echantillonnage

        {"fec",     no_argument, NULL, 'Z'}, // changer la frequence echantillonnage
        {"no-fec",  no_argument, NULL, 'z'}, // changer la frequence echantillonnage

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
                param.set("fe",   std::stoi(optarg));
                break;

            case 'A' :
                param.set("hackrf_amplifier",   std::stoi(optarg));
                break;

            case 'V' :
                param.set("hackrf_vga_gain",   std::stoi(optarg));
                break;

            case 'L' :
                param.set("hackrf_lna_gain",   std::stoi(optarg));
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
                param.set("verbose", param.toInt("verbose") + 1);
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

            case 'Q':
                param.set("mode_radio", "file-stream");
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

            case 'Z' :
                param.set("enable_fec", true);
                break;

            case 'z' :
                param.set("enable_fec", false);
                break;

		    default:
			    printf ("?? getopt returned character code 0%o ??\n", c);
                exit( EXIT_FAILURE );
                break;
		}
	}
	if (optind < argc) {
		printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        printf ("\n");
        exit( EXIT_FAILURE );
	}
	printf("%s",KNRM);
	cout << endl;

    vector<complex<float> > buffer   ( param.toDouble("fe")/2 ); // Notre buffer à nous dans le programme
    vector<complex<float> > buffer_fichier;

	//
	// Selection du module SDR employé dans le programme
	//
    Receiver* radio = ReceiverLibrary::allocate( param );

    //
    // Selection du module de correlation employé dans le programme
    //
    Detector* detect = DetectorLibrary::allocate( param );


    //
    // Selection du module de conversion employé dans le programme
    //
    CplxModule* conv = CplxModuleLibrary::allocate( param );


    Backend* dest = BackendLibrary::allocate( param );

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


    //
    // Selection du module de conversion employé dans le programme
    //

    CTickCounter timer;

    const float ps_min = param.toFloat("ps_min");
    uint32_t loop_counter = 0;


    std::vector<float> buffer_abs;
    std::vector<float> buffer_detect;

    DownSampling down(2);
    PPM_demod ppd;

    std::vector<uint8_t> buff_5( 4 * f.frame_bits() );
    std::vector<uint8_t> buff_6;
    std::vector<uint8_t> buff_7;

#if 0
    BMP* bmp = nullptr;
    int32_t i_width  = -1;
    int32_t i_height = -1;
    int32_t curr_x   =  0;
    int32_t curr_y   =  0;

    uint32_t nBytesPerPixel = 0;
    uint32_t nBytesPerLine = 0;
#endif

//    const uint32_t payload = f.payload_size();

#define _NEW_PROC_


    Decoder_chain* dec_chain;
    FECFrame F( param.toInt("payload") );

    if( param.toBool("enable_fec") == false) {
        dec_chain = new Decoder_ADBS_like_chain( F.size_frame() );
        std::cout << "(II) # of input  samples Decoder_ADBS_like_chain (mod) = " << dec_chain->ibuffer_size() << endl;
        std::cout << "(II) # of output samples Decoder_ADBS_like_chain (pld) = " << dec_chain->obuffer_size() << endl;
    }else {
        dec_chain = new Decoder_ADBS_FEC_chain( F.size_frame() );
        std::cout << "(II) # of input  samples Decoder_ADBS_FEC_chain (mod) = " << dec_chain->ibuffer_size() << endl;
        std::cout << "(II) # of output samples Decoder_ADBS_FEC_chain (pld) = " << dec_chain->obuffer_size() << endl;
    }

    std::vector<uint8_t> frame_buf( dec_chain->ibuffer_size() );


#ifdef _TRACE_MODE_
    std::ofstream of( "dec_frames.txt" );
#endif

//#define _DUMP_ALL_SIGNALS_


#ifdef _DUMP_ALL_SIGNALS_
    std::vector<uint8_t> frame_status( buffer_detect.size() );
#endif

    bool firstAcq = true;
    const bool mode_inter = param.toBool("mode_inter");

    uint32_t stream_ptr = 0;
	while( radio->alive() && (isFinished == false) )
	{
	    //
	    uint32_t coverage = dec_chain->ibuffer_size();
        coverage = (firstAcq == true) ? 0 : coverage;
        firstAcq = false;

#if 0
        printf("(II) RADIO RECEPTION (stream = %8u | nSample = %8lu)\n", stream_ptr, buffer.size() - coverage);
#endif

        stream_ptr += buffer.size() - coverage;

        radio->reception(buffer, coverage);

        // CALCUL DU MODULE DES VOIES (I,Q)
        conv->execute( &buffer, &buffer_abs );

#ifdef _DUMP_ALL_SIGNALS_
        frame_status.resize( buffer_abs.size() );
#endif

        // Si on est en mode inter, on lance la detection
        if( mode_inter == true )
        {
            timer.start_detection();
            detect->execute(&buffer_abs, &buffer_detect);
            timer.stop_detection();

#ifdef _DUMP_ALL_SIGNALS_
            FILE* f1 = fopen( "module.cu8", "w");
            std::vector<uint8_t> module( buffer_abs.size() );
            cvt_float_u8::execute( buffer_abs.data(), &module );
            fwrite(module.data(), sizeof(uint8_t), module.size(), f1 );
            fclose( f1 );

//            FILE* f3 = fopen( "det_value.float", "w" );
//            fwrite(buffer_detect.data(), sizeof(float), buffer_detect.size(), f3 );
//            fclose( f3 );

            std::vector<uint8_t> detection( buffer_detect.size() );
            FILE* f2 = fopen( "detection.cu8", "w" );
            for(uint32_t k = 0; k < buffer_detect.size(); k += 1)
            {
                if (buffer_detect[k] >= ps_min)
                {
                    for(uint32_t p = 0; p < dec_chain->ibuffer_size(); p += 1)
                    {
                        detection[k+p] = 200;
                    }
                    k += dec_chain->ibuffer_size() - 1;
                }else {
                    detection[k] = 0;
                }
            }
            fwrite(detection.data(), sizeof(uint8_t), detection.size(), f2 );
            fclose( f2 );
#endif
        }

        //
        //
        //
        const uint32_t verbose =  param.toInt("verbose");
//      const uint32_t length  = (buffer_abs.size() - 4 * f.frame_bits());
        const uint32_t length  = (buffer_abs.size() - coverage);
        uint32_t lastFrame = 0;
        for(uint32_t k = 0; k < length; k += 1)
		{

            //
            //
            //
		    float s;
            if( mode_inter == false ) {
                timer.start_detection();
                detect->execute( buffer_abs.data() + k );
                 s = detect->getValue(0);
                timer.stop_detection();
            }else{
                s = buffer_detect[k];
            }


            if (s > ps_min)
            {
                timer.start_decoding();
                nbTramesDetectees +=1;    // On vient de detecter qqchose

                //frame_buf.resize( 4672/2 );
                cvt_float_u8::execute( buffer_abs.data() + k, &frame_buf );

                std::vector<uint8_t> frame_tmp( F.size_frame() );
                bool ok     = dec_chain->execute(frame_buf, &frame_tmp);
                nbBonsCRCs += ok;

                // On met a jour pour faire les dumps
                F.update( frame_tmp );

                if( (ok == true && verbose >= 2) || (ok == false && verbose >= 1) )
                {
                    printf("%6d [%4d] : ", k, k - lastFrame);
                    printf("%1.3f : ", s);
                    if( ok )
                         { green(); printf("[CRC OK] "); black(); }
                    else { red();   printf("[CRC KO] "); black(); }
                    F.dump();
                }
                lastFrame = k;

                if( ok == true )
                {
                    dest->execute( &F );
#ifdef _TRACE_MODE_
                    of << std::setw(6) << k << " : " << F.to_string() << "   [CRC OK]" << std::endl;
#endif

#ifdef _DUMP_ALL_SIGNALS_
                    for (uint32_t q = 0; q < frame_buf.size(); q++)
                        frame_status[k + q] = 240;
#endif

                    k += (frame_buf.size() - 1); // On saute tous les bits qui composaient notre trame...

                }else{
#ifdef _TRACE_MODE_
                    of << std::setw(6) << k << " : " << F.to_string() << "   [CRC OK]" << std::endl;
#endif

#ifdef _DUMP_ALL_SIGNALS_
                    frame_status[k] = 120;
#endif
                }

                timer.stop_decoding();
            }else{
#ifdef _DUMP_ALL_SIGNALS_
                frame_status[k] = 0;
#endif
            }
        }
        loop_counter += 1;
	}

#ifdef _DUMP_ALL_SIGNALS_
    FILE* f1 = fopen( "frm_status.cu8", "w");
    fwrite(frame_status.data(), sizeof(uint8_t), frame_status.size(), f1 );
    fclose( f1 );
#endif

#ifdef _TRACE_MODE_
    of.close();
#endif


    delete dest;
    delete dec_chain;

    printf("\n================================================================\n");
    std::cout << "loading    : " << timer.loading()     << std::endl;
    std::cout << "conversion : " << timer.conversion()  << std::endl;
    std::cout << "decoding   : " << timer.detection()   << std::endl;
    std::cout << "criterion  : " << timer.decoding()   << std::endl;
	printf("================================================================\n");

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    printf("\n");
    std::cout << "Nombre de trames detectees : " << nbTramesDetectees << std::endl;
    std::cout << "Nombre de trames like      : " << nbTotalTrames     << std::endl;
    std::cout << "Nombre de trames bon crc   : " << nbBonsCRCs        << std::endl;
    printf("\n");
    std::cout << "Nombre de trames emises  (frames)  = " << nbBonsCRCs                                  << std::endl;

printf("%s", KGRN);
    std::cout << "Temps total de reception (seconds) = " << elapsed.count()                             << std::endl;
    std::cout << "Trames par seconde       (frames)  = " << (nbBonsCRCs/elapsed.count())                    << std::endl;
    std::cout << "Débit recu par seconde   (bits)    = " << (nbBonsCRCs/elapsed.count()*f.frame_bits())     << std::endl;
printf("%s", KNRM);

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

#pragma clang diagnostic pop