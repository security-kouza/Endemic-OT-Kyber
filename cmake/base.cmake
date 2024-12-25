if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

set(CMAKE_MACOSX_RPATH 0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR}/cmake)


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if(NOT DEFINED OPENSSL_ROOT_DIR)
		IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "(aarch64)|(arm64)")
			# M1 Apple
			set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
			message(STATUS "OPENSSL_ROOT_DIR set to default: ${OPENSSL_ROOT_DIR}")
		ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "(aarch64)|(arm64)")
			# Intel Apple
			set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
			message(STATUS "OPENSSL_ROOT_DIR set to default: ${OPENSSL_ROOT_DIR}")
		ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "(aarch64)|(arm64)" )

	endif()
endif()


## Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "${Blue}Build type: ${CMAKE_BUILD_TYPE}${ColourReset}")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEBUG ON)
    add_definitions(-DDEBUG)
elseif(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(CMAKE_BUILD_TYPE MATCHES Debug)


#Compilation flags
string(JOIN " " CMAKE_C_FLAGS
    ${CMAKE_C_FLAGS}
    -pthread
    -funroll-loops
    -Wno-unused-result
    -Wno-attributes # Suppress warnings about alignment of std::array<__m128i, size>
    -Wno-ignored-attributes # Suppress warnings about alignment of std::vector<__m128i>
    -msse4
)
message(STATUS "${Blue}Platform: ${CMAKE_SYSTEM_PROCESSOR}${ColourReset}")

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "(aarch64)|(arm64)")
    set(ARCH_ARM ON)
endif()

if(ARCH_ARM)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a+simd+crypto+crc")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -maes -mrdseed")
endif()

if(DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -O0")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")
endif()
string(STRIP ${CMAKE_C_FLAGS} CMAKE_C_FLAGS)
message(STATUS "  C Flags: ${CMAKE_C_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++17")
string(STRIP ${CMAKE_CXX_FLAGS} CMAKE_CXX_FLAGS)
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
