#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

int serverInit ( const uint16_t port )
{
	int server;
	struct sockaddr_in servAddr;

	// Créaation de la socket
	server = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( server < 0 )
	{
		return ( -1 );
	}

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons ( port );
	servAddr.sin_addr.s_addr = htonl ( INADDR_ANY );

	if ( bind ( server, ( struct sockaddr * ) &servAddr, sizeof ( servAddr ) ) )
	{
		return ( -1 );
	}
	return ( server );
}

int serverListen ( const int server )
{
	struct sockaddr_in from = { 0 };
	int addrlen = sizeof ( from );

	if ( listen ( server, SOMAXCONN ) )
	{
		return ( -1 );
	}

	return ( accept ( server, ( struct sockaddr * ) &from, ( socklen_t * ) &addrlen ) );
}

int recvTimed ( const int from, char * const buffer, const uint64_t size,
	uint64_t * const usec )
{
	struct timeval timeout;
	timeout.tv_sec = ( usec != NULL ) ? (*usec) / 1000000 : 0;
	timeout.tv_usec = ( usec != NULL ) ? (*usec) % 1000000 : 0;

	fd_set readfd;
	uint8_t result;

	FD_ZERO ( &readfd );
	FD_SET ( from, &readfd );

	if ( usec != NULL )
	{
		result = select ( from + 1, &readfd, NULL, NULL, &timeout );
		*usec = timeout.tv_sec * 1000000 + timeout.tv_usec;
	}
	else
	{
		result = select ( from + 1, &readfd, NULL, NULL, NULL );
	}

	if ( result  == -1 )
	{
		return ( -1 );
	}

	if ( result == 0 )
	{
		return ( -2 );
	}

	memset ( buffer, '\0', size );
	result = recv ( from, buffer, size, MSG_DONTWAIT );

	return ( result );
}

int clientInit ( const char * addr, const uint16_t port )
{
	int sock;
	struct sockaddr_in sin = { 0 };
	struct hostent *hostinfo = NULL;

	sock = socket ( AF_INET, SOCK_STREAM, 0);

	if ( sock == -1 )
	{
		return ( -1 );
	}

	hostinfo = ( struct hostent * ) gethostbyname ( addr );

	if ( hostinfo == NULL )
	{
		return ( -1 );
	}

	sin.sin_addr = *( struct in_addr * ) hostinfo->h_addr ;
	sin.sin_port = htons ( port );
	sin.sin_family = AF_INET;
	if ( connect ( sock, ( struct sockaddr * ) &sin, sizeof ( struct sockaddr ) ) == -1 )
	{
		return ( -1 );
	}
	return sock;
}

#if 0
res : http://broux.developpez.com/articles/c/sockets/#L5
#endif
