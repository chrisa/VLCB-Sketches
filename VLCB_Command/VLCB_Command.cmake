set_source_files_properties(VLCB_Command/VLCB_Command.ino PROPERTIES LANGUAGE CXX)
add_executable(VLCB_Command VLCB_Command/VLCB_Command.ino)

target_link_libraries(VLCB_Command PUBLIC ArduinoFlags)
target_link_libraries(VLCB_Command PUBLIC ArduinoCore)
target_link_libraries(VLCB_Command PUBLIC ArduinoLibs)
target_link_libraries(VLCB_Command PUBLIC VLCB_Arduino)
target_link_libraries(VLCB_Command PUBLIC controller_library)
target_link_libraries(VLCB_Command PUBLIC ArduinoStreaming)

arduino_avr_hex(VLCB_Command)
arduino_avr_upload(VLCB_Command ${ARDUINO_PORT})
