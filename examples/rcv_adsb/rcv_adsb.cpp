#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"

#include <iostream>

#include <cmath>
#include <complex>
#include <cstring>
#include <chrono>
#include <bitset>

#include <vector>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <signal.h>

#include "type_aircraft.hpp"

#include "../../src/Tools/Parameters/Parameters.hpp"
#include "../../src/Tools/CTickCounter/CTickCounter.hpp"

#include "../../src/Radio/Receiver/Library/ReceiverLibrary.hpp"

//#define _TIME_PROFILE_

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

#include "tools.hpp"
#include "Avion.hpp"

using namespace std;

bool isFinished = false;

void my_ctrl_c_handler(int s) {
    if (isFinished == true) {
        exit(EXIT_FAILURE);
    }
    isFinished = true;
}


/*   ============================== MAIN =========================== */
/*
	4 ech = 1 symb
	1 trame = 120 symb = 480 ech
*/
int main(int argc, char *argv[]) {
    init_crc_lut();

#if 0
    const float ref_latitude  = 44.806884; // latitude  de l'ENSEIRB
    const float ref_longitude = -0.606629; // longitude de l'ENSEIRB
#else
    const float ref_latitude  = 44.820919; // latitude  de la maison
    const float ref_longitude = -0.502448; // longitude de la maison
#endif

    map<int32_t, Avion*> liste_m;
    vector<Avion *> liste_v;

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    uint32_t nbTramesDetectees = 0; // nbre trame ...
    uint32_t nbBonsCRCs      = 0;
    uint32_t nbBonsCRC_init  = 0;
    uint32_t nbBonsCRCs_1x   = 0;
    uint32_t nbBonsCRCs_2x   = 0;
    uint32_t nbBonsCRCs_llr  = 0;
    uint32_t nbStrangeFrames = 0;

    int c; //getopt

    Parameters param;

    param.set("mode_radio", "radio");
    param.set("filename", "usrp");

    param.set("fc", 1090000000.0);
    param.set("fe", 4000000.0);

    param.set("hackrf_amplifier", -1);

    param.set("receiver_gain", -1);

#if defined(__ARM_NEON)
    param.set("mode_conv", "NEON"); // scalar
    param.set("mode_corr", "NEON"); // scalar
#elif defined(__AVX2__)
    param.set("mode_conv", "AVX2"); // scalar
    param.set("mode_corr", "AVX2"); // scalar
#else
    param.set("mode_conv", "scalar"); // scalar
    param.set("mode_corr", "scalar"); // scalar
#endif
    
    param.set("payload", 60);

    param.set("verbose", 0);

    param.set("ps_min", 0.75f);

    param.set("mode_inter", true);
    param.set("dump_frame", false);

    param.set("crystal_correct", 0);

    param.set("list_mode", false);

    param.set("brute_force_1x", false);
    param.set("brute_force_2x", false);
    param.set("brute_force_llr",false);


    static struct option long_options[] =
            {
                    {"verbose",     no_argument,       NULL, 'v'},  // affiche temps sur chaque boucle Np + cplx => abs
                    {"seuil",       required_argument, NULL, 's'},  // pour changer la valeur min de la correlation (synchro)
                    {"np",          required_argument, NULL, 'n'},  // pour changer le nombre de boucle Np (ie nbre echantillon*200000) // Np = 10 => 0.5 s

                    {"conv",        required_argument, NULL, 'c'}, // a partir d'un fichier
                    {"corr",        required_argument, NULL, 'd'}, // a partir d'un fichier

                    {"radio",       required_argument, NULL, 'r'}, // a partir d'un fichier
                    {"file",        required_argument, NULL, 'F'}, // a partir d'un fichier
                    {"file-stream", required_argument, NULL, 'Q'}, // a partir d'un fichier

                    {"fc",          required_argument, NULL, 'f'}, // changer la frequence de la porteuse
                    {"fe",          required_argument, NULL, 'e'}, // changer la frequence echantillonnage

                    {"payload",     required_argument, NULL, 'p'}, // changer la frequence echantillonnage

                    {"amplifier",   required_argument, NULL, 'A'}, // changer la frequence echantillonnage
                    {"rcv_gain",    required_argument, NULL, 'L'}, // changer la frequence echantillonnage
                    {"rcv-gain",    required_argument, NULL, 'L'}, // changer la frequence echantillonnage
                    {"brute-force-1x",  no_argument,       NULL, '1'}, // changer la frequence echantillonnage
                    {"brute-force-2x",  no_argument,       NULL, '2'}, // changer la frequence echantillonnage
                    {"brute-force-llr", no_argument,       NULL, 'X'}, // changer la frequence echantillonnage

                    {"inter",       no_argument,       NULL, 'I'}, // changer la frequence echantillonnage
                    {"intra",       no_argument,       NULL, 'i'}, // changer la frequence echantillonnage
                    {"dump-frames", no_argument,       NULL, 'D'}, // changer la frequence echantillonnage
                    {"ppm",         required_argument, NULL, 'P'}, // changer la frequence echantillonnage
                    {"liste",       no_argument,       NULL, 'l'}, // changer la frequence echantillonnage

                    {NULL, 0,                          NULL, 0}
            };

    int option_index = 0;

    cout << "par Florian LOUPIAS - Février 2020" << endl;
    cout << "par Bertrand LE GAL - Octobre 2020" << endl;
    cout << "==================================== ADSB ====================================" << endl;
    // ============== GETOPT ================
    printf("%s", KRED);

    while ((c = getopt_long(argc, argv, "be:p:f:n:s:vt8", long_options, &option_index)) != -1) {
        switch (c) {
            case 0:
                printf("%soption %s%s", long_options[option_index].name, KNRM, KRED);
                if (optarg)
                    printf("%s with arg %s%s", optarg, KNRM, KRED);
                printf("\n");
                break;

            case 'f':
                param.set("fc", std::stod(optarg));
                break;

            case 'e' :
                param.set("fe", std::stoi(optarg));
                break;

            case 'A' :
                param.set("hackrf_amplifier", std::stoi(optarg));
                break;

            case 'L' :
                param.set("receiver_gain", std::stoi(optarg));
                break;

            case 'p':
                param.set("payload", std::atoi(optarg));
                break;

            case 's':
                param.set("ps_min", std::stof(optarg));
                if ((param.toFloat("ps_min") > 1.0f) || (param.toFloat("ps_min") < 0.0f)) {
                    printf("erreur : --produit_scalaire (ps_min) doit etre compris entre 0 et 1\n");
                    exit(EXIT_FAILURE);
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

            case '1':
                param.set("brute_force_1x", true);
                break;

            case '2':
                param.set("brute_force_2x", true);
                break;

            case 'X':
                param.set("brute_force_llr", true);
                break;

            case 'I':
                param.set("mode_inter", true);
                break;

            case 'i':
                param.set("mode_inter", false);
                break;

            case 'D':
                param.set("dump_frame", true);
                break;

            case 'l':
                param.set("list_mode", true);
                break;

            case 'P' :
                param.set("crystal_correct", std::atoi(optarg));
                printf("%soption crystal_correct = %d dB%s\n", KNRM, param.toInt("crystal_correct"), KRED);
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
                exit(EXIT_FAILURE);
                break;
        }
    }
    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
        exit(EXIT_FAILURE);
    }
    printf("%s", KNRM);
    cout << endl;

    // param.toDouble("fe")/8
    vector<complex<float> > buffer(65536); // Notre buffer à nous dans le programme

    //
    // Selection du module SDR employé dans le programme
    //
    Receiver *radio = ReceiverLibrary::allocate(param);

    //
    // Selection du module de correlation employé dans le programme
    //
    Detector *detect = DetectorLibrary::allocate(param);


    //
    // Selection du module de conversion employé dans le programme
    //
    CplxModule *conv = CplxModuleLibrary::allocate(param);


//    Frame f( param.toInt("payload") );

    printf("(II) Launching the emitter application :\n");
    printf("(II) -> Modulation frequency    : %4d MHz\n", (uint32_t) (param.toDouble("fc") / 1000000.0));
    printf("(II) -> Symbol sampling freq.   : %4d MHz\n", (uint32_t) (param.toDouble("fe") / 1000000.0));
    printf("(II)\n");

    const int32_t crystal_correct_ppm = param.toInt("crystal_correct");
    if (crystal_correct_ppm != 0) {
        double freq_hz = param.toDouble("fc");
        double sample_rate_hz = param.toDouble("fe");

        sample_rate_hz = (uint32_t) ((double) sample_rate_hz * (1000000.0 - crystal_correct_ppm) / 1000000.0 + 0.5);
        freq_hz = freq_hz * (1000000.0 - crystal_correct_ppm) / 1000000.0;

        param.set("fc", freq_hz);
        param.set("fe", sample_rate_hz);

        printf("(II) -> Corrected modulation frequency    : %4d MHz\n", (uint32_t) (param.toDouble("fc") / 1000000.0));
        printf("(II) -> Corrected symbol sampling freq.   : %4d MHz\n", (uint32_t) (param.toDouble("fe") / 1000000.0));
        printf("(II)\n");
    }

    printf("(II) HackRF module configuration :\n");
    printf("(II) -> HackRF antenna parameter   : enable\n");
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

    const float ps_min     = param.toFloat("ps_min");

    const bool brute_force_1x  = param.toBool("brute_force_1x");
    const bool brute_force_2x  = param.toBool("brute_force_2x");
    const bool brute_force_llr = param.toBool("brute_force_llr");

    uint32_t loop_counter = 0;


    std::vector<float> buffer_abs;
    std::vector<float> buffer_detect;

    DownSampling down(2);
    PPM_demod ppd;

    std::vector<uint8_t> buff_6;
    std::vector<uint8_t> buff_7;


    vector<float>   vec_data (480);
    vector<float>   vec_down (240);
    vector<uint8_t> vec_demod(120);
    vector<uint8_t> vec_sync (112);
    vector<uint8_t> vec_pack(14);

    DownSampling      o_down(2);
    PPM_demod         o_ppm;
    RemoveADSBSynchro o_sync;
    BitPacking        o_pack;
    CheckCRC32b       o_ccrc;
    RemoveCRC32b      c_rcrc;


//    Decoder_chain* dec_chain;
//    dec_chain = new Decoder_ADBS_like_chain( F.size_frame() );

    std::vector<uint8_t> frame_buf(112);


#ifdef _TRACE_MODE_
    std::ofstream of( "dec_frames.txt" );
#endif

//#define _DUMP_ALL_SIGNALS_


#ifdef _DUMP_ALL_SIGNALS_
    std::vector<uint8_t> frame_status( buffer_detect.size() );
#endif

    const bool StoreDataSet = param.toBool("dump_frame");

    FILE* file_iq_raw     = nullptr; // Le fichier contenant les données I/Q
    FILE* file_frames_bin = nullptr; // Le fichier contenant les 112 bits correspondant aux frames
    FILE* file_frames_dec = nullptr; // Le fichier contenant les trames décodées
    FILE* file_planes     = nullptr; // Le fichier contenant les informations concernant les avions
    FILE* file_coords     = nullptr; // Le fichier contenant les informations concernant les avions

    if( StoreDataSet == true )
    {
        file_iq_raw     = fopen( "file_iq_raw.cs8", "w" );
        file_frames_bin = fopen( "file_frames_bin.txt", "w" );
        file_frames_dec = fopen( "file_frames_dec.txt", "w" );
        file_planes     = fopen( "file_planes.txt", "w" );
        file_coords     = fopen( "file_coords.txt", "w" );
    }

    bool firstAcq = true;
//    const bool mode_inter = param.toBool("mode_inter");

    uint32_t stream_ptr = 0;

    const uint32_t verbose = param.toInt("verbose");

#if 0
    vector<int32_t> histo(65536);
    for (int i = 0; i < histo.size(); i++) histo[i] = 0;
#endif

    printf("+---------------+--------+--------+-----+--------+-----+----------+-------------+------+--------+-----+-----+-----+---------+---------+-------+-----+\n");
    printf("| Frame | Acqui | Echan. |  Corr. |  DF |   AA   | FTC |    CS    | ALT (ft) | CPRF | LON (deg) | LAT (deg) | Dis | Speed.H | Speed.V | Angle | CRC |\n");
    printf("+---------------+--------+--------+-----+--------+-----+----------+-------------+------+--------+-----------+-----+---------+---------+-------+-----+\n");

    if( file_frames_dec != nullptr )
    {
        fprintf(file_frames_dec, "+---------------+--------+--------+-----+--------+-----+----------+-------------+------+--------+-----+-----+-----+---------+---------+-------+-----+\n");
        fprintf(file_frames_dec, "| Frame | Acqui | Echan. |  Corr. |  DF |   AA   | FTC |    CS    | ALT (ft) | CPRF | LON (deg) | LAT (deg) | Dis | Speed.H | Speed.V | Angle | CRC |\n");
        fprintf(file_frames_dec, "+---------------+--------+--------+-----+--------+-----+----------+-------------+------+--------+-----------+-----+---------+---------+-------+-----+\n");
    }

    while (radio->alive() && (isFinished == false))
    {
//        auto startIter = std::chrono::system_clock::now();

        uint32_t coverage = vec_data.size();
        coverage = (firstAcq == true) ? 0 : coverage;
        firstAcq = false;

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
        bool error_n = radio->reception(buffer, coverage);
        timer.stop_loading();

        if (error_n == false)    // Cela signifie que l'on a rencontré une erreur lors de la
            continue;                   // reception des echantillons

        //
        // CALCUL DU MODULE DES VOIES (I,Q)
        //
        timer.start_conversion();
        conv->execute(&buffer, &buffer_abs);
        timer.stop_conversion();

        //
        // ON LANCE LA FONCTION DE DETECTION SUR L'ENSEMBLE DU BUFFER
        //
        timer.start_detection();
        detect->execute(&buffer_abs, &buffer_detect);
        timer.stop_detection();

        const int dump_decoded_frame = true;
        const int dump_resume = param.toBool("list_mode");

        //
        // ON VA MAINTENANT PARCOURIR LE TABLEAU DE SCORE POUR DETECTER LES TRAMES
        //
        const uint32_t length = (buffer_abs.size() - coverage);
        for (uint32_t k = 0; k < length; k += 1) {

            float score = buffer_detect[k];
            if (score > ps_min)
            {

                //
                // Le score de la position k depasse le score minimum fixé par l'utilisateur
                //

                timer.start_decoding();
                nbTramesDetectees += 1;    // On vient de detecter qqchose

                //
                // On stocke les données pour une utilisation ultérieure
                //
                if( file_iq_raw != nullptr )
                {
                    int8_t dSet[960];
                    float* iSet = (float*)(buffer.data() + k);
                    float maxv = 0;
                    for (int x = 0; x < 960; x += 1)
                        maxv = std::max(maxv, iSet[x]);  // On suppose que les données que l'on récupere sont  -127 <=> +127
                    for (int x = 0; x < 960; x += 1)
                        dSet[x] = (maxv <= 1.0f) ? 127.0f * iSet[x] : iSet[x];  // On suppose que les données que l'on récupere sont  -127 <=> +127
                    fwrite(dSet, sizeof(int8_t), 960, file_iq_raw);
                    for (int x = 0; x < 60; x += 1) // On rajoute un peu de data entre 2 trames
                        dSet[x] = (rand()%6) - 3;
                    fwrite(dSet, sizeof(int8_t), 60, file_iq_raw);
                }

                for (int x = 0; x < vec_data.size(); x += 1)    // On extrait les 120 bits (x2) du vecteur
                    vec_data[x] = buffer_abs[x + k];            // d'echantillons (modules complexes de IQ)

                o_down.execute(vec_data, vec_down);         // 480 values => 240 values
                o_ppm.execute (vec_down, vec_demod);     // 240 values => 120 bits
                o_sync.execute(vec_demod, vec_sync);     // 112 bits
                o_pack.execute(vec_sync, vec_pack);         // 15 octets

                const bool crc_initial = check_crc  <112 / 8>( vec_pack.data() );
#if _LOUPIAS_CRC_TEST_
                bool ok_slow  = CRC(vec_demod.data());
                if ( ok_slow != ok_fast )
                {
                    red();
                    printf(" [ THERE IS SOMETHING STRANGE ])\n");
                    black();
                    printf("  "); for(int32_t i = 0; i < vec_demod.size(); i += 1) printf("%d", vec_demod[i]); printf("\n");
                    printf("  "); for(int32_t i = 0; i < vec_sync.size(); i += 1) printf("%d ", vec_sync[i] ); printf("\n");
                    printf("  "); for(int32_t i = 0; i < vec_pack.size(); i += 1) printf("%2.2X", vec_pack[i]); printf("\n");
                    printf("  brute_crc_flo = %d\n", ok_slow  );
                    printf("  check_crc     = %d\n", ok_fast  );
                }
#endif
                //
                // On stocke les données pour une utilisation ultérieure
                //
                if( file_frames_bin != nullptr ) {
                         if(crc_initial ) fprintf(file_frames_bin, "%5d | OK | [", nbTramesDetectees);
                    else            fprintf(file_frames_bin, "%5d | KO | [", nbTramesDetectees);
                    for (int x = 0; x < vec_sync.size() - 1; x += 1)
                        fprintf(file_frames_bin, "%d,", vec_sync[x]);
                    fprintf(file_frames_bin, "%d]\n", vec_sync[vec_sync.size() - 1]);
                }

                bool crc_brute_1x = false;
                if ( (crc_initial == false) && (brute_force_1x == true) )
                {
                    const int32_t stop_1_bit = 112;
                    for (int j = 0; j < stop_1_bit; j += 1)
                    {
                        flipbit( vec_pack.data(), j );

                        crc_brute_1x = check_crc<112 / 8>( vec_pack.data() );
                        if (crc_brute_1x == true)
                        {
                            vec_demod[j + 8] = !vec_demod[j + 8];       // On inverse le bit
                            vec_sync [j    ] = !vec_sync [j    ];       // On inverse le bit

#if 0
                            yellow();
                            printf("%6d : %6d | [ !!! BIT FLIP TECHNIQUE SAVED A FRAME !!! ] \n", loop_counter, k);
                            black();
#endif
                            break;
                        }

                        flipbit( vec_pack.data(), j );
                    }
                }

#if 0
                if ( (ok_fast == false) && (brute_force == true) )
                {
                    const int32_t stop_1_bit = 112; // Cela implique que l'on corrige un bit dans le CRC si nécessaire
                    for (int j = 0; j < stop_1_bit; j += 1)
                    {
                        vec_demod[j] = !vec_demod[j]; // On inverse le bit

                        const bool brute_crc_flo = CRC(vec_demod.data());

                        if (brute_crc_flo == true)
                        {
                            o_sync.execute(vec_demod, vec_sync); // 112 bits
                            o_pack.execute(vec_sync, vec_pack);    //  15 octets
                            const bool brute_crc = check_crc<112 / 8>(vec_pack.data());

                            if (brute_crc == false) {
                                red();
                                printf(" [THERE IS SOMETHING WRONG] (brute_crc_flo = %d and brute_crc = %d and bit-flip = %d)\n", brute_crc_flo, brute_crc, j);
                                black();
                            }else{
                                yellow();
                                printf("%6d : %6d | [ !!! BIT FLIP TECHNIQUE SAVED A FRAME !!! ] \n", loop_counter, k);
                                black();
                                nbSavedCRCs += 1;
                                ok_fast      = true;
                                o_pack.execute(vec_sync, vec_pack);
                                break;
                            }
                        }
                        vec_demod[j] = !vec_demod[j]; // On re-inverse le bit
                    }
                }
#endif

                
#define _BRUTE_FORCE_2x_
                bool crc_brute_2x = false;
#ifdef _BRUTE_FORCE_2x_
                if ( (crc_initial == false) && (crc_brute_1x == false) && (brute_force_2x == true) )
                {
                    const int32_t stop_1_bit = 112;      //
                    const int32_t stop_2_bit = 112 - 24;

                    for (int ii = 0; ii < stop_1_bit; ii += 1)
                    {
                        flipbit( vec_pack.data(), ii );

                        for (int jj = ii + 1; jj < stop_2_bit; jj += 1)
                        {
                            flipbit( vec_pack.data(), jj );

                            crc_brute_2x  = check_crc<112 / 8>( vec_pack.data() );
                            if (crc_brute_2x == true)
                            {
                                vec_demod[ii + 8] = !vec_demod[ii + 8]; // On inverse le bit
                                vec_sync [ii    ] = !vec_sync [ii    ]; // On inverse le bit
                                vec_demod[jj + 8] = !vec_demod[jj + 8]; // On inverse le bit
                                vec_sync [jj    ] = !vec_sync [jj    ]; // On inverse le bit
#if 0
                                magenta();
                                printf("%6d : %6d | [ !!! 2x BIT FLIP TECHNIQUE SAVED A FRAME !!! ] \n", loop_counter, k);
                                black();
#endif
//                                o_pack.execute(vec_sync, vec_pack);
                                break;
                            }

                            flipbit( vec_pack.data(), jj );
                        }

                        if( crc_brute_2x == true ) break;

                        flipbit( vec_pack.data(), ii );
                    }
                }
#endif

                
#define _BRUTE_FORCE_LLR_
#ifdef  _BRUTE_FORCE_LLR_
                bool crc_brute_llr = false;
                if ( (crc_initial == false) && (crc_brute_1x == false) && (crc_brute_2x == false) && (brute_force_llr == true)  )
                {
                    std::vector<float> vec_score (112);
                    for(int32_t ii = 16; ii < 240; ii += 2)
                    {
                        const float diff = vec_down[ii] - vec_down[ii + 1];
                        const float abso = std::abs(diff);
                        vec_score[(ii / 2) - 8] = abso;
                    }

                    sort(vec_score.begin(), vec_score.end());

                    for(int32_t ii = 0; ii < vec_score.size(); ii += 1)
                        printf("%3d : (%17.13f)\n", ii, vec_score[ii]);
                    
                    crc_brute_llr = true;

                    exit( 0 );
                }
#endif

                const bool crc_is_ok  = ((crc_initial == true)  || (crc_brute_1x == true) || (crc_brute_2x == true));
                nbBonsCRCs     += crc_is_ok;
                nbBonsCRC_init += (crc_initial   == true);
                nbBonsCRCs_1x  += (crc_brute_1x  == true);
                nbBonsCRCs_2x  += (crc_brute_2x  == true);
                nbBonsCRCs_llr += (crc_brute_llr == true);

                if ((crc_is_ok == true) || (crc_is_ok == false && verbose >= 2))
                {
                    string crc_show;
                         if( crc_initial   == true ) crc_show = "\x1B[32mOK\x1B[0m";
                    else if( crc_brute_1x  == true ) crc_show = "\x1B[33mOK\x1B[0m";
                    else if( crc_brute_2x  == true ) crc_show = "\x1B[35mOK\x1B[0m";
                    else if( crc_brute_llr == true ) crc_show = "\x1B[36mOK\x1B[0m";
//                    printf("%d %d %d %d => %s\n", crc_initial, nbBonsCRCs_1x, nbBonsCRCs_2x, nbBonsCRCs_llr, crc_show.c_str());

                    if( verbose >= 1 )
                    {
                        printf("  [");
                        for(int32_t i = 0; i < vec_sync.size(); i += 1)
                            printf("%d ", vec_sync[i] );
                        printf("];\n");
                    }

                    if (crc_is_ok)
                    {

                        const int32_t df_value   = pack_bits(vec_sync.data(), 5);
                        const int32_t type_frame = pack_bits(vec_sync.data() + 32, 5);
                        const int32_t oaci_value = pack_bits(vec_sync.data() + 8, 24);

                        if( df_value != 17 )
                        {
                            nbStrangeFrames += 1;
                            continue;
                        }

                        Avion *ptr_avion = liste_m[oaci_value];
                        if (ptr_avion == nullptr) {
                            ptr_avion = new Avion(oaci_value);
                            liste_m[oaci_value] = ptr_avion;
                            liste_v.push_back(ptr_avion);
                        }
                        ptr_avion->update();
                        ptr_avion->set_score( score );

                        if ((type_frame >= 1) && (type_frame <= 4))
                        {
                            const int32_t type_airc = pack_bits(vec_sync.data() + 37, 3);
                            const int32_t typeAricraft = toCode( type_frame, type_airc );
                            if( typeAricraft == -1 )
                            {
                                nbStrangeFrames += 1;
                                continue;
                            }

                            const char lut[] = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ##### ###############0123456789######";
                            char caractere[9];
                            caractere[0] = lut[ pack_bits(vec_sync.data() + 40, 6) ];
                            caractere[1] = lut[ pack_bits(vec_sync.data() + 46, 6) ];
                            caractere[2] = lut[ pack_bits(vec_sync.data() + 52, 6) ];
                            caractere[3] = lut[ pack_bits(vec_sync.data() + 58, 6) ];
                            caractere[4] = lut[ pack_bits(vec_sync.data() + 64, 6) ];
                            caractere[5] = lut[ pack_bits(vec_sync.data() + 70, 6) ];
                            caractere[6] = lut[ pack_bits(vec_sync.data() + 76, 6) ];
                            caractere[7] = lut[ pack_bits(vec_sync.data() + 82, 6) ];
                            caractere[8] = 0;

                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d | %s |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, caractere, crc_show.c_str());
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d | %s |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, caractere);

                            ptr_avion->set_type(typeAricraft);
                            ptr_avion->set_name(caractere);
                        }

                        if ((type_frame >= 5) && (type_frame <= 8) )
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show.c_str());
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if ((type_frame >= 9) && (type_frame <= 18) )
                        {
                            const int32_t upper = pack_bits(vec_sync.data() + 40, 7);
                            const int32_t incr  = pack_bits(vec_sync.data() + 47, 1);
                            const int32_t lower = pack_bits(vec_sync.data() + 48, 4);
                            int32_t altitude = (upper << 4) | lower;
                            altitude = ( incr == 1) ? (altitude * 25 - 1000) : -1;

                            //
                            // On extrait les informations de la trame
                            //
                            const int32_t CPR_format    = pack_bits(vec_sync.data() + 53, 1);
                            const float f_latitude      = pack_bits_float(vec_sync.data() + 54, 17);//(float)enc_latitude  / 131072.0; // divise par 2^17
                            const float f_longitude     = pack_bits_float(vec_sync.data() + 71, 17);//(float)enc_longitude / 131072.0; // divise par 2^17

                            //
                            // On calcule les parametres reels
                            //

                            float final_lat    = ComputeLatitude (f_latitude,  ref_latitude,                CPR_format);
                            float final_lon    = ComputeLongitude(f_longitude, final_lat,    ref_longitude, CPR_format);
                            const int32_t dist = distance(final_lat, final_lon, ref_latitude, ref_longitude);


                            if (dump_decoded_frame && (dump_resume == false)) {
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |   %6d |    %d |  %8.5f |  %8.5f | %3d |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t)altitude, CPR_format, final_lon, final_lat, dist, crc_show.c_str());
                            }
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |   %6d |    %d |  %8.5f |  %8.5f | %3d |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t)altitude, CPR_format, final_lon, final_lat, dist);

                            ptr_avion->set_altitude (altitude);
                            ptr_avion->set_GNSS_mode(false);
                            ptr_avion->set_latitude (final_lat);
                            ptr_avion->set_longitude(final_lon);
                            ptr_avion->update_distance();
                        }
#if 1
                        if (type_frame == 19)
                        {
                            if ((vec_demod[39 + 6] == 0) & (vec_demod[39 + 7] == 0) & (vec_demod[39 + 8] == 1)) {
                                // Type 1
                                // vitesse horizontale
                                int vNS = 0;
                                for (int q = 0; q < 10; q++) vNS += vec_demod[39 + 26 + q] * pow(2, 10 - q);
                                if (vec_demod[39 + 25] == 1) vNS = 1 - vNS;
                                else vNS = vNS - 1;
                                int vEW = 0;
                                for (int q = 0; q < 10; q++) vEW += vec_demod[39 + 15 + q] * pow(2, 10 - q);
                                if (vec_demod[39 + 25] == 1) vEW = 1 - vEW;
                                else vEW = vEW - 1;
                                float speed = sqrt(vEW * vEW + vNS * vNS);  // en kt ?? plutot en km/h
                                //speed = speed * 1.852;
                                // angle
                                float angle = atan2(vEW, vNS) * 360 / (2 * M_PI);
                                // vitesse verticale
                                int Vr = 0;
                                for (int q = 0; q < 8; q++) Vr += vec_demod[39 + 38 + q] * pow(2, 8 - q);
                                Vr = (Vr - 1) * 64;
                                if (vec_demod[39 + 37] == 1) Vr = -Vr; // en feet/min
                                Vr = Vr * 0.3048;
                                if (dump_decoded_frame && (dump_resume == false))
                                    printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |    %4d |    %4d |  %4d |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t) speed, (int32_t) Vr, (int32_t) angle, crc_show.c_str());
                                if( file_frames_dec != nullptr )
                                    fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |    %4d |    %4d |  %4d |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t) speed, (int32_t) Vr, (int32_t) angle);
                                ptr_avion->set_speed_horizontal(speed);
                                ptr_avion->set_speed_vertical(Vr);
                                ptr_avion->set_angle(angle);
                            }
                        }
#endif
#if 1
                        if ((type_frame >= 20) && (type_frame <= 22) )
                        {
                            int32_t altitude = 0;
                            for (int q = 0; q < 12; q++) {
                                if (q > 8)
                                    altitude += vec_demod[39 + 9 + q] * pow(2, 10 - q - 1);
                                else {
                                    if (q < 8)
                                        altitude += vec_demod[39 + 9 + q] * pow(2, 10 - q); }
                            }
                            altitude = altitude * 25 - 1000;

                            const int32_t CPR_format  = pack_bits      (vec_sync.data() + 53, 1);
                            const float f_latitude    = pack_bits_float(vec_sync.data() + 54, 17);//(float)enc_latitude  / 131072.0; // divise par 2^17
                            const float f_longitude   = pack_bits_float(vec_sync.data() + 71, 17);//(float)enc_longitude / 131072.0; // divise par 2^17
                            const float final_lat     = ComputeLatitude (f_latitude,  ref_latitude,                CPR_format);
                            const float final_lon     = ComputeLongitude(f_longitude, final_lat,    ref_longitude, CPR_format);
                            const int32_t dist        = distance(final_lat, final_lon, ref_latitude, ref_longitude);

                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |   %6d |    %d |  %8.5f |  %8.5f | %3d |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t)altitude, CPR_format, final_lon, final_lat, dist, crc_show.c_str());
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |   %6d |    %d |  %8.5f |  %8.5f | %3d |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, (int32_t)altitude, CPR_format, final_lon, final_lat, dist);
                            ptr_avion->set_altitude (altitude);
                            ptr_avion->set_GNSS_mode(true);
                            ptr_avion->set_latitude (final_lat);
                            ptr_avion->set_longitude(final_lon);
                            ptr_avion->update_distance();
                        }
#endif
                        if ((type_frame >= 23) && (type_frame <= 27))
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show.c_str());
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if (type_frame == 28)
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show.c_str());
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if (type_frame == 29)
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show.c_str());
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if (type_frame == 31)
                        {
                            if (dump_decoded_frame && (dump_resume == false))
                                printf("| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  %s |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame, crc_show.c_str());
                            if( file_frames_dec != nullptr )
                                fprintf(file_frames_dec, "| %5d | %5d | %6d | %1.4f |  %2d | %06X |  %2d |          |          |      |           |           |     |         |         |       |  OK |\n", nbBonsCRCs, loop_counter, k, score, df_value, oaci_value, type_frame);
                        }

                        if( (dump_decoded_frame == false) && (dump_resume == false) )
                        {
                            ptr_avion->print();
                        }
#if 0
                        for(int i = 0; i < vec_demod.size(); i += 1)
                        {
                            if(i ==  8) printf(" ");
                            else if(i == 13) printf(" ");
                            else if(i == 16) printf(" ");
                            else if(i == 40) printf(" ");
                            else if(i == 96) printf(" ");
                            printf("%d", vec_demod[i]);
                        }
                        printf("\n");
#endif
                    } else {
                        for (int i = 0; i < vec_demod.size(); i += 1) {
                            if (i == 8) printf(" ");
                            else if (i == 13) printf(" ");
                            else if (i == 16) printf(" ");
                            else if (i == 40) printf(" ");
                            else if (i == 96) printf(" ");
                            printf("%d", vec_demod[i]);
                        }
                        printf(" ");

                        int oaci = 0;
                        for (int q = 0; q < 24; q++) {
                            oaci += vec_demod[16 + q] * pow(2, 23 - q);
                        }
                        printf(" [OACI : %06X] ", oaci);

                        red();
                        printf(" [CRC KO]\n");
                        black();
                    }

                }
                timer.stop_decoding();
                k += 1; // On pourrait augementer de plus car il y a peu de chance qu'une autre
                        // trame soit présente d'ici 480 ech.
            }
        }
        loop_counter += 1;


        if ( (dump_resume == true) && ( (loop_counter % 32 == 0) || (!radio->alive()) || (isFinished == true)) )
        {
            printf("\x1B[1;1H\x1B[2J");
            const auto stop = std::chrono::system_clock::now();
            const int32_t eTime = chrono::duration_cast<chrono::seconds>(stop - start).count();
            std::cout << "Application runtime : " << eTime << " seconds"
                      << std::endl;
            std::cout << "Number of frames over the detection value      : " << nbTramesDetectees << std::endl;
            std::cout << "Number of frames with validated CRC value      : " << nbBonsCRCs        << std::endl;
            std::cout << " - Number of initially correct  CRC values     : " << nbBonsCRC_init    << std::endl;
            std::cout << " - Number of saved frames with bit-flip (x1)   : " << nbBonsCRCs_1x     << std::endl;
            std::cout << " - Number of saved frames with bit-flip (x2)   : " << nbBonsCRCs_2x     << std::endl;
            std::cout << "Number of discarded frames with strange values : " << nbStrangeFrames   << " (type != 17)" <<std::endl;
            nbStrangeFrames += 1;

            std::cout << std::endl;
            printf("OACI   | TYPE   | NAME     | CORR. SCORE       |  LATITUDE | LONGITUDE | H.SPEED   | V.SPEED   | ANGLE | TYPE | ALTITUDE  | DISTANCE [min,max] | FRAMES | LAST     |\n");
            printf("-------+--------+----------+-------------------+-----------+-----------+-----------+-----------+-------+------+-----------+--------------------+--------+----------|\n");
            for (uint32_t i = 0; i < liste_v.size(); i += 1)
            {
                if( liste_v.at(i)->get_messages() >= 1 )    // Pour filter les bétises...
                    liste_v.at(i)->print();
            }
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << "MESSAGES:" << std::endl;
            printf("|      n       |  t (in s)  |  Corr. |  DF |   AA   | FTC |    CS    | ALT (ft) | CPRF | LON (deg) | LAT (deg) | Speed.H | Speed.V | Angle | CRC |\n");
        }
        //
        // ON GARDE UNE TRACE DU TEMPS D'EXECUTION DE L'ITERATION POUR L'HISTOGRAMME DU REPORT
        //
    }

    //
    // https://mobisoftinfotech.com/tools/plot-multiple-points-on-map/
    //
    if( file_coords != nullptr )
    {
        for (uint32_t ii = 0; ii < liste_v.size(); ii += 1)    // pour tous les avions
        {
            for (uint32_t jj = 0; jj < liste_v.at(ii)->list_long.size(); jj += 1)    // pour tous les positions
            {
                fprintf(file_coords, "%24.22f,%24.22f,#00FF00\n", liste_v.at(ii)->list_lat[jj], liste_v.at(ii)->list_long[jj]);
            }
        }
    }

    printf("\n================================================================\n");
    std::cout << "loading    : " << timer.loading()    << std::endl;
    std::cout << "conversion : " << timer.conversion() << std::endl;
    std::cout << "detection  : " << timer.detection()  << std::endl;
    std::cout << "decoding   : " << timer.decoding()   << std::endl;
    printf("================================================================\n");

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double RTConstr = 1000.0f / (param.toDouble("fe") / buffer.size());
    double avgTime = chrono::duration_cast<chrono::milliseconds>(elapsed).count() / (float) loop_counter;

    printf("\n");
    std::cout << "Nombre d'aquisition réalisées  : " << loop_counter << std::endl;
    std::cout << "Temps moyen par acquisition    : " << avgTime << " ms" << std::endl;
    std::cout << "Constrainte tps réel / iter    : " << RTConstr << " ms" << std::endl;

    std::cout << "Number of frames over the detection value      : " << nbTramesDetectees << std::endl;
    std::cout << "Number of frames with validated CRC value      : " << nbBonsCRCs        << std::endl;
    std::cout << " - Number of initially correct  CRC values     : " << nbBonsCRC_init    << std::endl;
    std::cout << " - Number of saved frames with bit-flip (x1)   : " << nbBonsCRCs_1x     << std::endl;
    std::cout << " - Number of saved frames with bit-flip (x2)   : " << nbBonsCRCs_2x     << std::endl;
    std::cout << "Number of discarded frames with strange values : " << nbStrangeFrames   << " (type != 17)" <<std::endl;

    printf("\n");
    std::cout << "Nombre de trames emises  (frames)  = " << nbBonsCRCs << std::endl;

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

    if( StoreDataSet == true )
    {
        fclose(file_iq_raw    );
        fclose(file_frames_bin);
        fclose(file_frames_dec);
        fclose(file_planes    );
        fclose(file_coords    );
    }

    delete radio;
    delete detect;
    delete conv;

    return 0;
}

#pragma clang diagnostic pop
