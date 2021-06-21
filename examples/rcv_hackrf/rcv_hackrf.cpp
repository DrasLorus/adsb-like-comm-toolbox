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

#include "../../src/Processing/CplxModule/Library/CplxModuleLibrary.hpp"
#include "../../src/Processing/Detector/Library/DetectorLibrary.hpp"


//
//  Correlateur permettant de détecter le prologue des trames ADSB
//

#include "../../src/Frame/Frame.hpp"
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


	return 0;
}

#pragma clang diagnostic pop