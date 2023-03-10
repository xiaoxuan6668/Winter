cmake_minimum_required(VERSION 3.22)
project(winter)

# makefile has bug
include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
# -Wno-builtin-macro-redefined

include_directories(.)
# include_directories(/usr/local/include)
# link_directories(/usr/local/lib)

# find_library(YAMLCPP yaml-cpp)
# message("***", ${YAMLCPP})

set(LIB_SRC
    winter/address.cc
    winter/bytearray.cc
    winter/config.cc
    winter/daemon.cc
    winter/env.cc
    winter/fd_manager.cc
    winter/fiber.cc
    winter/http/http.cc
    winter/http/http_connection.cc
    winter/http/http_parser.cc
    winter/http/http_session.cc
    winter/http/http_server.cc
    winter/http/servlet.cc
    winter/hook.cc
    winter/iomanager.cc
    winter/log.cc
    winter/scheduler.cc
    winter/socket.cc
    winter/socket_stream.cc
    winter/stream.cc
    winter/tcp_server.cc
    winter/timer.cc
    winter/thread.cc
    winter/util.cc
    )

ragelmaker(winter/http/http11_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/winter/http)
ragelmaker(winter/http/httpclient_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/winter/http)
ragelmaker(winter/uri.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/winter)

add_library(winter SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(winter) #__FILE__
#add_library(winter_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES(winter_static PROPERTIES OUTPUT_NAME "winter")

set(LIBS
    winter
    pthread
    dl
    yaml-cpp)

# add_executable(test tests/test.cc)
# add_dependencies(test winter)
# force_redefine_file_macro_for_sources(test) #__FILE__
# target_link_libraries(test ${LIBS})

winter_add_executable(test1 "tests/test.cc" winter "${LIBS}")
winter_add_executable(test_config "tests/test_config.cc" winter "${LIBS}")
winter_add_executable(test_thread "tests/test_thread.cc" winter "${LIBS}")
winter_add_executable(test_util "tests/test_util.cc" winter "${LIBS}")
winter_add_executable(test_fiber "tests/test_fiber.cc" winter "${LIBS}")
winter_add_executable(test_scheduler "tests/test_scheduler.cc" winter "${LIBS}")
winter_add_executable(test_iomanager "tests/test_iomanager.cc" winter "${LIBS}")
winter_add_executable(test_hook "tests/test_hook.cc" winter "${LIBS}")
winter_add_executable(test_address "tests/test_address.cc" winter "${LIBS}")
winter_add_executable(test_socket "tests/test_socket.cc" winter "${LIBS}")
winter_add_executable(test_bytearray "tests/test_bytearray.cc" winter "${LIBS}")
winter_add_executable(test_http "tests/test_http.cc" winter "${LIBS}")
winter_add_executable(test_http_parser "tests/test_http_parser.cc" winter "${LIBS}")
winter_add_executable(test_tcp_server "tests/test_tcp_server.cc" winter "${LIBS}")
winter_add_executable(echo_server "examples/echo_server.cc" winter "${LIBS}")
winter_add_executable(test_http_server "tests/test_http_server.cc" winter "${LIBS}")
winter_add_executable(test_http_connection "tests/test_http_connection.cc" winter "${LIBS}")
winter_add_executable(test_uri "tests/test_uri.cc" winter "${LIBS}")

winter_add_executable(test_daemon "tests/test_daemon.cc" winter "${LIBS}")

winter_add_executable(test_env "tests/test_env.cc" winter "${LIBS}")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)