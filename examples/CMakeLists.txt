cmake_minimum_required(VERSION 3.16)

project(examples LANGUAGES C)

add_executable(example-alaw-enc "example-alaw-enc.c")
target_link_libraries(example-alaw-enc PUBLIC xcodec)

add_executable(example-alaw-dec "example-alaw-dec.c")
target_link_libraries(example-alaw-dec PUBLIC xcodec)

install(TARGETS example-alaw-enc DESTINATION bin)
install(TARGETS example-alaw-dec DESTINATION bin)

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/certs DESTINATION bin FILES_MATCHING PATTERN "*")
