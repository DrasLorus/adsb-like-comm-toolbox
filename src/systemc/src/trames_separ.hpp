/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _trames_separ_
#define _trames_separ_

#include "systemc.h"
#include <cstdint>
#include "constantes.hpp"

using namespace std;
// #define _DEBUG_SYNCHRO_

SC_MODULE(trames_separ)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_uint<8> > e;
    sc_fifo_in <bool> detect;
    sc_fifo_out< sc_uint<8> > s;

	SC_CTOR(trames_separ)
	{
		SC_CTHREAD(do_gen, clock.pos());
        reset_signal_is(reset,true);
	}

private:

	void do_gen( )
	{
// #ifdef _DEBUG_SYNCHRO_
	      // uint64_t counter = 0;
// #endif
        // float buffer[32];
// #pragma HLS ARRAY_PARTITION variable=buffer complete dim=0
        while( true )
        {
            const uint8_t data = e.read();
            const bool valid = detect.read();
            if( valid)
            {
                const int factor = 2 * 2; // PPM modulation + UpSampling(2)
                for(uint16_t i = 0; i < factor * _BITS_HEADER_; i += 1)
                {
                    s.write( e.read() );
                    detect.read();
                }
                for(uint16_t i = 0; i < factor * _BITS_PAYLOAD_; i += 1)
                {
                    s.write( e.read() );
                    detect.read();
                }
                for(uint16_t     i = 0; i < factor * _BITS_CRC_; i += 1)
                {
                    s.write( e.read() );
                    detect.read();
                }

            }

        }
	}
};

#endif
