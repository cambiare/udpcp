/*
 * Server.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: dmingus
 */

#include "server.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <fstream>

#define BUFSZ	bufsz


Server::Server( int port, int bufsz )
{
	this->port = port;
	this->bufsz = bufsz;

	initializeUDPServer();
}

Server::~Server() {

}

void Server::initializeUDPServer( )
{
	int fd;

	struct sockaddr_in myaddr;
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof( remaddr );

	char	*fileName;
	long	numbytes;

	char buf[BUFSZ];

	char charbuf[BUFSZ];

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			std::cout << "can not create socket: " << "\n";
			return;
	}

	memset( (char*)&myaddr, 0, sizeof(myaddr) );
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	myaddr.sin_port = htons( port );

	std::cout << "binding to port: " << port << "\n";

	if( (bind( fd, (struct sockaddr *)&myaddr, sizeof(myaddr) ) ) < 0 )
	{
		std::cout << "failed to bind to port: " << port << "\n";
		return;
	}

	std::cout << "waiting for file" << "\n";

	int recvlen = recvfrom( fd, charbuf, 1024, 0, (struct sockaddr *)&remaddr, &addrlen );
	charbuf[recvlen] = 0;
	char* p;

	std::cout << recvlen << " --- " << charbuf << "\n";

	p = strtok( charbuf, ":" );
	fileName = p;
	numbytes = atol( strtok( NULL, ":" ) );

	std::cout << "removing file: " << fileName << " " << numbytes << "\n";

	//std::remove( fileName );

	//sendto( fd, charbuf, BUFSZ, 0, (struct sockaddr *)&myaddr, sizeof(myaddr) );

	//std::ofstream of;

	//of.open( fileName, std::ios::binary );


	char *fullmembuf = new char[numbytes];

	long ib = 0;
	int counter = 0;
	for( ; ib < (numbytes-BUFSZ);  )
	{
		recvlen = recvfrom( fd, buf, BUFSZ, 0, (struct sockaddr *)&remaddr, &addrlen );
		//of.write( buf, recvlen );
		memcpy( buf, &fullmembuf[ib], recvlen );
		ib += recvlen;

		if( counter > 10000 )
		{
			std::cout << ib << ": " << recvlen << "\n";
			counter = 0;
		}
		counter++;
	}

	int remainder = numbytes - ib;

std::cout << "read: " << (ib+remainder) << " bytes\n";

	recvlen = recvfrom( fd, buf, remainder, 0, (struct sockaddr *)&remaddr, &addrlen );
	//of.write( buf, remainder );

	//of.close();
}



