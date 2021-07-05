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

#include "../../src/Radio/Receiver/Library/ReceiverLibrary.hpp"
#include "../../src/Processing/CplxModule/Library/CplxModuleLibrary.hpp"
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


class Histo
{
private:
    int32_t  _binSize;
    int32_t  _vmin;
    int32_t  _vmax;
    uint64_t _count;
    float    _vsum;

    vector<uint64_t> values;

public:
    Histo(const int32_t binSize = 4)
    {
        _binSize = binSize;
        values.resize( 256 );
        clear();
    }

    void update(vector<uint8_t>& data)
    {
        for(int i = 0; i < data.size(); i++)
        {
            const int32_t index = (int32_t)data[i];
            _vmax = (_vmax >= index) ? _vmax : index;
            _vmin = (_vmin >= index) ? _vmin : index;
            _vsum = _vsum + (index - 127);                  // WARNING IT IS NOT THE SAME EVERYTIMES
            values[ index ] += 1;
        }
        _count += data.size();
    }

    void update(vector<int8_t>& data)
    {
        for(int i = 0; i < data.size(); i++)
        {
            const int32_t index = ((int32_t)data[i]) + 128;
            _vmax = (_vmax >= index) ? _vmax : index;
            _vmin = (_vmin >= index) ? _vmin : index;
            _vsum = _vsum + (index - 128);                  // WARNING IT IS NOT THE SAME EVERYTIMES
            values[ index ] += 1;
        }
        _count += data.size();
    }

    void update(vector<float>& data)
    {
        for(int i = 0; i < data.size(); i++)
        {
            assert( data[i] <= +1.0f );
            assert( data[i] >= -1.0f );
            const int32_t index = ((data[i] * 127.0f) + 127.0f);
            _vmax = (_vmax >= index) ? _vmax : index;
            _vmin = (_vmin >= index) ? _vmin : index;
            _vsum = _vsum + (index - 127);                  // WARNING IT IS NOT THE SAME EVERYTIMES
            values[ index ] += 1;
        }
        _count += data.size();
    }

    void update(vector<complex<float>>& data)
    {
        for(int i = 0; i < data.size(); i++)
        {
            assert( data[i].real()  <= +1.0f );
            assert( data[i].real()  >= -1.0f );
            const int32_t index = ((data[i].real() * 127.0f) + 127.0f);
            _vmax = (_vmax >= index) ? _vmax : index;
            _vmin = (_vmin >= index) ? _vmin : index;
            _vsum = _vsum + (index - 127);                  // WARNING IT IS NOT THE SAME EVERYTIMES
            values[ index ] += 1;
//            cout << "data[i].real() = " << data[i].real() << " and index = " << index << std::endl;
        }
        _count += data.size();
    }

    void clear()
    {
        _vmin  = +127;
        _vsum  =    0;
        _vmax  = -127;
        _count =    0;

        for(int i = 0; i < values.size(); i++)
            values[i] = 0;
    }

    void stats()
    {
        //
        // On sous echantillonne notre vecteur
        //
        vector<int64_t> binValues( values.size() / _binSize );
        for(int32_t i = 0; i <  values.size(); i += _binSize)
        {
            int64_t tSum = 0;
            for(int32_t j = 0; j <  _binSize; j += 1)
            {
                tSum += values[i + j];
            }
            binValues[i/_binSize] = tSum;
        }

        int64_t sumv = 0;
        for(int32_t i = 0; i <  binValues.size(); i++)
        {
            sumv += binValues[i];
        }
        int32_t first = 0;
        for(int32_t i = 0; i <  binValues.size(); i++)
        {
            if(binValues[i] != 0) break;
            first = i;
        }
        int last = binValues.size() - 1;
        for(int32_t i = binValues.size() - 1; i >= 0; i -= 1)
        {
            if(binValues[i] != 0) break;
            last = i;
        }
        for(int32_t i = 0; i <  binValues.size(); i++)
        {
            binValues[i] = (10000 * binValues[i]) /  sumv; // 100 * 100 pour garder 2 digits frac.
        }
        cout << "---------------------------------------------------" << endl;
        for(int32_t i = first; i <=  last; i++)
        {
            printf("[%3d:%3d] - %1.3f\n", _binSize * i, _binSize * i + (_binSize -1), ((float)binValues[i])/100.0f);
        }
    }

};



int main(int argc, char* argv[])
{

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

	int c; //getopt

    Parameters param;
    param.set("mode_radio","radio");
    param.set("filename",   "usrp");
    param.set("fc", 1090000000.0);
    param.set("fe",    4000000.0);
    param.set("hackrf_amplifier", -1);
    param.set("receiver_gain",      -1);
    param.set("mode_corr",  "AVX2"); // scalar
    param.set("mode_inter",       true);
    param.set("crystal_correct",   0);


	static struct option long_options[] =
	{
		{"verbose", no_argument,         NULL, 'v'},  // affiche temps sur chaque boucle Np + cplx => abs
		{"seuil",   required_argument,   NULL, 's'},  // pour changer la valeur min de la correlation (synchro)
		{"np",      required_argument,   NULL, 'n'},  // pour changer le nombre de boucle Np (ie nbre echantillon*200000) // Np = 10 => 0.5 s

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


    //
    // Selection du module de conversion employé dans le programme
    //

    radio->initialize();
    radio->start_engine();

    auto start = std::chrono::system_clock::now();


    //
    // Selection du module de conversion employé dans le programme
    //

    uint32_t loop_counter = 0;

    vector<int32_t> histo(1024);

    for(int i = 0; i <  1024; i++) histo[i] = 0;

    Histo mHisto;

	while( radio->alive() && (isFinished == false) )
	{
        auto startIter = std::chrono::system_clock::now();
	    const uint32_t coverage = 0;

        radio->reception(buffer, coverage);

        mHisto.update( buffer );

        if( radio->alive() == false )    // Cela signifie que l'on a rencontré une erreur lors de la
            continue;                   // reception des echantillons

        if( loop_counter%32 == 0 )
        {
            mHisto.stats();
            mHisto.clear();
        }

        //
        // ON GARDE UNE TRACE DU TEMPS D'EXECUTION DE L'ITERATION POUR L'HISTOGRAMME DU REPORT
        //
        auto stopIter = std::chrono::system_clock::now();
        int ms = chrono::duration_cast<chrono::milliseconds>(stopIter - startIter).count();
        histo[ms] += 1;

        loop_counter += 1;
	}


    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double RTConstr = 1000.0f / (param.toDouble("fe") / buffer.size());
    double avgTime = chrono::duration_cast<chrono::milliseconds>(elapsed).count() / (float)loop_counter;

    printf("\n");
    std::cout << "Nombre d'aquisition réalisées : " << loop_counter << std::endl;
    std::cout << "Temps moyen par acquisition   : " << avgTime      << " ms" << std::endl;
    std::cout << "Constrainte tps réel / iter   : " << RTConstr     << " ms" << std::endl;

    printf("\n");
    std::cout << "Temps total : " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << std::endl;
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

	return 0;
}

#pragma clang diagnostic pop