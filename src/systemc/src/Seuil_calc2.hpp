/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _Seuil_calc2_
#define _Seuil_calc2_

#include "systemc.h"
#include <cstdint>
#include "constantes.hpp"
using namespace std;

// #define _DEBUG_SYNCHRO_

SC_MODULE(Seuil_calc2)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_uint<8> > e;
    sc_fifo_out  <bool> detect;
    // sc_fifo_out  <bool> detect1;


	SC_CTOR(Seuil_calc2)
	{
		SC_CTHREAD(do_gen, clock.pos());
        reset_signal_is(reset,true);
	}

private:

	void do_gen( )
	{

        sc_uint<8> buffer[32];
        sc_uint<22> seuil= 26;
        sc_uint<22> sum = 0;
        sc_uint<8> x0 = buffer[0];
        sc_uint<8> x31 = 0;
        sc_uint<16> x02 = 0;
        sc_uint<16> x312 = 0;
        sc_uint<22> tmp = 0;
#pragma HLS ARRAY_PARTITION variable=buffer complete dim=0
        while( true )
        {
#pragma HLS PIPELINE
            for (uint8_t j = 0; j < 32-1; j += 1)
            {
// #pragma HLS PIPELINE
                buffer[j] = buffer[j+1];
            }
            buffer[32-1] = e.read();
            x31 = buffer[32-1];

            const sc_uint<11> ps = (sc_uint<11>) buffer[ 0] + (sc_uint<11>) buffer[ 1] + (sc_uint<11>) buffer[ 4] + (sc_uint<11>) buffer[ 5] + // 2 bits à 1 en PPM
                             (sc_uint<11>) buffer[14] + (sc_uint<11>) buffer[15] + (sc_uint<11>) buffer[18] + (sc_uint<11>) buffer[19];  // 2 bits à 0 en PPM

            x02 = x0 * x0 ;
            x312 = x31 * x31;
            tmp = (x312 -x02);
            sum +=  8*tmp;
            x0 = buffer[0];
            const sc_uint<22> res_0  = (ps* ps);
            const sc_uint<22> res_1 = sum >> 5;
            const sc_uint<22> res_2 = (res_1 == 0)? (sc_uint<22>)31 :res_1;
            const sc_uint<22> res_3  = res_0 /res_2;
            const bool condition = (res_3> seuil);
            detect.write(condition);



        }
	}
};

#endif
