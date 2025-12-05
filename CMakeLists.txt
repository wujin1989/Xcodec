cmake_minimum_required(VERSION 3.16)

project(Xcomm LANGUAGES C)

option(ENABLE_TESTING "Enable Testing" ON)
option(BUILD_DYNAMIC_LIBRARY "Build Dynamic Library" OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS "Export Compile-Commands" OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)

if(WIN32)
	if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
		add_compile_options("/experimental:c11atomics")
	endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SRCS
	src/alaw/xcodec-alaw-encoder.c
	src/alaw/xcodec-alaw-decoder.c
	src/alaw/xcodec-alaw-module.c
)

if(WIN32)
	list(APPEND SRCS 

	)
endif()

if(UNIX)
	list(APPEND SRCS 

	)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
	set(CMAKE_SYSTEM_PROCESSOR "AMD64")
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
	set(CMAKE_SYSTEM_PROCESSOR "aarch64")
endif()

add_library(ssl IMPORTED STATIC)
add_library(crypto IMPORTED STATIC)

if(WIN32)
	set_target_properties(ssl PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3/lib/${CMAKE_SYSTEM_PROCESSOR}/win/ssl.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3//include")

	set_target_properties(crypto PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3/lib/${CMAKE_SYSTEM_PROCESSOR}/win/crypto.lib"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3//include" 
		INTERFACE_LINK_LIBRARIES "Crypt32.lib")
endif()

if(UNIX)
	set_target_properties(ssl PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3/lib/${CMAKE_SYSTEM_PROCESSOR}/unix/libssl.a"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3//include")

	set_target_properties(crypto PROPERTIES
		IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3/lib/${CMAKE_SYSTEM_PROCESSOR}/unix/libcrypto.a"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third-party/openssl3//include")
endif()

if(BUILD_DYNAMIC_LIBRARY)
    add_library(xcodec SHARED ${SRCS})
	target_link_libraries(xcodec PRIVATE ssl crypto ${CMAKE_DL_LIBS})
	set_target_properties(xcodec PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)
else()
    add_library(xcodec STATIC ${SRCS})
	target_link_libraries(xcodec PRIVATE ssl crypto ${CMAKE_DL_LIBS})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION . FILES_MATCHING PATTERN "*")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs DESTINATION . FILES_MATCHING PATTERN "*")
install(TARGETS xcodec DESTINATION lib)

add_subdirectory(examples)

#if(ENABLE_TESTING)
#	enable_testing()
#	add_subdirectory(tests)
#endif()

