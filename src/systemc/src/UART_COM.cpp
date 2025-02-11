/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include "UART_COM.h"

#include <cstdlib>

using namespace std;

#ifdef MACOS
#include <stdio.h>
	#include <string.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <errno.h>
	#include <paths.h>
	#include <termios.h>
	#include <sysexits.h>
	#include <sys/param.h>
	#include <sys/select.h>
	#include <sys/time.h>
	#include <time.h>
	#include <cassert>
	#include <stdint.h>
	#include <signal.h>
#else
	#include <stdio.h>
	#include <string.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <errno.h>
	#include <paths.h>
	#include <termios.h>
	#include <sysexits.h>
	#include <sys/param.h>
	#include <sys/select.h>
	#include <sys/time.h>
	#include <time.h>
	#include <cassert>
#endif

void UART_COM::do_com(){
    int fileDescriptor = -1;

#ifdef MACOS
    vector<std::string> vs;
    vs.push_back("/dev/cu.usbserial-210274532517B");
    vs.push_back("/dev/cu.usbserial-210274532517A");
    vs.push_back("/dev/cu.usbserial-210274532245B");
    vs.push_back("/dev/cu.usbserial-210274532245A");
    vs.push_back("/dev/cu.usbserial-A503PYMW");
    vs.push_back("/dev/cu.usbserial-210274533335B");
    vs.push_back("/dev/cu.usbserial-210274533335A");
    vs.push_back("/dev/cu.usbserial-210274531985B");
    vs.push_back("/dev/cu.usbserial-210274531985A");
    vs.push_back("/dev/cu.usbserial-210274532494B");
    vs.push_back("/dev/cu.usbserial-210274532494A");

    for (unsigned long i = 0; i < vs.size(); i += 1) {
        std::string s = vs.at(i);
        fileDescriptor = open(s.c_str(), O_RDWR | O_NOCTTY);
        if (fileDescriptor != -1)
            break;
    }

    if (fileDescriptor == -1) {
        printf("(II) Connection to board : FAILED\n");
        exit(0);
    }

    printf("(II) Connection to board : OK\n");

    struct termios theTermios;
    memset(&theTermios, 0, sizeof(struct termios));
    cfmakeraw(&theTermios);
    cfsetspeed(&theTermios, 921600);
    theTermios.c_cflag     = CREAD | CLOCAL;     // turn on READ
    theTermios.c_cflag    |= CS8;
    theTermios.c_cc[VMIN]  = 0;
    theTermios.c_cc[VTIME] = 5;     // 5 sec timeout
    ioctl(fileDescriptor, TIOCSETA, &theTermios);
#else
    fileDescriptor = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY );
    if(fileDescriptor == -1)
    {
        printf("Impossible d'ouvrir ttyUSB1 !\n");
        exit( 0 );
    }
    struct termios t;
    tcgetattr(fileDescriptor, &t); // recupère les attributs
    cfmakeraw(&t); // Reset les attributs
    t.c_cflag     = CREAD | CLOCAL;     // turn on READ
    t.c_cflag    |= CS8;
    t.c_cc[VMIN]  = 0;
    t.c_cc[VTIME] = 100;     // timeout
    cfsetispeed(&t, B921600);
    cfsetospeed(&t, B921600);
    tcsetattr(fileDescriptor, TCSAFLUSH, &t); // envoie le tout au driver
#endif

	#define buff_size 8

    while( true ){

        unsigned char buffer[ buff_size];
        for(int i = 0; i <  buff_size; i += 1)
        	buffer[i] = (unsigned char) e.read();

        int wBytes = write( fileDescriptor, buffer, buff_size * sizeof(unsigned char) );
        assert( wBytes == (buff_size * sizeof(unsigned char)) );


    }
}
