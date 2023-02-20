cmake_minimum_required(VERSION 3.25)
project(rmat CXX)

find_package(OpenMP)

option(RMAT_STATIC_EXE OFF)

include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY     https://github.com/fmtlib/fmt.git
  GIT_TAG            master)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(docopt
  GIT_REPOSITORY     https://github.com/docopt/docopt.cpp.git
  GIT_TAG            master)
FetchContent_MakeAvailable(docopt)

add_executable(rmat rmat.cpp)
target_compile_features(rmat PRIVATE cxx_std_23)
target_link_libraries(rmat PRIVATE docopt fmt::fmt)

if (OpenMP_FOUND)
  target_link_libraries(rmat PRIVATE OpenMP::OpenMP_CXX)
endif (OpenMP_FOUND)

if (RMAT_STATIC_EXE)
  target_link_libraries(rmat -static)
endif (RMAT_STATIC_EXE)
