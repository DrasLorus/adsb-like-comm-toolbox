/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _Seuil_calc_
#define _Seuil_calc_

#include "systemc.h"
#include <cstdint>
#include "constantes.hpp"
using namespace std;

// #define _DEBUG_SYNCHRO_

SC_MODULE(Seuil_calc)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_uint<8> > e;
    sc_fifo_out  <bool> detect;

	SC_CTOR(Seuil_calc)
	{
		SC_CTHREAD(do_gen, clock.pos());
        reset_signal_is(reset,true);
	}

private:

	void do_gen( )
	{
// #ifdef _DEBUG_SYNCHRO_
	    // uint64_t counter = 0;
    //  detect.write(false);
// #endif
        float buffer[32];
        // const int factor = 2 * 2; // PPM modulation + UpSampling(2)
#pragma HLS ARRAY_PARTITION variable=buffer complete dim=0
        while( true )
        {
#pragma HLS PIPELINE
            for (int j = 0; j < 32-1; j += 1)
            {
// #pragma HLS PIPELINE
                buffer[j] = buffer[j+1];
            }
            buffer[32-1] = e.read();

            const float ps = buffer[ 0] + buffer[ 1] + buffer[ 4] + buffer[ 5] + // 2 bits à 1 en PPM
                             buffer[14] + buffer[15] + buffer[18] + buffer[19];  // 2 bits à 0 en PPM

            float sum = 0.0;
            for (int j = 0; j < 32; j += 1)
            {
// #pragma HLS PIPELINE
                const float temp = buffer[j];
                const float sqrv = (temp * temp);
                sum += sqrv;
            }
            sum = 8.0f * sum;
            float res = (ps*ps / sum );

            detect.write(res>0.80f);


        }
	}
};

#endif
