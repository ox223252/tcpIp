#ifndef __SERVERTCP_H__
#define __SERVERTCP_H__

////////////////////////////////////////////////////////////////////////////////
/// \file serverTcp.h
/// \brief library to create and use a basic TCP server to send and received 
///     datas
/// \author ox223252
/// \date 2017-07
/// \copyright LGPL
/// \version 0.1
/// \warning NONE
/// \bug NONE
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// \fn int serverInit ( const uint16_t port );
///
/// \param [in] const uint16_t port : to specify the port where server will
///     listen
///
/// \brief create a TCP server
///
/// \return 0 : OK
///         -1 : error errno will set
////////////////////////////////////////////////////////////////////////////////
int serverInit ( const uint16_t port );

////////////////////////////////////////////////////////////////////////////////
/// \fn int serverListen ( const int server );
///
/// \param [in] const int server : the server descriptor (return by serverInit)
///
/// \brief taht will get a client for server
///
/// \return < 0 : error, erno is set
///         else : client id
////////////////////////////////////////////////////////////////////////////////
int serverListen ( const int server );

////////////////////////////////////////////////////////////////////////////////
/// \fn int recvTimed ( const int from, char * const buffer,
///     const uint64_t size, uint64_t * const usec );
///
/// \param [in] const int from : sender id
/// \param [out] char * const buffer : buffer t store msg
/// \param [in] const uint64_t size : size max of msg
/// \param [in] uint64_t * const usec :  time ned to wait
///
/// \brief wait until the timeout expired or it received msg
///
/// \detail if ( usec = NULL ) it's an bloked read, if ( *usec = 0 ) it's a non
///     bloking read, else it will wait for read or timeout expired, if a msg
///     arrived before *usec expired, *usec will be updated to store the
///     remaining time.
///
/// \return -2 : timeout expired
///         -1 : error, errno is set
///         else : number of read bytes
////////////////////////////////////////////////////////////////////////////////
int recvTimed ( const int from, void * const buffer, const uint64_t size,
	uint64_t * const usec );

////////////////////////////////////////////////////////////////////////////////
/// \fn int clientInit ( const char * addr, const uint16_t port );
///
/// \param [in] const char * addr :
/// \param [in] const uint16_t port :
///
/// \brief create an TCP client
///
/// \return < 0 : error, erno is set
///         else : client id
////////////////////////////////////////////////////////////////////////////////
int clientInit ( const char * addr, const uint16_t port );

#endif
