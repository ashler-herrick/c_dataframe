#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <stdlib.h>

// Maximum length for column names
#define MAX_COLUMN_NAME_LENGTH 64

// Enum for supported data types
typedef enum {
    DATA_TYPE_INT = 0,
    DATA_TYPE_FLOAT = 1,
    DATA_TYPE_STRING = 2
} DataType;

// Holds the data for a column in the form of type-specific arrays.
typedef union {
    int *int_data;          
    float *float_data;
    char **string_data;
} ColumnData;

// Represents a single column in a dataframe.
typedef struct {
    char name[MAX_COLUMN_NAME_LENGTH]; // Column name
    DataType type;                     // Data type of the column
    ColumnData data;                   // Union containing the actual data
} Column;

// Represents a collection of columns and their associated data, forming a 2D data structure (dataframe).
typedef struct {
    Column *columns;    // Pointer to an array of Column structs
    size_t num_columns; // Number of columns
    size_t num_rows;    // Number of rows
} DataFrame;

// Function Prototypes

/**
 * Creates a new DataFrame with the specified number of rows and columns.
 *
 * @param num_rows The number of rows in the DataFrame.
 * @param num_columns The number of columns in the DataFrame.
 * @return A pointer to the newly created DataFrame, or NULL on failure.
 */
DataFrame *create_dataframe(size_t num_rows, size_t num_columns);

/**
 * Adds a column to the DataFrame at the specified index.
 *
 * @param df Pointer to the DataFrame.
 * @param column_index The index at which to add the column.
 * @param name The name of the column.
 * @param type The data type of the column.
 * @return 0 on success, -1 on failure.
 */
int add_column(DataFrame *df, DataType type, size_t column_index, const char *name);

/**
 * Sets a value in the DataFrame at the specified row and column.
 *
 * @param df Pointer to the DataFrame.
 * @param row The row index.
 * @param column The column index.
 * @param value Pointer to the value to set.
 * @return 0 on success, -1 on failure.
 */
int set_value(DataFrame *df, size_t row, size_t column, const void *value);

/**
 * Gets a value from the DataFrame at the specified row and column.
 *
 * @param df Pointer to the DataFrame.
 * @param row The row index.
 * @param column The column index.
 * @param output Pointer to store the retrieved value.
 * @return 0 on success, -1 on failure.
 */
int get_value(const DataFrame *df, size_t row, size_t column, void *output);


/**
 * Frees all allocated memory within the DataFrame.
 *
 * @param df Pointer to the DataFrame to destroy.
 */
void destroy_dataframe(DataFrame *df);

#endif // DATAFRAME_H
