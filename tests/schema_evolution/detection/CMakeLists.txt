set(should_fail_cases
  vector_members:add_new
)

set(should_succeed_cases
  members:float_to_double
)

set(should_warn_cases
  members:rename
)

foreach(test_case IN LISTS should_fail_cases should_succeed_cases should_warn_cases)
  add_test(NAME schema_evol:detection:${test_case} COMMAND ${CMAKE_CURRENT_LIST_DIR}/run_test_case.sh ${test_case})
  PODIO_SET_TEST_ENV(schema_evol:detection:${test_case})
endforeach()

foreach(test_case IN LISTS should_fail_cases)
  set_property(TEST schema_evol:detection:${test_case} PROPERTY WILL_FAIL true)
endforeach()

foreach(test_case IN LISTS should_warn_cases)
  set_property(TEST schema_evol:detection:${test_case} PROPERTY PASS_REGULAR_EXPRESSION "Warnings:")
endforeach()
