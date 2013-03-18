cmake_minimum_required(VERSION 2.8.3)
project(demo)

function(compile_protobuf_c prefix)
  if(NOT ARGN)
    message(SEND_ERROR "Error: compile_protobuf_c called without any proto files")
    return()
  endif(NOT ARGN)

  set(PROTO_HEADERS)
  set(${prefix})
  foreach(file ${ARGN})
    get_filename_component(file_abs ${file} ABSOLUTE)
    get_filename_component(file_dir ${file_abs} PATH)
    get_filename_component(file_we ${file} NAME_WE)
    list(APPEND ${prefix} "${CMAKE_CURRENT_BINARY_DIR}/${file_we}.pb-c.c")
    list(APPEND PROTO_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/${file_ws}.pb-c.h")

    add_custom_command(
      OUTPUT ${${prefix}} ${PROTO_HEADERS}
      COMMAND protoc-c
      ARGS --c_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=${file_dir} ${file_abs}
      DEPENDS ${file_abs}
      COMMENT "Running protocol buffer C compiler on ${file}"
      VERBATIM
    )
  endforeach()

  set(${prefix} ${${prefix}} PARENT_SCOPE)

endfunction()

compile_protobuf_c(TIME_PROTOBUFC msgs/time.proto)
compile_protobuf_c(STRING_STAMPED_PROTOBUFC msgs/string_stamped.proto)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(talker src/talker.c ${TIME_PROTOBUFC} ${STRING_STAMPED_PROTOBUFC})
target_link_libraries(talker protobuf-c zmq)

add_executable(listener src/listener.c ${TIME_PROTOBUFC} ${STRING_STAMPED_PROTOBUFC})
target_link_libraries(listener protobuf-c zmq)
