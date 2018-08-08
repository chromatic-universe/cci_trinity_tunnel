//cci_time_utils.h    william k. jonson 2016
//c++ std
#pragma once


#include <chrono>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <set>
//c runtime
#include <ctime>
#include <cstdint>
#include <cassert>
#include <sys/types.h>
#include <unistd.h>




namespace cpp_real_stream
{
        //forward declarations
        class time_utils;
        enum class stamp_color : char;

        //aliases
        using time_t_ptr = std::time_t* ;
        using time_utils_ptr = time_utils*;
        using color_map = std::map<stamp_color,std::string>;

        //enumerations
        enum  class stamp_color : char { red , blue , green , none };

        //immutable
        static char space( ' ' );
        static char colon ( ':' );
        static std::string _process( "/proc/self/comm" );
        //constant
        static color_map colors = { { stamp_color::red , "\033[22;31m"  } ,
                                  { stamp_color::blue , "\033[22;34m"  } ,
                                  { stamp_color::green , "\033[22;32m" } ,
                                  { stamp_color::none , "\033[0m"  } };

         //helpers
        static std::string moniker()
        {
            //non-portable
            std::ostringstream ostr;
            //proc name from /proc
            std::ifstream comm( _process.c_str() );
            std::string proc;
            //getline
            getline ( comm , proc );
            //caller;s pid
            ostr << proc
                 << "(" << getpid() << ")";

            return ostr.str();
        }
        ///classes
        //helpers
        class time_utils
        {
            //we make some platform assumptions here about time structures and calcs,
            //but acceptable begin this class is not for scientific computing
            friend std::ostream& operator<< ( std::ostream& ostr , time_utils& tu )
            {
                    ostr << tu.ret_stamp();
                    return ostr;//<< tu.time_stamp();
            }


            public :

                               //ctor
                explicit time_utils( stamp_color sc = stamp_color::none ) : m_time( new std::time_t ) ,
                                                                            m_color { sc }

                {
                    assert( m_time.get() );
                    m_moniker = moniker();
                }
                explicit time_utils( std::unique_ptr<std::time_t> time_ptr ,
                                     stamp_color sc = stamp_color::none ) :
                                                                  m_time( std::move( time_ptr ) ) ,
                                                                  m_color { sc }
                {
                    assert( m_time.get() );
                    m_moniker = moniker();
                }

                //copy
                time_utils( time_utils&& tu ) = default;
                //assign
                time_utils& operator= ( time_utils&& tu ) = default;
                //compare
                bool operator== ( const time_utils& tu )
                {
                    //resolution is seconds as double
                    return std::difftime( *m_time.get() , *(tu.m_time.get() ) ) == 0;
                }
                bool operator< ( const time_utils& tu )
                {
                    //resolution is seconds as double
                    return *(tu.m_time.get() ) < *m_time.get();
                }

                //dtor
                //we only clear the default strean cerr
                virtual ~time_utils() { clear_color(); };


            protected :

                //attributes } }
                std::unique_ptr<std::time_t> m_time;
                stamp_color m_color;

            private :

                //attributes
                std::string m_moniker;

            public :

                //inspectors-accessors
                const time_t_ptr raw_t() const noexcept { return m_time.get(); }
                stamp_color color() const noexcept { return m_color; }

                //mutators
                void raw_t( std::unique_ptr<std::time_t>& t ) {  m_time = std::move( t ); }
                void color( const stamp_color sc ) { m_color = sc; }

                //services
                virtual std::string present()
                {
                    //std::time_t result = time( nullptr );time c function thread safe
                    //std::string sz = std::ctime( &result );
                    //sz.resize( sz.size() - 1 );

                    time_t rawtime;
                    struct tm * timeinfo;

                    time ( &rawtime );
                    timeinfo = localtime ( &rawtime );
                    std::string sz { asctime( timeinfo ) };
                    sz.resize( sz.length() - 1 );

                    return sz;

                }


                void time_stamp( std::ostream& ostr = std::cerr )
                {
                    //non-portable
                    ostr << colors[m_color]
                         << present()
                         << space
                         << m_moniker
                         << space
                         << colon
                         << space;
                }

                void stamp( const std::string& st , std::ostream& ostr = std::cerr )
                {
                    //non-portable
                    ostr << colors[m_color]
                         << st;
                }


                std::string ret_stamp()
                {
                    std::ostringstream ostr;
                    //non-portable
                    ostr << present()
                         << space
                         << m_moniker
                         << space
                         << colon
                         << space
                         << std::ends;

                    return ostr.str();
                }

		void null_stamp( std::ostream& ostr = std::cerr )
                {
                    //non-portable
                    ostr << colors[m_color]
                         << "";
                }



                void time_stamp( std::string& str )
                {
                    std::ostringstream ostr;//events

                    time_stamp( ostr );

                    str = ostr.str();
                }

                void clear_color( std::ostream& ostr = std::cerr ) noexcept
                {
                    ostr << colors[stamp_color::none];
                }

        };

}
