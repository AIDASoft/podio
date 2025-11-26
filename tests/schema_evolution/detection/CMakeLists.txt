set(should_fail_cases
  vector_members:add_new
  vector_members:add_additional
  vector_members:rm_one
  relations:new_single_relation
  relations:rm_single_relation
  relations:new_multi_relation
  relations:rm_multi_relation
  relations:mv_single_to_multi
  relations:mv_multi_to_single
  members:change_component_type
)

set(should_succeed_cases
  members:float_to_double
  members:add_one_rm_one
)

set(should_warn_cases
  members:rename
)

foreach(test_case IN LISTS should_fail_cases should_succeed_cases should_warn_cases)
  add_test(NAME schema_evol:detection:${test_case} COMMAND ${CMAKE_CURRENT_LIST_DIR}/run_test_case.sh ${test_case})
  PODIO_SET_TEST_ENV(schema_evol:detection:${test_case})
endforeach()

foreach(test_case IN LISTS should_succeed_cases)
  set_property(TEST schema_evol:detection:${test_case} PROPERTY FAIL_REGULAR_EXPRESSION "Warnings:")
endforeach()

foreach(test_case IN LISTS should_fail_cases)
  set_property(TEST schema_evol:detection:${test_case} PROPERTY WILL_FAIL true)
endforeach()

foreach(test_case IN LISTS should_warn_cases)
  set_property(TEST schema_evol:detection:${test_case} PROPERTY PASS_REGULAR_EXPRESSION "Warnings:")
endforeach()
