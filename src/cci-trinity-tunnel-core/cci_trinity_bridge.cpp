// cci_trinity_tunnel.cpp   chromatic universe  2016-2-2020    william k. johnson
// added support for c++11 .
//                   command line handling ,
//                   prcess signaling ,
//                   interpocess communication
//                   application binding,
//                   sqlite db
// copyright (c) 2017 chromatic universe

//
// tcpproxy_server.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2007 Arash Partow (http://www.partow.net)
// URL: http://www.partow.net/programming/tcpproxy/index.html
//
// Distributed under the Boost Software License, Version 1.0.

// subprocess

#include <cci_trinity_tunnel.h>


using namespace cci_trinity;



//---------------------------------------------------------------------------------------
trinity_bridge::trinity_bridge( boost::asio::io_service& ios ) :  m_downstream_socket_( ios ) ,
                                                                  m_upstream_socket_( ios )
{
        //
}


//---------------------------------------------------------------------------------------
void trinity_bridge::start( const std::string& upstream_host , unsigned short upstream_port )
{
         m_upstream_socket_.async_connect(  ip::tcp::endpoint
                                            (
                                                  boost::asio::ip::address::from_string(upstream_host),
                                                  upstream_port
                                             ) ,
                                             boost::bind(&trinity_bridge::handle_upstream_connect ,
                                             shared_from_this() ,
                                             boost::asio::placeholders::error ) );
}


//------------------------------------------------------------------------------------------
void trinity_bridge::handle_upstream_connect( const boost::system::error_code& error )
{
         if ( !error )
         {
            m_upstream_socket_.async_read_some( boost::asio::buffer
                                              (
                                                  m_upstream_data_ , max_data_length
                                              ) ,
                                              boost::bind
                                              (
                                                  &trinity_bridge::handle_upstream_read ,
                                                  shared_from_this() ,
                                                  boost::asio::placeholders::error ,
                                                  boost::asio::placeholders::bytes_transferred
                                             )
                                            );

            m_downstream_socket_.async_read_some
            (
                 boost::asio::buffer( m_downstream_data_ , max_data_length ) ,
                 boost::bind
                 (
                     &trinity_bridge::handle_downstream_read ,
                     shared_from_this() ,
                     boost::asio::placeholders::error ,
                     boost::asio::placeholders::bytes_transferred
                )
            );
         }
         else
         {
            handle_close();
         }
}


//-----------------------------------------------------------------------------------------
void trinity_bridge::handle_downstream_write( const boost::system::error_code& error )
{
         if ( !error )
         {
            m_upstream_socket_.async_read_some( boost::asio::buffer( m_upstream_data_,max_data_length ) ,
                                              boost::bind
                                              (
                                                 &trinity_bridge::handle_upstream_read ,
                                                 shared_from_this() ,
                                                 boost::asio::placeholders::error ,
                                                 boost::asio::placeholders::bytes_transferred
                                              ) );
         }
         else
         {
            handle_close();
         }
}



//------------------------------------------------------------------------------------------
 void trinity_bridge::handle_downstream_read( const boost::system::error_code& error ,
                                              const size_t& bytes_transferred )
{
         if ( !error )
         {
            async_write(   m_upstream_socket_,
                           boost::asio::buffer( m_downstream_data_ ,
                                                bytes_transferred ) ,
                           boost::bind
                           (
                             &trinity_bridge::handle_upstream_write ,
                             shared_from_this() ,
                             boost::asio::placeholders::error
                           )
                        );
         }
         else
         {
            handle_close();
         }
}



//----------------------------------------------------------------------------------------------
void trinity_bridge::handle_upstream_write( const boost::system::error_code& error )
{
         if ( !error )
         {
            m_downstream_socket_.async_read_some(  boost::asio::buffer( m_downstream_data_ ,
                                                   cci_trinity::max_data_length ) ,
                                                   boost::bind
                                                   (
                                                     &trinity_bridge::handle_downstream_read ,
                                                     shared_from_this() ,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred
                                                   )
                                              );
         }
         else
         {
            handle_close();
         }
}


//-------------------------------------------------------------------------------------------------
void trinity_bridge::handle_upstream_read( const boost::system::error_code& error,
                                           const size_t& bytes_transferred )
 {
         if ( !error )
         {
            async_write(  m_downstream_socket_ ,
                          boost::asio::buffer(  m_upstream_data_,bytes_transferred ) ,
                                                boost::bind
                                                (
                                                  &trinity_bridge::handle_downstream_write ,
                                                  shared_from_this(),
                                                  boost::asio::placeholders::error
                                                )
                                             );
         }
         else
         {
            handle_close();
         }
}



//-----------------------------------------------------------------------------------------------------
void trinity_bridge::handle_close()
{
         boost::mutex::scoped_lock lock( m_mutex_ );

         if( m_downstream_socket_.is_open() )  { m_downstream_socket_.close(); }
         if( m_upstream_socket_.is_open() )    { m_upstream_socket_.close();   }
}





