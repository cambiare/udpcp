/*
 * FileTransferClient.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: dmingus
 */

#include "filetransferclient.h"

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


FileTransferClient::FileTransferClient(
		const char* host,
		int port,
		int burst,
		int wait,
		int bufsz )
{
	this->burst = burst;
	this->wait = wait;
	this->bufsz = bufsz;
	this->packetsTransferred = 0;
	this->inTransfer = false;

	connect( host, port );
}

FileTransferClient::~FileTransferClient()
{

}

bool FileTransferClient::sendFile(
		const char* srcPath,
		const char* dstPath )
{
	char databuf[bufsz];
	char sfsize[128];
	std::ifstream fs;

	std::cout << "send file: " << srcPath << "\n";

	fs.open( srcPath, std::ios::binary | std::ios::ate );
	long fsize = fs.tellg();
	fs.seekg( 0, std::ios::beg );

	std::string msg;

	sprintf( sfsize, "%d", fsize );
	msg = msg + srcPath + ":" + sfsize;

	std::cout << msg << "\n";

	sendto( fd, msg.c_str(), msg.length(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr) );

	inTransfer = true;

	pthread_t t;
	pthread_create( &t, NULL, &timer, (void*)this );

	int ib = 0;
	int count = 0;

	time_t t1, t2;
	time( &t1 );

	for( ; ib < (fsize - bufsz); ib += bufsz )
	{
		count++;
		if( count > burst )
		{
			//std::this_thread::sleep_for( std::chrono::milliseconds( wait ) );
			usleep( wait );
			count = 0;
		}

		fs.read( databuf, bufsz );
		sendto( fd, databuf, bufsz, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr) );

		packetsTransferred++;
	}

	time( &t2 );
	std::cout << "total time: " << (t2 - t1) << "\n";

	inTransfer = false;

	int remainder = fsize - ib;

	std::cout << "wrote: " << (ib+remainder) << " bytes\n";

	fs.read( databuf, remainder );
	sendto( fd, databuf, remainder, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr) );


	return true;
}

void* FileTransferClient::timer( void *_ftc )
{
	FileTransferClient *ftc = (FileTransferClient*)_ftc;

	long lastBytesTransferred = 0;

	for( ;; )
	{
		long bytesTransferred = ftc->getBytesSent();
		long deltaBytesTransferred = bytesTransferred - lastBytesTransferred;

		lastBytesTransferred = bytesTransferred;

		double mbs = (deltaBytesTransferred / 1024.0 / 1024.0);
		std::cout << "TX RATE: " << (mbs * 8 ) << " Mb/s -- " << mbs << "MB/s -- b sent :" << bytesTransferred << "\n";

		if( !ftc->getInTransfer() )
		{
			break;
		}

		usleep( 1000000 );
	}

	pthread_exit( NULL );


}

void FileTransferClient::connect( const char* host, int port )
{
	struct hostent *hp;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return;
	}

	std::cout << "socket created\n";

	memset((char *)&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);

	hp = gethostbyname(host);
	memcpy((void *)&serverAddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	if( bind(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		return;
	}

	std::cout << "socket bound\n";

}

