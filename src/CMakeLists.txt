file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${VLCB_Arduino_SOURCE_DIR}/src/*.h")

add_library(controller_library OBJECT
  DcPwmTrainController.cpp
  DcCommandStationService.cpp
)
target_include_directories(controller_library PUBLIC .)

target_link_libraries(controller_library PUBLIC ArduinoFlags)
target_link_libraries(controller_library PUBLIC ArduinoLibs)
target_link_libraries(controller_library PUBLIC ArduinoStreaming)
target_link_libraries(controller_library PUBLIC VLCB_Arduino)
target_link_libraries(controller_library PUBLIC ArduinoAVR_PWM)

source_group(
  TREE ${PROJECT_SOURCE_DIR}
  PREFIX "Header Files"
  FILES ${HEADER_LIST}
)
