
#include "filetransferclient.h"
#include <stdlib.h>

int main( int argc, char **argv )
{
	char 	*host = argv[1];
	int	 	port = atoi( argv[2] );
	int		burst = atoi( argv[3] );
	int		wait = atoi( argv[4] );
	int		bufsz = atoi( argv[5] );

	FileTransferClient ftc( host, port, burst, wait, bufsz );
//	FileTransferClient ftc( "localhost", 55555 );

	ftc.sendFile( "somefile.txt", "somefile.txt" );
}
