
# version
cmake_minimum_required(VERSION 2.8)
# project
project(cci_trinity_tunnel)


#cpp flags
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g  -O0 -Qunused-arguments -DDEBUG -std=c++1z")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -Qunused-arguments -DRELEASE -std=c++1z")



# includes
include_directories( ${cci_trinity_tunnel_SOURCE_DIR}
                    /usr/include
		    $ENV{DEV}/include 
                    $ENV{SOURCE_ROOT}/cci_develop/cci-cpp-stream/include
                    $ENV{SOURCE_ROOT}/cci_trinity_tunnel/include
                   ) 

# link
link_directories( ${CMAKE_SOURCE_DIR}
                  /usr/lib
                   $ENV{DEV}/lib
                  $ENV{SOURCE_ROOT}/cci_trinity_tunnel/lib
                 /usr/lib/x86_64-linux-gnu
                )
 
 #binary
add_executable(cci-trinity-tunnel  cci_trinity_tunnel.cpp) 
                                  
#link libs
target_link_libraries(cci-trinity-tunnel -lstdc++
				         -lpthread 
                                         -lboost_thread
                                         -lboost_system 
					 cci_trinity_tunnel_core)


#install
install_targets(/bin  cci-trinity-tunnel)
install(DIRECTORY $ENV{SOURCE_ROOT}/cci_trinity_tunnel/py DESTINATION . )
install(FILES $ENV{SOURCE_ROOT}/cci_trinity_tunnel//instance_id DESTINATION bin)



