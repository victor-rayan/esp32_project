# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/mateus/esp/esp-idf/components/bootloader/subproject"
  "/home/mateus/Documentos/FSE/trabalho-final-2023-1-victorpaulomateus/smartwatch/build/bootloader"
  "/home/mateus/Documentos/FSE/trabalho-final-2023-1-victorpaulomateus/smartwatch/build/bootloader-prefix"
  "/home/mateus/Documentos/FSE/trabalho-final-2023-1-victorpaulomateus/smartwatch/build/bootloader-prefix/tmp"
  "/home/mateus/Documentos/FSE/trabalho-final-2023-1-victorpaulomateus/smartwatch/build/bootloader-prefix/src/bootloader-stamp"
  "/home/mateus/Documentos/FSE/trabalho-final-2023-1-victorpaulomateus/smartwatch/build/bootloader-prefix/src"
  "/home/mateus/Documentos/FSE/trabalho-final-2023-1-victorpaulomateus/smartwatch/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/mateus/Documentos/FSE/trabalho-final-2023-1-victorpaulomateus/smartwatch/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/mateus/Documentos/FSE/trabalho-final-2023-1-victorpaulomateus/smartwatch/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
