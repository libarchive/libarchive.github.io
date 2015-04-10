project(demo C)
cmake_minimum_required(VERSION 2.8)

find_path(LZMA_INCLUDE_DIR lzma.h)
if (NOT LZMA_INCLUDE_DIR MATCHES "-NOTFOUND")
   include_directories(${LZMA_INCLUDE_DIR})
endif ()

find_path(ICONV_INCLUDE_DIR iconv.h)
include_directories(${ICONV_INCLUDE_DIR})

find_library(LIBICONV_PATH iconv)
list(APPEND ADDITIONAL_LIBS ${LIBICONV_PATH})

add_executable(demo demo.c)
target_link_libraries(demo ${ADDITIONAL_LIBS})
