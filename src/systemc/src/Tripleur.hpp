/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _Tripleur_
#define _Tripleur_

#include "systemc.h"
#include <cstdint>

SC_MODULE(Tripleur)
{
public:
    sc_in < bool > clock;
    sc_in < bool > reset;
    sc_fifo_in < sc_int<8> > e;
    sc_fifo_out< sc_int<8> > s1;
    sc_fifo_out< sc_int<8> > s2;
    sc_fifo_out< sc_int<8> > s3;



    SC_CTOR(Tripleur)
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
            s3.write( data );


        }
    }

};

#endif
