
# version
cmake_minimum_required(VERSION 2.8)
# project
project(cci_trinity_tunnel)


#cpp flags
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g  -O0 -Qunused-arguments -DDEBUG -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -Qunused-arguments -DRELEASE -std=c++14")



# includes
include_directories( ${cci_trinity_tunnel_SOURCE_DIR}
                    /usr/include
                    ${CMAKE_SOURCE_DIR}/include
                   ) 

# link
link_directories( ${CMAKE_SOURCE_DIR}
                  /usr/lib
		  $ENV{DEV}/lib
                  ${CMAKE_SOURCE_DIR}/lib
                 /usr/lib/x86_64-linux-gnu
                )
 
 #library
add_library(cci_trinity_tunnel_core  SHARED cci_trinity_acceptor.cpp
                                            cci_trinity_bridge.cpp 
                                            cci_trinity_options.cpp)
                                  
set_target_properties(cci_trinity_tunnel_core PROPERTIES
                                              VERSION 1.0.0
                                              SOVERSION 1)    
#install
install_targets(/lib  cci_trinity_tunnel_core)



