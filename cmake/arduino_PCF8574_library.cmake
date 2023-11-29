if(NOT ARDUINO_PATH)
    message(FATAL_ERROR "Arduino-specific variables are not set. \
                         Did you select the right toolchain file?")
endif()

add_library(ArduinoPCF8574_library STATIC
    ${ARDUINO_USER_LIBS_PATH}/PCF8574_library/PCF8574.cpp
)
target_link_libraries(ArduinoPCF8574_library PUBLIC ArduinoFlags)
target_link_libraries(ArduinoPCF8574_library PUBLIC ArduinoCore)
target_link_libraries(ArduinoPCF8574_library PUBLIC ArduinoLibs)

target_include_directories(ArduinoPCF8574_library PUBLIC
    ${ARDUINO_USER_LIBS_PATH}/PCF8574_library
)
