# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/rodrigo/esp/esp-idf/components/bootloader/subproject"
  "/home/rodrigo/ic_fmon/fmon2/build/bootloader"
  "/home/rodrigo/ic_fmon/fmon2/build/bootloader-prefix"
  "/home/rodrigo/ic_fmon/fmon2/build/bootloader-prefix/tmp"
  "/home/rodrigo/ic_fmon/fmon2/build/bootloader-prefix/src/bootloader-stamp"
  "/home/rodrigo/ic_fmon/fmon2/build/bootloader-prefix/src"
  "/home/rodrigo/ic_fmon/fmon2/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/rodrigo/ic_fmon/fmon2/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/rodrigo/ic_fmon/fmon2/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
