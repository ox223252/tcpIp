# tcpIp

Small TCP/IP client server to exchange data.

## Usage:

To use it you need to create a client and a server like this:

```C
// client
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "tcpIp.h"

#define BUFSIZE 1024

struct data
{
	uint32_t a;
	uint32_t b;
	float c;
};

void main ( void )
{
	char buffer[BUFSIZE] = { 0 };
	int c = clientInit ( "127.0.0.1", 10000 );
	uint64_t t = 6000000;

	struct data tyu;

	memset ( buffer, '.', BUFSIZE - 1 );

	tyu.a = 1;
	tyu.b = 2;
	tyu.c = 3.0;

	write ( c, &tyu, sizeof ( struct data ) );
	
	recvTimed ( c, buffer, BUFSIZE-1, &t );
	printf ( "%s\n", buffer );
	close ( c );
}
```

```C
// server
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "tcpIp.h"

struct data
{
	uint32_t a;
	uint32_t b;
	float c;
};

int main ( void )
{
	static int server;
	static int client;

	char buff[1024];

	uint64_t timeout = 10000000;
	uint8_t loopCounter = 0;

	struct data tyu;

	server = serverInit ( 10000 );

	while ( ( server < 0 ) &&
		( errno == EADDRINUSE ) &&
		( loopCounter < 62 ) )
	{
		server = serverInit ( 10000 );
		sleep ( 1 );
		loopCounter++;
		printf ( "time out: %2d   \n\e[A", 62 - loopCounter );
	}
	printf ( "\n" );

	if ( server < 0 )
	{
		return ( __LINE__ );
	}
	printf ( "server listenning on port 10000\n" );


	client = serverListen ( server );

	if ( client < 0 )
	{
		return ( __LINE__ );
	}
	printf ( "client connected\n" );

	if ( recvTimed ( client, &tyu, sizeof ( struct data ), &timeout ) <= 0 )
	{
		printf ( "disconnected\n" );
		close ( server );
		close ( client );
		return ( __LINE__ );
	}
	
	printf ( "ok %s\n", buff );
	printf ( "tyu :\n\ta : %d\n\tb : %d\n\tc : %f\n", tyu.a, tyu.b, tyu.c );

	sleep ( 2 );

	write ( client, "Ok", 3 );

	close ( server );
	close ( client );

	return ( 0 );
}
```

```Shell
$ gcc server.c tcpIp.c -o a && ./a

server listenning on port 10000
client connected
tyu :
        a : 1
        b : 2
        c : 3.000000
```

```Shell
$ gcc client.c tcpIp.c -o b && ./b
Ok
```

## Note:
Be care your OS can release the port for 30 to 240 seconds after the effective *close()*. Here in the example I place a timeout of 60 seconds (+2) because my system is set to release port 60 s after the *close()*.