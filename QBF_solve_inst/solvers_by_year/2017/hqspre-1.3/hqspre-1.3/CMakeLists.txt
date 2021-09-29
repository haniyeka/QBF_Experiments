project(HQSpre C CXX)

cmake_minimum_required(VERSION 3.0)

# Enforce C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANARD_REQUIRED ON)
#set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_BUILD_TYPE "Debug")

# Enforce static binaries and enable warnings
set(CMAKE_EXE_LINKER_FLAGS "-std=c++14 -Wall -Wextra -g ${CMAKE_EXE_LINKER_FLAGS}" )
set(CMAKE_C_FLAGS          "-Wall -Wextra -g ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS        "-std=c++14 -Wall -Wextra -Wcast-align -Wnon-virtual-dtor -Woverloaded-virtual -g ${CMAKE_CXX_FLAGS}")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_EXE_LINKER_FLAGS "-static ${CMAKE_EXE_LINKER_FLAGS}" )
  message("-- Enabled static linking.")
endif()

# Find Boost libraries
set(Boost_USE_STATIC_LIBS    OFF)
set(Boost_USE_MULTITHREADED  OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.40 REQUIRED COMPONENTS iostreams program_options)


# Find libz (used by boost-iostreams)
find_package(ZLIB REQUIRED)


# Enable OpenMP (used by the Antom SAT solver)
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


# Check if Doxygen is available; if yes, add the build target 'doc'
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc EXCLUDE_FROM_ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
     COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)


add_definitions("-DELPP_NO_DEFAULT_LOG_FILE")

# Check if valgrind is installed. This is used by the macro
# val_assert(...) to print a stack trace when the condition
# is violated and run through valgrind.
include(CheckIncludeFiles)
check_include_files("valgrind/valgrind.h" HAVE_VALGRIND)
if(HAVE_VALGRIND)
  add_definitions("-DHAVE_VALGRIND")
endif()


# Enable the selection of different SAT solvers
option(SAT_SOLVER:String "Select the SAT solver to be used (Antom|Picosat)" "Antom")
if (SAT_SOLVER STREQUAL "Antom")
    add_definitions("-DSatSolver=Antom")
    message("-- Using Antom as a SAT solver")
elseif(SAT_SOLVER STREQUAL "Picosat")
    add_definitions("-DSatSolver=Picosat")
    message("-- Using Picosat as a SAT solver")
endif()

# Enable the computation of Skolem functions
option(SKOLEM:Bool "Enable the computation of Skolem functions" OFF)
if (SKOLEM)
    add_definitions("-DSKOLEM")
    message("-- Enabled Skolem function computation")
else()
    message("-- Disabled Skolem function computation")
endif()

option(NO_LOGGING:Bool "Disable logging" OFF)
if (NO_LOGGING)
    add_definitions("-DELPP_DISABLE_LOGS -Wno-unused-parameter")
    message("-- Logging disabled")
endif()


# Set the include paths
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/antom/solver")
include_directories("${PROJECT_SOURCE_DIR}/libs/picosat")
include_directories(${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})

add_subdirectory("libs/antom")
add_subdirectory("libs/picosat")
add_subdirectory("libs/easylogging")
add_subdirectory("src")
