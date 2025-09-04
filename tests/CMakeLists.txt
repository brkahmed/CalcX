add_executable(test_eval test_eval.c)

target_link_libraries(test_eval PRIVATE eval)

add_test(NAME TestEval COMMAND test_eval)
