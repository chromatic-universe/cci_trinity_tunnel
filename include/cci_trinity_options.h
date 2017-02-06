// cci_trinity_options.h    william k. johnson copyrihgt 2016 chromatic universe
#pragma once

#include <iostream>
#include <sstream>
#include <memory>

// command line templates
#include <memory>


#include <tclap/CmdLine.h>

// cci
#include <cci_trinity_db.h>
#include <cci_time_utils.h>
#include <rb.h>

namespace tclap = TCLAP;


namespace cci_trinity
{


            using switch_arg = tclap::SwitchArg;
            using value_arg = tclap::ValueArg<std::string>;
            using cmd_line = tclap::CmdLine;

            class cci_cli_output : public tclap::StdOutput
            {
                protected :

                    std::ostringstream* m_ptr_ostr;
                    std::unique_ptr<cpp_real_stream::time_utils> m_tmu;

                public :

                    //ctor
                    cci_cli_output( std::ostringstream* ostr_ptr ) : m_ptr_ostr { ostr_ptr } ,
                                m_tmu( make_unique<cpp_real_stream::time_utils>( cpp_real_stream::stamp_color::blue ) )
                    {}
                    //dtor
                    ~cci_cli_output() = default;

                    virtual void failure( tclap::CmdLineInterface& c , tclap::ArgException& e );
                    virtual void usage( tclap::CmdLineInterface& c );
                    virtual void version( tclap::CmdLineInterface& );


             };




}
