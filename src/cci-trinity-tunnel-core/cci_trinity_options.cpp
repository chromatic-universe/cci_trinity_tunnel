// cci_trinity_options.cpp   chromatic universe 2016-2020     william k. johnson

#include <cci_trinity_options.h>
#include <cci_time_utils.h>

using namespace cci_trinity;
using namespace tclap;
using namespace cpp_real_stream;


//----------------------------------------------------------------------------------------
void cci_cli_output::failure( tclap::CmdLineInterface& c , tclap::ArgException& e )
{
        m_tmu->color(  stamp_color::red );
        m_tmu->time_stamp();
        std::cerr << "failure in parse args : "
                  << "\n"
                  << e.what()
                  << "\n";
        m_tmu->clear_color();

        usage( c  );

        exit( 1 );
}

//----------------------------------------------------------------------------------------
void cci_cli_output::usage( tclap::CmdLineInterface& c )
{
        std::cerr << colors[stamp_color::none]
                  << "\nusage for cci-trinity-tunnel:\n"
                  << m_ptr_ostr->str()
                  << "\n\n";
        std::list<tclap::Arg*> args = c.getArgList();
		for( auto elem : args )
        {
              std::cerr  << "\t"
                         << elem->longID()
                         << "\n\t\t"
                         << "(";
             if( elem->isRequired() ) { std::cerr << colors[stamp_color::red];  }
             std::cerr << elem->getDescription()
                       << colors[stamp_color::none]
                       << ")"
                       << "\n";
        }
       m_tmu->clear_color();
}

//----------------------------------------------------------------------------------------
void cci_cli_output::version( tclap::CmdLineInterface& c )
{
    std::cerr << "cci-trinity-tunnel: version 0.94 william k. johnson 2017\n";
    m_tmu->clear_color();

}


