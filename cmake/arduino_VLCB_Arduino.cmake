if(NOT ARDUINO_USER_LIBS_PATH)
    message(FATAL_ERROR "Arduino-specific variables are not set. \
                         Did you select the right toolchain file?")
endif()

add_library(VLCB_Arduino STATIC
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/CAN2515.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/EventProducerService.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/CanService.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/EventTeachingService.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/FlashStorage.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/LED.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/Configuration.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/LEDUserInterface.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/ConsumeOwnEventsService.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/LongMessageService.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/Controller.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/MinimumNodeService.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/CreateDefaultStorageForPlatform.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/NodeVariableService.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/DueEepromEmulationStorage.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/Parameters.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/EepromExternalStorage.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/SerialUserInterface.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/EepromInternalStorage.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/Switch.cpp
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src/EventConsumerService.cpp
)
target_link_libraries(VLCB_Arduino PUBLIC ArduinoFlags)
target_link_libraries(VLCB_Arduino PUBLIC ArduinoCore)
target_link_libraries(VLCB_Arduino PUBLIC ArduinoLibs)
target_link_libraries(VLCB_Arduino PUBLIC ArduinoStreaming)
target_link_libraries(VLCB_Arduino PUBLIC ArduinoACAN2515)

target_include_directories(VLCB_Arduino PUBLIC
    ${ARDUINO_USER_LIBS_PATH}/VLCB-Arduino/src
)
