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

#include "../../src/Radio/Receiver/Library/ReceiverLibrary.hpp"


bool CRC( const uint8_t trame[120]){

    bitset<25> crc = 0b0000000000000000000000000;
    bitset<25> polynome2 = 0b1111111111111010000001001;
    bitset<25> polynome = 0b1001000000101111111111111;

    for(int q=8; q <33; q++){
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

    if (crc == 0x00000000) return true;
    else return false;
}

//
//  CplxModule des nombres complexes => module flottant
//

#include "../../src/Processing/CplxModule/Library/CplxModuleLibrary.hpp"
#include "../../src/Processing/Detector/Library/DetectorLibrary.hpp"
//#include "../../src/Chains/ADBS-like/Decoder/Decoder_ADBS_like_chain.hpp"
//#include "../../src/Chains/ADBS-like-fec/Decoder/Decoder_ADBS_FEC_chain.hpp"
#include "../../src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.hpp"
#include "../../src/Processing/DataPacking/BitPacking/BitPacking.hpp"
#include "../../src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.hpp"
#include "../../src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.hpp"


//
//  Correlateur permettant de détecter le prologue des trames ADSB
//

//#include "../../src/Frame/Frame.hpp"
#include "../../src/Processing/Sampling/Down/DownSampling.hpp"
#include "../../src/Processing/PPM/demod/PPM_demod.hpp"

#include "../../src/couleur.h"

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

    param.set("mode_radio","radio");
    param.set("filename",   "usrp");

    param.set("fc", 1090000000.0);
    param.set("fe",    4000000.0);

    param.set("hackrf_amplifier", -1);

    param.set("receiver_gain", -1);

    param.set("mode_conv",  "AVX2"); // scalar
    param.set("mode_corr",  "AVX2"); // scalar

    param.set("payload", 60);

    param.set("verbose", 0);

    param.set("ps_min", 0.75f);

    param.set("mode_inter",       true);
    param.set("dump_first_frame", false);

    param.set("crystal_correct",   0);


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
        {"rcv_gain", required_argument,   NULL, 'L'}, // changer la frequence echantillonnage
        {"rcv-gain", required_argument,   NULL, 'L'}, // changer la frequence echantillonnage

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
                param.set("fe",   std::stoi(optarg));
                break;

            case 'A' :
                param.set("hackrf_amplifier",   std::stoi(optarg));
                break;

            case 'L' :
                param.set("receiver_gain", std::stoi(optarg));
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

	// param.toDouble("fe")/8
    vector<complex<float> > buffer   ( 4*65536 ); // Notre buffer à nous dans le programme

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


//    Frame f( param.toInt("payload") );

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

    vector<float>   vec_down (480);
    vector<float>   vec_oppm (240);
    vector<uint8_t> vec_osync(120);
    vector<uint8_t> vec_pack (112);
    vector<uint8_t> vec_final( 14);

    DownSampling      o_down(2);
    PPM_demod         o_ppm;
    RemoveADSBSynchro o_sync;
    BitPacking        o_pack;
    CheckCRC32b       o_ccrc;
    RemoveCRC32b      c_rcrc;


//    Decoder_chain* dec_chain;
//    dec_chain = new Decoder_ADBS_like_chain( F.size_frame() );

    std::vector<uint8_t> frame_buf( 112 );


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

    const uint32_t verbose =  param.toInt("verbose");

    vector<int32_t> histo(1024);
    for(int i = 0; i <  1024; i++) histo[i] = 0;

	while( radio->alive() && (isFinished == false) )
	{
        auto startIter = std::chrono::system_clock::now();

	    //
	    uint32_t coverage = 0;
//	    uint32_t coverage = dec_chain->ibuffer_size();
//      coverage = (firstAcq == true) ? 0 : coverage;
//      firstAcq = false;

#if 0
        printf("(II) RADIO RECEPTION (stream = %8u | nSample = %8lu)\n", stream_ptr, buffer.size() - coverage);
#endif

        stream_ptr += buffer.size() - coverage;
#if 0
        int caff = cnt%64;
        if( caff ==   0 ) printf("Reception .   \r");
        if( caff ==  16 ) printf("Reception ..  \r");
        if( caff ==  32 ) printf("Reception ... \r");
        if( caff ==  48 ) printf("Reception ....\r");
        fflush(stdout);
        cnt += 1;
#endif
        timer.start_loading();
        radio->reception(buffer, coverage);
        timer.stop_loading();

        if( radio->alive() == false )    // Cela signifie que l'on a rencontré une erreur lors de la
            continue;                   // reception des echantillons

        //
        // CALCUL DU MODULE DES VOIES (I,Q)
        //
        timer.start_conversion();
        conv->execute( &buffer, &buffer_abs );
        timer.stop_conversion();

        //
        // ON LANCE LA FONCTION DE DETECTION SUR L'ENSEMBLE DU BUFFER
        //
        timer.start_detection();
        detect->execute(&buffer_abs, &buffer_detect);
        timer.stop_detection();

        //
        // ON VA MAINTENANT PARCOURIR LE TABLEAU DE SCORE POUR DETECTER LES TRAMES
        //
        const uint32_t length  = (buffer_abs.size() - coverage);
        for(uint32_t k = 0; k < length; k += 1)
		{

            float s = buffer_detect[k];
            if (s > ps_min)
            {
                timer.start_decoding();
                nbTramesDetectees +=1;    // On vient de detecter qqchose

//                cvt_float_u8::execute( buffer_abs.data() + k, &vec_down );
                for(int x = 0; x < vec_down.size(); x += 1)
                    vec_down[x] = buffer_abs[x + k];

                o_down.execute(vec_down,   vec_oppm );
                o_ppm.execute (vec_oppm, vec_osync );   // 120 bits
                o_sync.execute(vec_osync, vec_pack);    // 112 bits
                o_pack.execute(vec_pack,   vec_final );    //  15 octets

                bool ok = CRC( vec_osync.data() );

                int sum = 0;
                for (int j=0; j < vec_osync.size(); j = j+1)
                    sum += vec_osync[j];
                if( sum == 0 ) ok = false;

                nbBonsCRCs += ok;

                if( ((sum != 0) || (verbose >= 2))  && ((ok == true) || (ok == false && verbose >= 1)) )
                {
                    printf("%1.3f : 0x", s);
                    if( ok )
                    {
//                        for(int i = 0; i < vec_osync.size(); i += 1)
//                            printf("%d", vec_osync[i]);
                        for(int i = 0; i < vec_final.size(); i += 1)
                            printf("%2.2X", vec_final[i]);

                        int oaci = 0;
                        for (int q=0; q < 24; q++){
                            oaci += vec_osync[16+q] * pow(2,23-q);
                        }
                        printf(" [OACI : %06X] ", oaci);

                        green();
                        printf(" [CRC OK]\n");
                        black();
                    }
                    else
                    {
                        for(int i = 0; i < vec_osync.size(); i += 1)
                        {
                            if(i ==  8) printf(" ");
                            else if(i == 13) printf(" ");
                            else if(i == 16) printf(" ");
                            else if(i == 40) printf(" ");
                            else if(i == 96) printf(" ");
                            printf("%d", vec_osync[i]);
                        }
                        printf(" ");

                        int oaci = 0;
                        for (int q=0; q < 24; q++){
                            oaci += vec_osync[16+q] * pow(2,23-q);
                        }
                        printf(" [OACI : %06X] ", oaci);

                        red();
                        printf(" [CRC KO]\n");
                        black();
                    }
                }
                timer.stop_decoding();
            }
        }
        loop_counter += 1;


        //
        // ON GARDE UNE TRACE DU TEMPS D'EXECUTION DE L'ITERATION POUR L'HISTOGRAMME DU REPORT
        //

        auto stopIter = std::chrono::system_clock::now();
        int ms = chrono::duration_cast<chrono::milliseconds>(stopIter - startIter).count();
        histo[ms] += 1;
	}


    printf("\n================================================================\n");
    std::cout << "loading    : " << timer.loading()     << std::endl;
    std::cout << "conversion : " << timer.conversion()  << std::endl;
    std::cout << "detection  : " << timer.detection()   << std::endl;
    std::cout << "decoding   : " << timer.decoding()    << std::endl;
	printf("================================================================\n");

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double RTConstr = 1000.0f / (param.toDouble("fe") / buffer.size());
    double avgTime = chrono::duration_cast<chrono::milliseconds>(elapsed).count() / (float)loop_counter;

    printf("\n");
    std::cout << "Nombre d'aquisition réalisées : " << loop_counter << std::endl;
    std::cout << "Temps moyen par acquisition   : " << avgTime      << " ms" << std::endl;
    std::cout << "Constrainte tps réel / iter   : " << RTConstr     << " ms" << std::endl;

    std::cout << "Nombre de trames detectees    : " << nbTramesDetectees << std::endl;
    std::cout << "Nombre de trames like         : " << nbTotalTrames     << std::endl;
    std::cout << "Nombre de trames bon crc      : " << nbBonsCRCs        << std::endl;
    printf("\n");
    std::cout << "Nombre de trames emises  (frames)  = " << nbBonsCRCs                                  << std::endl;

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


    int sumv = 0;
    for(int i = 0; i <  1024; i++)
    {
        sumv += histo[i];
    }
    int first = 0;
    for(int i = 0; i <  1024; i++)
    {
        if(histo[i] != 0) break;
        first = i;
    }
    int last = 0;
    for(int i = 0; i <  1024; i++)
    {
        if(histo[1023-i] != 0) break;
        last = 1023 - i;
    }
    for(int i = 0; i <  1024; i++)
    {
        histo[i] = (10000 * histo[i]) /  sumv;
    }
    for(int i = first; i <=  last; i++)
    {
        printf("%4d - %1.3f\n", i, ((float)histo[i])/100.0f);
    }


    delete radio;
    delete detect;
    delete conv;

	return 0;
}

#pragma clang diagnostic pop
