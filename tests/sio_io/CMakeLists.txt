set(sio_dependent_tests
  write_sio.cpp
  read_sio.cpp
  read_and_write_sio.cpp
  write_timed_sio.cpp
  read_timed_sio.cpp
  read_frame_sio.cpp
  write_frame_sio.cpp
  read_frame_legacy_sio.cpp
  read_and_write_frame_sio.cpp
  read_python_frame_sio.cpp
)
set(sio_libs podio::podioSioIO)
foreach( sourcefile ${sio_dependent_tests} )
  CREATE_PODIO_TEST(${sourcefile} "${sio_libs}")
endforeach()

# These need to be linked against TTree explicitly, since it is not done
# through another library and the TimedReader/Writer decorators are
# header-only wrappers
target_link_libraries(write_timed_sio PRIVATE ROOT::Tree)
target_link_libraries(read_timed_sio PRIVATE ROOT::Tree)

set_property(TEST read_sio PROPERTY DEPENDS write_sio)
set_property(TEST read_and_write_sio PROPERTY DEPENDS write_sio)
set_property(TEST read_timed_sio PROPERTY DEPENDS write_timed_sio)
set_property(TEST read_frame_legacy_sio PROPERTY DEPENDS write_sio)

set_tests_properties(
  read_frame_sio
  read_and_write_frame_sio

  PROPERTIES
    DEPENDS
    write_frame_sio
)

add_test(NAME check_benchmark_outputs_sio COMMAND check_benchmark_outputs write_benchmark_sio.root read_benchmark_sio.root)
set_property(TEST check_benchmark_outputs_sio PROPERTY DEPENDS read_timed_sio write_timed_sio)

#--- Write via python and the SIO backend and see if we can read it back in in
#--- c++
add_test(NAME write_python_frame_sio COMMAND python3 ${PROJECT_SOURCE_DIR}/tests/write_frame.py example_frame_with_py.sio sio_io.Writer)
PODIO_SET_TEST_ENV(write_python_frame_sio)
set_property(TEST read_python_frame_sio PROPERTY DEPENDS write_python_frame_sio)
