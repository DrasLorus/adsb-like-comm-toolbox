/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include "systemc.h"

SC_MODULE(UART_COM)
{
private:
	int t[3];

public:
		sc_fifo_in < sc_int<8> > e;

    // sc_fifo_out<unsigned char> s;

	SC_CTOR(UART_COM)
	{
		SC_THREAD(do_com);
		//sensitive << e;
	}

    void do_com();
 };
