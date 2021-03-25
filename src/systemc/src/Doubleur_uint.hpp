/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _DOUBLEUR_U_
#define _DOUBLEUR_U_

#include "systemc.h"
#include <cstdint>

SC_MODULE(DOUBLEUR_U)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_uint<8> > e;
    sc_fifo_out< sc_uint<8> > s1;
    sc_fifo_out< sc_uint<8> > s2;


    SC_CTOR(DOUBLEUR_U)
    {
		SC_CTHREAD(do_gen, clock.pos());
        reset_signal_is(reset,true);
    }

private:

    void do_gen( )
    {
        while ( true )
        {
            const uint8_t data  = e.read();
            s1.write( data );
            s2.write( data );

        }
    }

};

#endif
