// cci_trinity_tunnel.h    copyright  william k. johnson 2016 chromatic universe
//
//added support for c++11 .
//                   command line handling ,
//                   prcess signaling ,
//                   interpocess communication,
//                   application binding,
//                   sqlite db

//Copyright (c) 2007 Arash Partow (http://www.partow.net)
// URL: http://www.partow.net/programming/tcpproxy/index.html
//
// Distributed under the Boost Software License, Version 1.0.
//
//
// Description
// ~~~~~~~~~~~
// The objective of the TCP proxy server is to act as an intermediary
// in order to 'forward' TCP based connections from external clients
// onto a singular remote server.
// The communication flow in the direction from the client to the proxy
// to the server is called the upstream flow, and the communication flow
// in the direction from the server to the proxy to the client is called
// the downstream flow. Furthermore the up and down stream connections
// are consolidated into a single concept known as a bridge.
// In the event either the downstream or upstream end points disconnect,
// the proxy server will proceed to disconnect the other end point
// and eventually destroy the associated bridge.
//
// The following is a flow and structural diagram depicting the
// various elements (proxy, server and client) and how they connect
// and interact with each other.
//
//                                    ---> upstream --->           +---------------+
//                                                     +---->------>               |
//                               +-----------+         |           | Remote Server |
//                     +--------->          [x]--->----+  +---<---[x]              |
//                     |         | TCP Proxy |            |        +---------------+
// +-----------+       |  +--<--[x] Server   <-----<------+
// |          [x]--->--+  |      +-----------+
// |  Client   |          |
// |           <-----<----+
// +-----------+
//                <--- downstream <---
//
#pragma once

// c++ standard
#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <memory>
#include <map>
// boost
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
// sqlite
//#include <SQLiteCpp/SQLiteCpp.h>
// json
#include <json.h>
// cci
#include <cci_trinity_db.h>
#include <cci_time_utils.h>
#include <cci_trinity_options.h>
#include <rb.h>

namespace cci_trinity
{
                       //scope
            namespace ip = boost::asio::ip;


            //forward declarations
            class trinity_bridge;
            class trinity_acceptor;


            //enumerations


            //immutabble
            enum { max_data_length = 8192 };

            //aliases
            using trinity_meta_dictionary = std::map<std::string,std::string>;
            using switch_arg = tclap::SwitchArg;
            using value_arg = tclap::ValueArg<std::string>;
            using cmd_line = tclap::CmdLine;




            //servces
            //trinity_bridge
            class trinity_bridge : public boost::enable_shared_from_this<trinity_bridge>
            {

                     public :

                            //aliases
                            using socket_type =  ip::tcp::socket;
                            using ptr_type =  boost::shared_ptr<trinity_bridge>;

                            //ctor
                            explicit trinity_bridge( boost::asio::io_service& ios );

                            //dtor
                            virtual ~trinity_bridge() = default;


                            //no copy
                            trinity_bridge( const trinity_bridge& br ) = delete;
                            //no assign
                            const trinity_bridge& operator= ( const trinity_bridge& tb ) = delete;



                    private :

                             //attibutes
                             socket_type        m_downstream_socket_;
                             socket_type        m_upstream_socket_;
                             unsigned char      m_downstream_data_[max_data_length];
                             unsigned char      m_upstream_data_[max_data_length];
                             //composition
                             boost::mutex       m_mutex_;


                             //internal services
                             void handle_downstream_write( const boost::system::error_code& error );
                             void handle_downstream_read( const boost::system::error_code& error ,
                                  const size_t& bytes_transferred );
                             void handle_upstream_write( const boost::system::error_code& error );
                             void handle_upstream_read( const boost::system::error_code& error ,
                                const size_t& bytes_transferred );
                             void handle_close();

                    public :

                             //accesors-inspectors
                             socket_type& downstream_socket() noexcept { return m_downstream_socket_; }
                             socket_type& upstream_socket() noexcept { return m_upstream_socket_; }


                            //services
                            void start (const std::string& upstream_host , unsigned short upstream_port );
                            void handle_upstream_connect( const boost::system::error_code& error );


             };

             //trinity_acceptor
             class trinity_acceptor
             {

                    public :

                        //ctor
                        explicit trinity_acceptor( boost::asio::io_service& io_service ,
                                                   const std::string& local_host ,
                                                   unsigned short local_port ,
                                                   const std::string& upstream_host ,
                                                   unsigned short upstream_port );

                        //dtor
                        virtual ~trinity_acceptor() = default;


                        //no copy
                        trinity_acceptor( const trinity_acceptor& ta ) = delete;
                        //no assign
                        const trinity_acceptor& operator= ( const trinity_acceptor& ta ) = delete;



                    private :

                         //attributes
                         boost::asio::io_service&       m_io_service_;
                         ip::address_v4                 m_localhost_address;
                         ip::tcp::acceptor              m_acceptor_;
                         trinity_bridge::ptr_type       m_session_;
                         unsigned short                 m_upstream_port_;
                         std::string                    m_upstream_host_;


                         //services
                         void handle_accept( const boost::system::error_code& error );


                    public :

                         //services
                         bool accept_connections();


             };




}

