////////////////////////////////////////////////////////////////////////////////
/// \copiright ox223252, 2018
///
/// This program is free software: you can redistribute it and/or modify it
///     under the terms of the GNU General Public License published by the Free
///     Software Foundation, either version 2 of the License, or (at your
///     option) any later version.
///
/// This program is distributed in the hope that it will be useful, but WITHOUT
///     ANY WARRANTY; without even the implied of MERCHANTABILITY or FITNESS FOR
///     A PARTICULAR PURPOSE. See the GNU General Public License for more
///     details.
///
/// You should have received a copy of the GNU General Public License along with
///     this program. If not, see <http://www.gnu.org/licenses/>
////////////////////////////////////////////////////////////////////////////////

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

#include <time.h>
#include <stdio.h>

int serverInit ( const uint16_t port, const uint16_t nbMaxClient )
{
	int server;
	struct sockaddr_in servAddr;

	// Créaation de la socket
	server = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( server < 0 )
	{
		return ( -__LINE__ );
	}

	memset ( &servAddr, 0, sizeof ( struct sockaddr_in ) );
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons ( port );
	servAddr.sin_addr.s_addr = htonl ( INADDR_ANY );

	if ( bind ( server, ( struct sockaddr * ) &servAddr, sizeof ( servAddr ) ) )
	{
		close ( server );
		return ( -__LINE__ );
	}

	if ( listen ( server, nbMaxClient ) )
	{
		close ( server );
		return ( -__LINE__ );
	}

	return ( server );
}

int serverAccept ( const int server )
{
	struct sockaddr_in from = { 0 };
	int addrlen = sizeof ( from );

	return ( accept ( server, ( struct sockaddr * ) &from, ( socklen_t * ) &addrlen ) );
}

int recvTimed ( const int from, char * const buffer, const uint64_t size,
	uint64_t * const usec )
{
	struct timeval timeout;
	timeout.tv_sec = ( usec != NULL ) ? (*usec) / 1000000 : 0;
	timeout.tv_usec = ( usec != NULL ) ? (*usec) % 1000000 : 0;

	fd_set readfd;
	int32_t result;

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
		return ( -__LINE__ );
	}

	hostinfo = ( struct hostent * ) gethostbyname ( addr );

	if ( hostinfo == NULL )
	{
		return ( -__LINE__ );
	}

	sin.sin_addr = *( struct in_addr * ) hostinfo->h_addr ;
	sin.sin_port = htons ( port );
	sin.sin_family = AF_INET;
	if ( connect ( sock, ( struct sockaddr * ) &sin, sizeof ( struct sockaddr ) ) == -1 )
	{
		return ( -__LINE__ );
	}
	return sock;
}

// exemple of client/serveur
// first launch it start as server
// next ones start as client, and connect to the server
int __attribute__((weak)) main ( void )
{
	char serverAddr[] = "127.0.0.1\0";
	uint16_t serverPort = 6666;
	
	int socket = 0;
	
	if ( socket = clientInit ( serverAddr, serverPort ), socket > 0 )
	{ // client only part
		srand( time ( NULL ) );
		char name[4];
		for ( int i = 0; i <  3; i++ )
		{
			name[ i ] = rand ( ) % 26 + 'a';
		}
		name[ 3 ] = 0;

		printf ( "%s\n", name );

		if ( send ( socket, name, 10, 0 ) < 0 )
		{
			printf ( "error on send name\n" );
			return ( __LINE__ );
		}

		while ( 1 )
		{
			if ( send ( socket, "msg", 4, 0 ) < 0 )
			{ // server endded
				close ( socket );
				return ( 0 );
			}
			sleep ( 1 );
		}
	}
	else if ( socket = serverInit ( serverPort, 5 ), socket > 0 )
	{ // server only part
		printf( "Press q to quit\n" );

		int clientMax = 5;
		int clientFd[ 32 ] = { 0 };
		char clientName[ 32 ][ 64 ] = { 0 };

		fd_set rdfs;
		while ( 1 )
		{
			int max = socket;
			FD_ZERO ( &rdfs );
			FD_SET ( STDIN_FILENO, &rdfs ); // add stdin fd
			FD_SET ( socket, &rdfs ); // add server fd

			for ( int i = 0; i < clientMax; i++ ) 
			{
				if ( clientFd[ i ] > 0 )
				{ // add socket of each client
					FD_SET ( clientFd[ i ], &rdfs );
				}

				if ( clientFd[ i ] > max )
				{ // find the higger socket addr
					max = clientFd[ i ];
				}
			}

			if ( select ( max + 1, &rdfs, NULL, NULL, NULL) == -1 )
			{
				perror ( "select()" );
				return ( __LINE__ );
			}

			if ( FD_ISSET ( STDIN_FILENO, &rdfs ) )
			{ // something from standard input : i.e keyboard
				char c;
				while ( c = getchar(), c != '\n' )
				{
					if ( ( c == 'q' ) ||
						( c == 'Q' ) )
					{
						for ( int i = 0; i < clientMax; i++ )
						{ // clean the client socket buffer
							if ( clientFd[ i ] != 0 )
							{
								close ( clientFd[ i ] );
							}
						}
						close ( socket );
						return ( 0 );
					}
				}
				printf( "Press q to quit\n" );
			}
			else if ( FD_ISSET ( socket, &rdfs ) )
			{ // new client request
				printf ( "new client :\n" );
				for ( int i = 0; i < clientMax; i++ )
				{
					if ( clientFd[ i ] == 0 )
					{
						clientFd[ i ] = serverAccept ( socket );
						if ( clientFd[ i ] > 0 )
						{ // new client regitered
							printf ( "client connected : " );
						}
						else
						{
							perror ( "new client" );
							clientFd[ i ] = 0;
							continue;
						}
						
						uint64_t t = 1000000;
						int n = 0;
						if ( n = recvTimed ( clientFd[ i ], clientName[ i ], 64, &t ), n <= 0 )
						{ // after connecting the client sends its name
							close ( clientFd[ i ] );

							clientName[ i ][ n ] = 0;
							clientFd[ i ] = 0;
							
							printf ( "unamed\n" );
							continue;
						}
						printf ( "%s\n", clientName[ i ] );
						break;
					}
				}
			}
			else for ( int i = 0; i < clientMax; i++ )
			{ // a client is talking
				if ( FD_ISSET ( clientFd[ i ], &rdfs ) )
				{
					char buffer[ 64 ] = { 0 };
					uint64_t t = 1000000;
					if ( recvTimed ( clientFd[ i ], buffer, 64, &t ) <= 0 )
					{ // client disconnected
						close ( clientFd[ i ] );

						printf ( "%s disconnected\n", clientName[ i ] );
						clientFd[ i ] = 0; // remove client
						clientName[ i ][ 0 ] = 0; // remove client name
					}
					else
					{
						printf ( "%s send %s\n", clientName[ i ], buffer );
					}
				}
			}
		}
	}

	return ( 0 );
}

#if 0
res : http://broux.developpez.com/articles/c/sockets/#L5
#endif