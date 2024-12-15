// File: tests/test_read_csv.c

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataframe.h"
#include "dfio.h"

/**
 * Test function for read_csv.
 * Assumes there's a sample CSV file named "sample.csv" with known content.
 */
void test_read_csv(void) {
    // Define expected number of columns
    size_t num_columns = 3;
    
    // Define data types for each column
    // Example: Column 0 - INT, Column 1 - FLOAT, Column 2 - STRING
    DataType types[3] = {DATA_TYPE_INT, DATA_TYPE_FLOAT, DATA_TYPE_STRING};
    
    // Path to the sample CSV file
    const char *filename = "/home/ubuntu/c_dataframe/data/data.csv";
    
    // Create the CSV file for testing
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        CU_FAIL("Failed to create sample CSV file");
        return;
    }
    fprintf(fp, "ID,Value,Name\n");
    fprintf(fp, "1,3.14,Alice\n");
    fprintf(fp, "2,2.718,Bob\n");
    fprintf(fp, "3,1.618,Charlie\n");
    fclose(fp);
    
    // Call read_csv
    DataFrame *df = read_csv(filename, types, num_columns);
    
    // Assertions
    CU_ASSERT_PTR_NOT_NULL(df);
    if (df) {
        CU_ASSERT_EQUAL(df->num_rows, 3);
        CU_ASSERT_EQUAL(df->num_columns, 3);
        
        // Check column names
        CU_ASSERT_STRING_EQUAL(df->columns[0].name, "ID");
        CU_ASSERT_STRING_EQUAL(df->columns[1].name, "Value");
        CU_ASSERT_STRING_EQUAL(df->columns[2].name, "Name");
        
        // Check data types
        CU_ASSERT_EQUAL(df->columns[0].type, DATA_TYPE_INT);
        CU_ASSERT_EQUAL(df->columns[1].type, DATA_TYPE_FLOAT);
        CU_ASSERT_EQUAL(df->columns[2].type, DATA_TYPE_STRING);
        
        // Check values
        int id;
        float value;
        char *name;
        
        // Row 0
        CU_ASSERT_EQUAL(get_value(df, 0, 0, &id), 0);
        CU_ASSERT_EQUAL(id, 1);
        CU_ASSERT_EQUAL(get_value(df, 0, 1, &value), 0);
        CU_ASSERT_DOUBLE_EQUAL(value, 3.14, 0.001);
        CU_ASSERT_EQUAL(get_value(df, 0, 2, &name), 0);
        CU_ASSERT_STRING_EQUAL(name, "Alice");
        
        // Row 1
        CU_ASSERT_EQUAL(get_value(df, 1, 0, &id), 0);
        CU_ASSERT_EQUAL(id, 2);
        CU_ASSERT_EQUAL(get_value(df, 1, 1, &value), 0);
        CU_ASSERT_DOUBLE_EQUAL(value, 2.718, 0.001);
        CU_ASSERT_EQUAL(get_value(df, 1, 2, &name), 0);
        CU_ASSERT_STRING_EQUAL(name, "Bob");
        
        // Row 2
        CU_ASSERT_EQUAL(get_value(df, 2, 0, &id), 0);
        CU_ASSERT_EQUAL(id, 3);
        CU_ASSERT_EQUAL(get_value(df, 2, 1, &value), 0);
        CU_ASSERT_DOUBLE_EQUAL(value, 1.618, 0.001);
        CU_ASSERT_EQUAL(get_value(df, 2, 2, &name), 0);
        CU_ASSERT_STRING_EQUAL(name, "Charlie");
        
        // Clean up
        destroy_dataframe(df);
    }
    
    // Remove the sample CSV file after testing
    remove(filename);
}

/**
 * Main function to run CUnit tests.
 */
int main() {
    // Initialize CUnit
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }
    
    // Create a test suite
    CU_pSuite suite = CU_add_suite("CSV Import Suite", NULL, NULL);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // Add tests to the suite
    if (CU_add_test(suite, "test_read_csv", test_read_csv) == NULL) {
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
