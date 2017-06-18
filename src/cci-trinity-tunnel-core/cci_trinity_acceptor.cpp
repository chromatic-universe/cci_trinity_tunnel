// cci_trinity_acceptor.cpp   william k. johnson 2016
//
// added support for c++11 .
//                   command line handling ,
//                   prcess signaling ,
//                   interpocess communication
//                   application binding
// Copyright (c) 2016 chromatic universe

//
// tcpproxy_server.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2007 Arash Partow (http://www.partow.net)
// URL: http://www.partow.net/programming/tcpproxy/index.html
//
// Distributed under the Boost Software License, Version 1.0.

#include <cci_trinity_tunnel.h>


using namespace cci_trinity;

//------------------------------------------------------------------------------------------
trinity_acceptor::trinity_acceptor(     boost::asio::io_service& io_service ,
                                        const std::string& local_host ,
                                        unsigned short local_port ,
                                        const std::string& upstream_host ,
                                        unsigned short upstream_port ) :  m_io_service_( io_service ) ,
                                                                          m_localhost_address
                                                                          (
                                                                            boost::asio::ip::address_v4::from_string
                                                                            (
                                                                              local_host
                                                                            )
                                                                          ) ,
                                                                          m_acceptor_
                                                                          (
                                                                             m_io_service_,
                                                                             ip::tcp::endpoint
                                                                             (
                                                                               m_localhost_address ,
                                                                               local_port
                                                                            )
                                                                          ) ,
                                                                          m_upstream_port_( upstream_port ) ,
                                                                          m_upstream_host_( upstream_host )
{
            //
}


//-------------------------------------------------------------------------------------------
bool trinity_acceptor::accept_connections()
{
            try
            {
               m_session_ = boost::shared_ptr<trinity_bridge>( new trinity_bridge( m_io_service_ ) );
               m_acceptor_.async_accept( m_session_->downstream_socket() ,
                                          boost::bind
                                          (
                                               &trinity_acceptor::handle_accept ,
                                               this ,
                                               boost::asio::placeholders::error
                                          )
                                       );
            }
            catch( std::exception& e )
            {
               std::cerr << "acceptor exception: "
                         << e.what()
                         << "\n";
               return false;
            }

            return true;
}



//----------------------------------------------------------------------------------------------
void trinity_acceptor::handle_accept( const boost::system::error_code& error )
{
            if( !error )
            {
               m_session_->start( m_upstream_host_, m_upstream_port_ );

               if ( !accept_connections() )  { std::cerr << "...failure during call to accept...\n"; }
            }
            else
            {
               std::cerr << "error: "
                         << error.message()
                         << "\n";
            }
}

