#include <CUnit/CUnit.h>
#include <CUnit/CUnit_intl.h>
#include <CUnit/CUError.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Assuming dataframe.h is correctly included with all necessary declarations
#include "dataframe.h"


void test_create_dataframe(){
    int num_rows = 3;
    int num_columns = 2;
    DataFrame *df = create_dataframe(num_rows, num_columns);

    //tests
    CU_ASSERT_PTR_NOT_NULL_FATAL(df);  
    CU_ASSERT_EQUAL(df->num_rows, num_rows);
    CU_ASSERT_EQUAL(df->num_columns, num_columns);

    //cleanup
    destroy_dataframe(df);
}


// Helper function to calculate estimated memory usage
size_t estimate_memory_usage(size_t num_rows, size_t num_columns) {
    size_t memory = sizeof(DataFrame);
    memory += num_columns * sizeof(Column);
    
    // Assuming half columns are INT and half are STRING for estimation
    size_t int_columns = num_columns / 2;
    size_t string_columns = num_columns - int_columns;
    
    memory += int_columns * num_rows * sizeof(int);
    memory += string_columns * num_rows * sizeof(char *);
    
    return memory;
}

// Test for creating a large dataframe
void test_big_dataframe(void) {
    size_t num_rows = 1000000;      // 1,000,000 rows
    size_t num_columns = 25;        // 25 columns
    DataFrame *df = create_dataframe(num_rows, num_columns);
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(df);  // Assert DataFrame is not NULL
    CU_ASSERT_EQUAL(df->num_rows, num_rows);
    CU_ASSERT_EQUAL(df->num_columns, num_columns);
    
    // Add columns
    for (int i = 0; i < num_columns; i++) {
        char column_name[MAX_COLUMN_NAME_LENGTH];
        if (i % 2 == 0) {
            snprintf(column_name, MAX_COLUMN_NAME_LENGTH, "Dimension%d", i);
            CU_ASSERT_EQUAL(add_column(df, i, column_name, DATA_TYPE_STRING), 0);
        } else {
            snprintf(column_name, MAX_COLUMN_NAME_LENGTH, "Value%d", i);
            CU_ASSERT_EQUAL(add_column(df, i, column_name, DATA_TYPE_INT), 0);
        }
    }
    
    // Optional: Set some values to ensure columns are functional
    // For brevity, setting only the first and last row for some columns
    int sample_int = 42;
    const char *sample_str = "TestString";
    
    CU_ASSERT_EQUAL(set_value(df, 0, 1, &sample_int), 0); // Assuming column 1 is INT
    CU_ASSERT_EQUAL(set_value(df, num_rows - 1, 0, sample_str), 0); // Assuming column 0 is STRING
    
    // Validate values (optional)
    int retrieved_int;
    char *retrieved_str;
    
    CU_ASSERT_EQUAL(get_value(df, 0, 1, &retrieved_int), 0);
    CU_ASSERT_EQUAL(retrieved_int, sample_int);
    
    CU_ASSERT_EQUAL(get_value(df, num_rows - 1, 0, &retrieved_str), 0);
    CU_ASSERT_STRING_EQUAL(retrieved_str, sample_str);
    
    // Estimate and print memory usage
    size_t estimated_memory = estimate_memory_usage(num_rows, num_columns);
    printf("Estimated Memory Usage: %.2f MB\n", estimated_memory / (1024.0 * 1024.0));
    
    // Note: sizeof(df) only gives the size of the pointer
    printf("Size of df pointer: %zu bytes\n", sizeof(df));
    
    // Clean up
    destroy_dataframe(df);
}

// Main function to run tests
int main() {
    // Initialize CUnit
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }
    
    // Create a test suite
    CU_pSuite suite = CU_add_suite("DataFrame Suite", NULL, NULL);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // Add tests to the suite
    if ((CU_add_test(suite, "test_create_dataframe", test_create_dataframe) == NULL) ||
        (CU_add_test(suite, "test_big_dataframe", test_big_dataframe) == NULL)) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // Run the tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    // Clean up
    CU_cleanup_registry();
    return CU_get_error();
}
