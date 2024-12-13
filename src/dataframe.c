// File: src/dataframe.c

#include "dataframe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Function to create a new dataframe
DataFrame *create_dataframe(size_t num_rows, size_t num_columns) {
    DataFrame *df = malloc(sizeof(DataFrame));
    if (df == NULL) {
        fprintf(stderr, "Memory allocation failed for DataFrame\n");
        return NULL;
    }

    df->columns = malloc(num_columns * sizeof(Column));
    if (df->columns == NULL) {
        fprintf(stderr, "Memory allocation failed for Columns\n");
        free(df);
        return NULL;
    }

    df->num_columns = num_columns;
    df->num_rows = num_rows;

    // Initialize columns
    for (size_t i = 0; i < num_columns; i++) {
        df->columns[i].data.int_data = NULL;
        df->columns[i].name[0] = '\0'; // Initialize name to empty string
    }
    return df;
}

// Function to add a column to the dataframe
int add_column(DataFrame *df, size_t column_index, const char *name, DataType type) {
    if (df == NULL || name == NULL) {
        fprintf(stderr, "DataFrame or name is NULL\n");
        return -1;
    }

    // Check that column is not out of bounds
    if (column_index >= df->num_columns) {
        fprintf(stderr, "Column index %zu out of bounds (max %zu)\n", column_index, df->num_columns - 1);
        return -1;
    }

    Column *col = &df->columns[column_index];

    // Ensure name is not empty
    if (strlen(name) == 0){
        fprintf(stderr, "Column name cannot be empty\n");
        return -1;
    }

    strncpy(col->name, name, MAX_COLUMN_NAME_LENGTH - 1);
    col->name[MAX_COLUMN_NAME_LENGTH - 1] = '\0'; // Ensure null termination
    col->type = type;

    // Allocate memory for the column data with error checking
    switch (type) {
        case DATA_TYPE_INT:
            col->data.int_data = malloc(df->num_rows * sizeof(int));
            if (col->data.int_data == NULL) {
                fprintf(stderr, "Memory allocation failed for INT column '%s'\n", name);
                return -1;
            }
            memset(col->data.int_data, 0, df->num_rows * sizeof(int)); // Initialize to 0
            break;
        case DATA_TYPE_FLOAT:
            col->data.float_data = malloc(df->num_rows * sizeof(float));
            if (col->data.float_data == NULL) {
                fprintf(stderr, "Memory allocation failed for FLOAT column '%s'\n", name);
                return -1;
            }
            memset(col->data.float_data, 0, df->num_rows * sizeof(float)); // Initialize to 0.0
            break;
        case DATA_TYPE_STRING:
            col->data.string_data = malloc(df->num_rows * sizeof(char *));
            if (col->data.string_data == NULL) {
                fprintf(stderr, "Memory allocation failed for STRING column '%s'\n", name);
                return -1;
            }
            memset(col->data.string_data, 0, df->num_rows * sizeof(char *)); // Initialize to NULL
            break;
        default:
            fprintf(stderr, "Unsupported DataType %d\n", type);
            return -1;
    }

    return 0;
}

// Function to set a value in the dataframe
int set_value(DataFrame *df, size_t row, size_t column, const void *value) {
    if (df == NULL || value == NULL) {
        fprintf(stderr, "DataFrame or value is NULL\n");
        return -1;
    }

    if (column >= df->num_columns || row >= df->num_rows) {
        fprintf(stderr, "Index out of bounds (row: %zu, column: %zu)\n", row, column);
        return -1;
    }

    Column *col = &df->columns[column];
    switch (col->type) {
        case DATA_TYPE_INT:
            col->data.int_data[row] = *(int *)value;
            break;
        case DATA_TYPE_FLOAT:
            col->data.float_data[row] = *(float *)value;
            break;
        case DATA_TYPE_STRING: {
            char **str_ptr = &col->data.string_data[row];
            if (*str_ptr != NULL) {
                free(*str_ptr);
            }
            *str_ptr = strdup((char *)value);
            if (*str_ptr == NULL) {
                fprintf(stderr, "strdup failed for row %zu, column %zu\n", row, column);
                return -1;
            }
            break;
        }
        default:
            fprintf(stderr, "Unsupported DataType %d\n", col->type);
            return -1;
    }
    return 0;
}

// Function to get a value from the dataframe
int get_value(const DataFrame *df, size_t row, size_t column, void *output) {
    if (df == NULL || output == NULL) {
        fprintf(stderr, "DataFrame or output is NULL\n");
        return -1;
    }

    if (column >= df->num_columns || row >= df->num_rows) {
        fprintf(stderr, "Index out of bounds (row: %zu, column: %zu)\n", row, column);
        return -1;
    }

    const Column *col = &df->columns[column];
    switch (col->type) {
        case DATA_TYPE_INT:
            *(int *)output = col->data.int_data[row];
            break;
        case DATA_TYPE_FLOAT:
            *(float *)output = col->data.float_data[row];
            break;
        case DATA_TYPE_STRING:
            *(char **)output = col->data.string_data[row];
            break;
        default:
            fprintf(stderr, "Unsupported DataType %d\n", col->type);
            return -1;
    }
    return 0;
}


// Function to free all allocated memory in the dataframe
void destroy_dataframe(DataFrame *df) {
    if (df == NULL) return;

    // Free each column's data
    for (size_t i = 0; i < df->num_columns; i++) {
        Column *col = &df->columns[i];
        if (col->data.int_data != NULL || col->data.float_data != NULL || col->data.string_data != NULL) {
            if (col->type == DATA_TYPE_STRING) {
                // Free each string in the column
                for (size_t row = 0; row < df->num_rows; row++) {
                    if (col->data.string_data[row] != NULL) {
                        free(col->data.string_data[row]);
                        col->data.string_data[row] = NULL;
                    }
                }
            }
            // Free the data array
            switch (col->type) {
                case DATA_TYPE_INT:
                    free(col->data.int_data);
                    break;
                case DATA_TYPE_FLOAT:
                    free(col->data.float_data);
                    break;
                case DATA_TYPE_STRING:
                    free(col->data.string_data);
                    break;
                default:
                    // Do nothing for unsupported types
                    break;
            }
            col->data.int_data = NULL;
            col->data.float_data = NULL;
            col->data.string_data = NULL;
        }
    }

    // Free columns array
    free(df->columns);
    df->columns = NULL;

    // Free DataFrame
    free(df);
}
