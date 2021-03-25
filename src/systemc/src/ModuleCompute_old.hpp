/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _ModuleCompute_
#define _ModuleCompute_

#include "systemc.h"
#include <cstdint>

SC_MODULE(ModuleCompute)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_int <8> > e;
    sc_fifo_out< sc_uint<8> > s;


	SC_CTOR(ModuleCompute)
	{
		SC_CTHREAD(do_gen, clock.pos());
        reset_signal_is(reset,true);
	}

private:
/*
  template< int XW, int OW, bool OS >
  void ac_sqrt(
    ac_int<XW,false> x,
    ac_int<OW,OS> &sqrt
  )
  {
    const int RW = (XW+1)/2;
    // masks used only to hint synthesis on precision
    ac_int<RW+2, false> mask_d = 0;
    ac_int<RW+2, false> d = 0;
    ac_int<RW, false> r = 0;
    ac_int<2*RW, false> z = x;
    // needs to pick 2 bits of z for each iteration starting from
    // the 2 MSB bits. Inside loop, z will be shifted left by 2 each
    // iteration. The bits of interest are always on the same
    // position (z_shift+1 downto z_shift)
    unsigned int z_shift = (RW-1)*2;
    for (int i = RW-1; i >= 0; i--) {
      r <<= 1;
      mask_d = (mask_d << 2) | 0x3;
      d = mask_d & (d << 2) | ((z >> z_shift) & 0x3 );
      ac_int<RW+2, false> t = (ac_int<RW+2, false>)(d - (( ((ac_int<RW+1, false>)r) << 1) | 0x1));
      if ( !t[RW+1] ) { // since t is unsigned, look at MSB
        r |= 0x1;
        d = mask_d & t;
      }
      z <<= 2;
    }
    sqrt = r;
  }
*/


    unsigned short int_sqrt32(unsigned short x)
    {
        unsigned char res = 0;
        unsigned char add = 0x80;   // 32b = 0x8000
        int i;
        for(i = 0; i < 8; i++) // 32b = 16
        {
            unsigned char temp = res | add;
            unsigned short g2 = temp * temp;
            if (x >= g2)
            {
                res = temp;
            }
            add >>=1;
        }
        return res;
    }

    void do_gen( )
	{
	    while ( true )
        {
//#pragma HLS PIPELINE enable_flush
#if 0
            float breal = e.read();
            float bimag = e.read();
            float resul = sqrt( breal * breal + bimag * bimag);
            s.write( (sc_uint<8>)resul );

#else
            const sc_int < 8> breal = e.read();
            const sc_int < 8> bimag = e.read();
            const sc_uint<16> rr    = (breal * breal);
            const sc_uint<16> ii    = (bimag * bimag);
            sc_uint< 8> rc = int_sqrt32( rr + ii);
            s.write( rc );

#endif
        }
	}

};

#endif
