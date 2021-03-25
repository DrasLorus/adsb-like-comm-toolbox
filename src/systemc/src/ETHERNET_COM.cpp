/*
 *  Adder.h
 *  SystemC_SimpleAdder
 *
 *  Created by Le Gal on 07/05/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include "ETHERNET_COM.h"


using namespace std;


void error(const char *msg)
{
    perror(msg);
    exit(0);
}


void ETHERNET_COM::do_com(){
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[4672];

	portno = 57345;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
			error("ERROR opening socket");

	server = gethostbyname("192.168.1.15");
	if (server == NULL) {
			fprintf(stderr,"ERROR, no such host\n");
			exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
			 (char *)&serv_addr.sin_addr.s_addr,
			 server->h_length);

	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
			error("ERROR connecting");


	#define buff_size 4672

    while( true ){

        // unsigned char buffer[ buff_size];
        for(int i = 0; i <  buff_size; i += 1)
        	buffer[i] = (unsigned char) e.read();

	      int wBytes = write( sockfd, buffer, buff_size * sizeof(unsigned char) );
        assert( wBytes == (buff_size * sizeof(unsigned char)) );

}

	if (n < 0)
			 error("ERROR writing to socket");

	close(sockfd);

}
