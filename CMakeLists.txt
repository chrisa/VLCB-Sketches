cmake_minimum_required(VERSION 3.22.1)
project(VLCB_Sketches LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 11)

include(cmake/lto.cmake)
include(cmake/arduino_flags.cmake)

add_subdirectory(src)

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "avr")
  set(ARDUINO_PORT "/dev/ttyACM0" CACHE STRING
    "The serial port for uploading using avrdude")

  include(cmake/arduino_sdk.cmake)

  include(VLCB_christmas/VLCB_christmas.cmake)
  include(VLCB_i2c/VLCB_i2c.cmake)
  include(VLCB_Command/VLCB_Command.cmake)
else()
  add_subdirectory(Arduino)
  add_subdirectory(test)

  #enable_testing()
  #add_test(NAME testAll COMMAND $<TARGET_FILE:testAll>)
endif()
