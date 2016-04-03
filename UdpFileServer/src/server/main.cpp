
#include "server.h"

#include <stdlib.h>

int main( int argc, char **argv )
{
	int port = atoi( argv[1] );
	int bufsz = atoi( argv[2] );

	new Server( port, bufsz );
}
