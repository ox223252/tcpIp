#ifndef __TCPIP_H__
#define __TCPIP_H__
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

////////////////////////////////////////////////////////////////////////////////
/// \file tcpIp.h
/// \brief library to create and use a basic TCP server to send and received 
///     datas
/// \author ox223252
/// \date 2017-07
/// \copyright GPLv2
/// \version 1.0
/// \warning NONE
/// \bug NONE
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// \fn int serverInit ( const uint16_t port, const uint16_t nbMaxClient );
/// \param [in] port : to specify the port where server will listen
/// \param [in] nbMaxClient : nb max of client accepted on the server
/// \brief create a TCP server
/// \return if less than zero function failed else return server file descriptor
///     see errno for more details
////////////////////////////////////////////////////////////////////////////////
int serverInit ( const uint16_t port, const uint16_t nbMaxClient );

////////////////////////////////////////////////////////////////////////////////
/// \fn int serverListen ( const int server );
/// \param [in] const int server : the server descriptor (return by serverInit)
/// \brief taht will get a client for server
/// \return if less than zero function failed else return client file descriptor
///     see errno for more details
////////////////////////////////////////////////////////////////////////////////
int serverAccept ( const int server );

////////////////////////////////////////////////////////////////////////////////
/// \fn int recvTimed ( const int from, char * const buffer,
///     const uint64_t size, uint64_t * const usec );
/// \param [in] const int from : sender id
/// \param [out] char * const buffer : buffer t store msg
/// \param [in] const uint64_t size : size max of msg
/// \param [in] uint64_t * const usec :  time ned to wait
/// \brief wait until the timeout expired or it received msg
/// \detail if ( usec = NULL ) it's an bloked read, if ( *usec = 0 ) it's a non
///     bloking read, else it will wait for read or timeout expired, if a msg
///     arrived before *usec expired, *usec will be updated to store the
///     remaining time.
/// \return -2 : timeout expired
///         -1 : error, errno is set
///         else : number of read bytes
////////////////////////////////////////////////////////////////////////////////
int recvTimed ( const int from, void * const buffer, const uint64_t size,
	uint64_t * const usec );

////////////////////////////////////////////////////////////////////////////////
/// \fn int clientInit ( const char * addr, const uint16_t port );
/// \param [in] const char * addr :
/// \param [in] const uint16_t port :
/// \brief create an TCP client
/// \return if less than zero function failed else return client file descriptor
///     see errno for more details
////////////////////////////////////////////////////////////////////////////////
int clientInit ( const char * addr, const uint16_t port );

#endif
