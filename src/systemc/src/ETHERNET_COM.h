/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include "systemc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../../Tools/BMP.hpp"


SC_MODULE(ETHERNET_COM)
{
private:
	int t[3];

public:
		sc_fifo_in < sc_int<8> > e;

    // sc_fifo_out<unsigned char> s;

	SC_CTOR(ETHERNET_COM)
	{
		SC_THREAD(do_com);
		//sensitive << e;
	}

    void do_com();
 };
