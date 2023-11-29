if(NOT ARDUINO_PATH)
    message(FATAL_ERROR "Arduino-specific variables are not set. \
                         Did you select the right toolchain file?")
endif()

add_library(ArduinoAVR_PWM INTERFACE)

target_link_libraries(ArduinoAVR_PWM INTERFACE ArduinoFlags)
target_link_libraries(ArduinoAVR_PWM INTERFACE ArduinoCore)
target_link_libraries(ArduinoAVR_PWM INTERFACE ArduinoLibs)

target_include_directories(ArduinoAVR_PWM INTERFACE
    ${ARDUINO_USER_LIBS_PATH}/AVR_PWM/src
)
