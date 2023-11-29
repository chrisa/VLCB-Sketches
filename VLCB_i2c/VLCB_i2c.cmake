set_source_files_properties(VLCB_i2c/VLCB_i2c.ino PROPERTIES LANGUAGE CXX)
add_executable(VLCB_i2c VLCB_i2c/VLCB_i2c.ino)

target_link_libraries(VLCB_i2c PUBLIC ArduinoFlags)
target_link_libraries(VLCB_i2c PUBLIC ArduinoCore)
target_link_libraries(VLCB_i2c PUBLIC ArduinoLibs)
target_link_libraries(VLCB_i2c PUBLIC VLCB_Arduino)
target_link_libraries(VLCB_i2c PUBLIC ArduinoPCF8574_library)
target_link_libraries(VLCB_i2c PUBLIC ArduinoStreaming)

arduino_avr_hex(VLCB_i2c)
arduino_avr_upload(VLCB_i2c ${ARDUINO_PORT})
