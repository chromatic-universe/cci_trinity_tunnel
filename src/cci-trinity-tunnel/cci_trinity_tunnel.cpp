// cci_trinity_tunnel.cpp   william k. johnson 2016
// added support for c++11/14 .
//                   command line handling ,
//                   prcess signaling ,
//                   interpocess communication
//                   application binding,
//                   sqlite db
// Copyright (c) 2016 chromatic universe

//
// tcpproxy_server.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2007 Arash Partow (http://www.partow.net)
// URL: http://www.partow.net/programming/tcpproxy/index.html
//
// Distributed under the Boost Software License, Version 1.0.
#include <signal.h>
//#include <cci_trinity_options.h>
#include <cci_trinity_tunnel.h>

const std::string instance_filename( "instance_id" );

using namespace cci_trinity;
using json = nlohmann::json;
using namespace cpp_real_stream;

//-----------------------------------------------------------------------------------------------------
//use python subprocess to retrieve mongo configuration
//mongo c/c++ drivers  introduce too much overhead and the builds
//pkg-config are shaky on ubuntu

namespace
{

            std::vector<std::string>
            split(const std::string& str, const std::string& delims=" \t")
            {
                std::vector<std::string> res;
                size_t init = 0;

                while (true) {
                  auto pos = str.find_first_of(delims, init);
                  if (pos == std::string::npos) {
                res.emplace_back(str.substr(init, str.length()));
                break;
                  }
                  res.emplace_back(str.substr(init, pos - init));
                  pos++;
                  init = pos;
                }

                return res;
            }


            //signals
            void sigterm( int sig )
            {
                std::unique_ptr<time_utils> tu ( new time_utils( stamp_color::green ) );
                tu->null_stamp();
                std::cerr << "[trinity] ";
                tu->time_stamp();
                std::cerr << "..caught SIGTERM...\n";
                std::cerr << "[trinity] ";
                tu->time_stamp();
                std::cerr << "..server shutdown...fini..";

                std::cerr << "\033[0m\n" ;


                exit( 0 );
            }

            //command line
            auto ccmd( make_unique<cmd_line> ( "cci-trinity-tunnel 2017" , ' ' , "0.94" ) );
            //quiet output
            auto quiet_switch( std::make_unique<cci_trinity::switch_arg> (  "q" ,
                                                                             "quiet" ,
                                                                             "non-verbose output" ,
                                                                             *ccmd.get() ,
                                                                             false
                                                                          )
                            );
            //no retrieve metadata
            auto no_meta_switch( std::make_unique<cci_trinity::switch_arg> (  "n" ,
                                                                             "no-metadata" ,
                                                                             "do not retrieve metadata" ,
                                                                             *ccmd.get() ,
                                                                             false
                                                                            )
                            );
            //atomic
            auto atomic_switch( std::make_unique<cci_trinity::switch_arg> (  "a" ,
                                                                             "atomic" ,
                                                                             "standalone implementation" ,
                                                                             false
                                                                          )
                              );
            //atomic
            auto drone_switch( std::make_unique<cci_trinity::switch_arg> (  "r" ,
                                                                             "drone" ,
                                                                             "controlled drone" ,
                                                                             false
                                                                          )
                              );
            //atomic
            auto daemon_switch( std::make_unique<cci_trinity::switch_arg> (  "d" ,
                                                                            "daemon" ,
                                                                             "run as unsupervised daaemon" ,
                                                                              *ccmd.get() ,
                                                                             false
                                                                          )
                              );
            std::vector<std::string> allowed = { "http" , "tcp" , "icmp"  };
            tclap::ValuesConstraint<std::string> allowed_vals( allowed );
            //tunnel tuple
            auto tunnel_val( std::make_unique<cci_trinity::value_arg> ( "t" ,
                                                                          "tunnel" ,
                                                                          "tunnel type" ,
                                                                           true ,
                                                                           "http" ,
                                                                            &allowed_vals
                                                                       )

                            );
            //proxy ip tuple
            auto proxy_tuple( std::make_unique<cci_trinity::value_arg> ( "p" ,
                                                                          "proxy" ,
                                                                          "proxy ip tuple" ,
                                                                           true ,
                                                                           "" ,
                                                                          "proxy ip:port" ,
                                                                          *ccmd.get()
                                                                        )
                            );
            //beard ip tuple
            auto destination_tuple( std::make_unique<cci_trinity::value_arg> ( "b" ,
                                                                               "desination" ,
                                                                               "destination ip tuple" ,
                                                                               true ,
                                                                               "foo" ,
                                                                               "destination ip:port" ,
                                                                               *ccmd.get()
                                                                             )
                                  );


            // -------------------------------------------------------------------------------------------------------
            void stream_metadata( json js , std::ostream& ostr = std::cerr );
            // -------------------------------------------------------------------------------------------------------
            trinity_meta_dictionary retr_mongo_metadata( bool stream_out = false , std::ostream& fstr = std::cerr );
            // -----------------------)-------------------------------------------------------------------------------
            void non_cli_boiler();
            //--------------------------------------------------------------------------------------------------------
            void dispatch_tunnel( const std::string& tunnel_type  ,
                                  const std::string& host_tuple ,
                                  const std::string& dest_tuple );
            //--------------------------------------------------------------------------------------------------------
            void dispatch_direct_tcp( const std::string& host_tuple ,
                                      const std::string& dest_tuple );



            //------------------------------------------------------------------------------------------------------
            void stream_metadata( json js , std::ostream& ostr )
            {

                      std::unique_ptr<time_utils> tu ( new time_utils( stamp_color::blue ) );


                      for ( json::iterator it = js.begin(); it != js.end(); ++it )
                      {
                                          tu->color( stamp_color::blue );
                                          tu->null_stamp();
                                          ostr << "[metadata] ";
                                          tu->time_stamp();
                                          ostr << it.key();
                                          it.key().compare( "current_service_provider" )  == 0
                                              ?  tu->color( stamp_color::red )
                                              :  tu->color( stamp_color::none );
                                          tu->null_stamp();
                                          ostr << " : "
                                               << it.value()
                                               << "\n";
                      }
                      ostr << "\n\n"
                           << "copyright chromatic universe 2017\n\n";
                      tu->clear_color();

            }


            // -------------------------------------------------------------------------------------------------------
            trinity_meta_dictionary retr_mongo_metadata( bool stream_out  , std::ostream& osstr )
            {
                      trinity_meta_dictionary tmd;
                      std::unique_ptr<time_utils> tu ( new time_utils( stamp_color::blue ) );

                      std::string err_str( "...bootstrap exception...\n" );

                      try
                      {
                            //deserialize
                            json j;

                            //std::istringstream istr( check_output( {"python3", "../py/trinity_cpp.py"} ).buf.data() );
                            //err_str = istr.str();
                            //istr >> j;

                    if ( stream_out )  { stream_metadata( j );  }



              }
              catch( std::exception& e )
              {
                  tu->color( stamp_color::red );
                  tu->null_stamp();
                  std::cerr << "[exception] ";
                  tu->time_stamp();
                  std::cerr << err_str
                            << "[exception] ";
                  tu->time_stamp();
                  std::cerr << "...serialization from mongo server failed..."
                            << "\n";
                  tu->clear_color();
              }

              return tmd;

    }

    // -------------------------------------------------------------------------------------------------------
    void non_cli_boiler()
    {

                  std::unique_ptr<time_utils> tu ( new time_utils( stamp_color::green ) );
                  tu->null_stamp();
                  std::cerr << "[trinity] ";
                  tu->time_stamp();
                  std::cerr << "cci-trinity-tunnel( [http.icmp,tcp] )\n[trinity] version 0.9 ";
                  tu->time_stamp();
                  std::cerr << "http/icmp tunnel & smart proxy chromatic universe 2017\n";
                  std::cerr << "[trinity] ";
                  tu->time_stamp();
                  std::cerr << "process: ";
                  std::cerr  << moniker();
                  tu->color( stamp_color::blue );
                  std::cerr << "\n";
                  tu->clear_color();

    }

    //--------------------------------------------------------------------------------------------------------
    void dispatch_tunnel( const std::string& tunnel_type ,
                          const std::string& host_tuple ,
                          const std::string& dest_tuple  )
    {
                  if( tunnel_type.compare( "tcp") == 0 )
                  {
                        dispatch_direct_tcp( host_tuple , dest_tuple );
                  }

    }


    // -------------------------------------------------------------------------------------------------------
    void dispatch_direct_tcp( const std::string& host_tuple ,
                              const std::string& dest_tuple )
    {

               boost::asio::io_service ios;
               std::vector<std::string> v_host;
               std::vector<std::string> v_dest;

               const std::string colon { ":" };
               std::unique_ptr<time_utils> tu ( new time_utils( stamp_color::blue ) );

               try
               {
                      v_host = split( host_tuple , colon );
                      v_dest = split( dest_tuple , colon );

                      cci_trinity::trinity_acceptor acceptor( ios ,
                                                              v_host[0],
                                                              stoi( v_host[1] ) ,
                                                              v_dest[0] ,
                                                              stoi( v_dest[1] ) ) ;
                      tu->null_stamp();
                      std::cerr << "[trinity-stream] ";
                      tu->time_stamp();

                      std::cerr << "..starting tcp proxy for "
                                << dest_tuple
                                << "....\n";
                      tu->clear_color();

                      acceptor.accept_connections();

                      ios.run();
               }
               catch( std::exception& e )
               {
                  tu->color( stamp_color::red );
                  tu->null_stamp();
                  std::cerr << "[exception] ";
                  tu->time_stamp();
                  std::cerr << e.what()
                            << "\n";
                  tu->clear_color();
                  exit( 1  );
               }

    }

    }



    //------------------------------------------------------------------------------------------------------
    int main( int argc , char* argv[] )
    {

              signal( SIGINT  , sigterm );
              signal( SIGTERM , sigterm );

              non_cli_boiler();


              try
              {
                    // mutually exclusive switches
                    std::vector<tclap::Arg*>  xorlist;
                    xorlist.push_back( atomic_switch.get() );
                    xorlist.push_back( drone_switch.get() );
                    // ad xor , cmd object also contains value arguments from static constuction
                    ccmd->xorAdd( xorlist );
                    ccmd->add( tunnel_val.get() );

                    // output
                    std::ostringstream ostr;
                    ostr << "copyright chromatic universe 2017";

                    auto cli( std::make_unique<cci_cli_output>( &ostr ) );
                    ccmd->setOutput( cli.get() );
                    // parse args array
                    ccmd->parse( argc, argv );


              }
              catch ( tclap::ArgException &e )
              {
                    std::cerr << "error: "
                              << e.error()
                              << " for arg "
                              << e.argId()
                              << "\n";

              }


              dispatch_tunnel( tunnel_val->getValue() ,
                               proxy_tuple->getValue() ,
                               destination_tuple->getValue() );


               return 0;
    }











