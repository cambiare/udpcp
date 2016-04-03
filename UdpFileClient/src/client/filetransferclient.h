/*
 * FileTransferClient.h
 *
 *  Created on: Mar 26, 2016
 *      Author: dmingus
 */

#ifndef FILETRANSFERCLIENT_H_
#define FILETRANSFERCLIENT_H_

#include <netinet/in.h>

class FileTransferClient
{
private:
	struct sockaddr_in serverAddr;
	int fd;
	int burst;
	int wait;
	int bufsz;
	long packetsTransferred;
	bool inTransfer;

	void connect( const char* host, int port );

	static void *timer( void *ftc );

public:
	FileTransferClient( const char* host, int port, int burst, int wait, int bufsz );
	virtual ~FileTransferClient();

	bool sendFile( const char* srcPath, const char* dstPath );

	double getBytesSent( ) { return packetsTransferred * bufsz; }

	bool getInTransfer( ) { return inTransfer; }
};

#endif /* FILETRANSFERCLIENT_H_ */
