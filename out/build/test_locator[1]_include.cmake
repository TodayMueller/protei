if(EXISTS "/home/daniil/Work/protei/out/build/test_locator[1]_tests.cmake")
  include("/home/daniil/Work/protei/out/build/test_locator[1]_tests.cmake")
else()
  add_test(test_locator_NOT_BUILT test_locator_NOT_BUILT)
endif()