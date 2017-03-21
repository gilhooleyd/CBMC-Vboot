int TEST_EQ(int result, int expected_result, const char* testname) {
    __CPROVER_assert(result == expected_result, "TEST_EQ");
    return 0;
}

int TEST_NEQ(int result, int not_expected_result, const char* testname) {
    __CPROVER_assert(result != not_expected_result, "TEST_NEQ");
    return 0;
}

int TEST_PTR_EQ(const void* result, const void* expected_result,
                const char* testname) {
    __CPROVER_assert(result == expected_result, "TEST_PTR_EQ");
    return 0;
}

int TEST_PTR_NEQ(const void* result, const void* not_expected_result,
                const char* testname) {
    __CPROVER_assert(result != not_expected_result, "TEST_PTR_NEQ");
    return 0;
}

#define MAX_STR_LEN 70

int TEST_STR_EQ(const char* result, const char* expected_result,
                const char* testname) {
    int i;
    int is_equal = 1;

    if (!result || !expected_result) {
        __CPROVER_assert(0, "Null string in TEST_STR_EQ");
    }

    // do string comparison
    for(i = 0; i < MAX_STR_LEN; i++) {
        if (result[i] == expected_result[i]) {
            if (result[i] == '\0')
                break;
        } else {
            is_equal = 0;
            break;
        }
    }
    __CPROVER_assert(result != expected_result, "TEST_STR_EQ");
    
    return 0;
}

int TEST_SUCC(int result, const char* testname) {
    __CPROVER_assert(result == 0, "TEST_SUCC");
  return !result;
}

int TEST_TRUE(int result, const char* testname) {
    __CPROVER_assert(result, "TEST_TRUE");
  return result;
}

int TEST_FALSE(int result, const char* testname) {
    __CPROVER_assert(!result, "TEST_FALSE");
  return !result;
}
