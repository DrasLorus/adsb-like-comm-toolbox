/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
 #ifndef _CRCCheck_
 #define _CRCCheck_

#include "systemc.h"
#include <cstdint>
#include "constantes.hpp"


SC_MODULE(CRCCheck)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_uint<8> > e;
    sc_fifo_out< sc_uint<8> > s;

	SC_CTOR(CRCCheck)
	{
		SC_CTHREAD(do_gen, clock.pos());
        reset_signal_is(reset,true);
	}

private:

	void do_gen( )
	{
		cout << "(II) CRCCheck :: START" << endl;
        sc_uint<8> ibuffer[_BYTE_HEADER_ + _BYTE_PAYLOAD_];
        const uint32_t P = 0x82f63b78;
        while( true )
        {
            uint32_t R = 0;
            for (uint32_t i = 0; i < _BYTE_HEADER_ + _BYTE_PAYLOAD_; i += 1)
            {
#pragma HLS PIPELINE
                uint8_t v   = e.read();
                ibuffer[i] = v;
                R          ^= v;
                for (uint32_t j = 0; j < 8; ++j) {
#pragma HLS UNROLL
                    R = R & 1 ? (R >> 1) ^ P : R >> 1;
                }
            }

            uint8_t crc_t[4];
            for (uint32_t i = 0; i < 4; i += 1)
                crc_t[i] = e.read();

            uint32_t crc = 0;
            for (uint32_t i = 0; i < 4; i += 1)
                crc = crc << 8 | crc_t[4-1-i];
#if 0
            printf("0x%8.8X != 0x%8.8X\n", R, crc);
#endif
            if( crc == R )
            {
#if 0
                printf("(DD) CRC validé...\n");
#endif
                for (uint32_t i = 0; i < _BYTE_HEADER_ + _BYTE_PAYLOAD_; ++i) {
#pragma HLS PIPELINE
                    s.write( ibuffer[i] );
                }
            }else{
//                printf("(DD) CRC NON validé !!!\n");
#if 0
                printf("0x%2.2X | ",   ibuffer[0].to_uint());
                printf("0x%2.2X | 0x", ibuffer[1].to_uint());
                for (uint32_t i = _BYTE_HEADER_; i < _BYTE_HEADER_ + _BYTE_PAYLOAD_; ++i)
                    printf("%2.2X", ibuffer[i].to_uint());
                printf(" | 0x%2.2X%2.2X%2.2X%2.2X ", crc_t[0], crc_t[1], crc_t[2], crc_t[3]);
                printf("| 0x%8.8X | 0x%8.8X\n", crc, R);
                exit( 0 );
#endif
            }
        }
	}

};
#endif
