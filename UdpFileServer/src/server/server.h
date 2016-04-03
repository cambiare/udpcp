/*
 * Server.h
 *
 *  Created on: Mar 26, 2016
 *      Author: dmingus
 */

#ifndef SERVER_H_
#define SERVER_H_

class Server
{
private:
	int port;
	int bufsz;

protected:
	void initializeUDPServer( );

public:
	Server( int port, int bufsz );
	virtual ~Server();
};

#endif /* SERVER_H_ */
