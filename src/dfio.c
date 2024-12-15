#include "dataframe.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// Function to save the dataframe to a CSV file
void save_to_csv(const DataFrame *df, const char *filename) {
    if (df == NULL || filename == NULL) {
        fprintf(stderr, "DataFrame or filename is NULL\n");
        return;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Could not open file");
        return;
    }

    // Write column names
    for (size_t i = 0; i < df->num_columns; i++) {
        // Check if column name is not empty
        if (df->columns[i].name[0] == '\0') {
            fprintf(stderr, "Error: Column %zu name is empty\n", i);
            fclose(file);
            return;
        }

        fprintf(file, "\"%s\"%s", df->columns[i].name, (i == df->num_columns - 1) ? "\n" : ",");
    }

    // Write data rows
    for (size_t row = 0; row < df->num_rows; row++) {
        for (size_t col = 0; col < df->num_columns; col++) {
            const Column *column = &df->columns[col];
            switch (column->type) {
                case DATA_TYPE_INT:
                    fprintf(file, "%d", column->data.int_data[row]);
                    break;
                case DATA_TYPE_FLOAT:
                    fprintf(file, "%.2f", column->data.float_data[row]);
                    break;
                case DATA_TYPE_STRING:
                    if (column->data.string_data[row] != NULL) {
                        // Escape double quotes by replacing " with ""
                        fprintf(file, "\"");
                        for (char *c = column->data.string_data[row]; *c != '\0'; c++) {
                            if (*c == '\"') {
                                fprintf(file, "\"\"");
                            } else {
                                fprintf(file, "%c", *c);
                            }
                        }
                        fprintf(file, "\"");
                    } else {
                        fprintf(file, "\"NULL\"");
                    }
                    break;
                default:
                    fprintf(file, "\"UNKNOWN\"");
                    break;
            }
            fprintf(file, "%s", (col == df->num_columns - 1) ? "\n" : ",");
        }
    }

    fclose(file);
}

// Function to print the dataframe to the console (for debugging)
void print_dataframe(const DataFrame *df) {
    if (df == NULL) {
        printf("DataFrame is NULL\n");
        return;
    }

    // Print column names
    for (size_t i = 0; i < df->num_columns; i++) {
        printf("%s%s", df->columns[i].name, (i == df->num_columns - 1) ? "\n" : "\t");
    }

    // Print rows
    for (size_t row = 0; row < df->num_rows; row++) {
        for (size_t col = 0; col < df->num_columns; col++) {
            const Column *column = &df->columns[col];
            switch (column->type) {
                case DATA_TYPE_INT:
                    printf("%d\t", column->data.int_data[row]);
                    break;
                case DATA_TYPE_FLOAT:
                    printf("%.2f\t", column->data.float_data[row]);
                    break;
                case DATA_TYPE_STRING:
                    if (column->data.string_data[row] != NULL) {
                        printf("%s\t", column->data.string_data[row]);
                    } else {
                        printf("NULL\t");
                    }
                    break;
                default:
                    printf("UNKNOWN\t");
                    break;
            }
        }
        printf("\n");
    }
}

/**
 * Helper function to trim whitespace from both ends of a string.
 */
static char *trim_whitespace(char *str) {
    char *end;
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0)  // All spaces?
        return str;
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    // Write new null terminator
    end[1] = '\0';
    return str;
}

/**
 * Helper function to split a CSV line into fields.
 * Handles basic quoted fields.
 */
static int split_csv_line(char *line, char ***fields, size_t *num_fields) {
    size_t capacity = 10; // Initial capacity
    size_t count = 0;
    char **result = malloc(capacity * sizeof(char *));
    if (!result) {
        fprintf(stderr, "Memory allocation failed in split_csv_line\n");
        return -1;
    }

    char *ptr = line;
    while (*ptr) {
        // Allocate space for a new field
        if (count >= capacity) {
            capacity *= 2;
            char **temp = realloc(result, capacity * sizeof(char *));
            if (!temp) {
                fprintf(stderr, "Memory reallocation failed in split_csv_line\n");
                free(result);
                return -1;
            }
            result = temp;
        }

        // Check if field is quoted
        if (*ptr == '"') {
            ptr++; // Skip opening quote
            char *start = ptr;
            while (*ptr && !(*ptr == '"' && (*(ptr + 1) == ',' || *(ptr + 1) == '\0'))) {
                if (*ptr == '"' && *(ptr + 1) == '"') {
                    ptr += 2; // Skip escaped quote
                } else {
                    ptr++;
                }
            }
            size_t len = ptr - start;
            char *field = malloc(len + 1);
            if (!field) {
                fprintf(stderr, "Memory allocation failed for field in split_csv_line\n");
                // Free previously allocated fields
                for (size_t i = 0; i < count; i++) free(result[i]);
                free(result);
                return -1;
            }
            strncpy(field, start, len);
            field[len] = '\0';
            // Replace double quotes with single quotes
            char *src = field, *dst = field;
            while (*src) {
                if (*src == '"' && *(src + 1) == '"') {
                    *dst++ = '"';
                    src += 2;
                } else {
                    *dst++ = *src++;
                }
            }
            *dst = '\0';
            result[count++] = field;
            if (*ptr == '"') ptr++; // Skip closing quote
            if (*ptr == ',') ptr++; // Skip comma
        } else {
            // Unquoted field
            char *start = ptr;
            while (*ptr && *ptr != ',') ptr++;
            size_t len = ptr - start;
            char *field = malloc(len + 1);
            if (!field) {
                fprintf(stderr, "Memory allocation failed for field in split_csv_line\n");
                for (size_t i = 0; i < count; i++) free(result[i]);
                free(result);
                return -1;
            }
            strncpy(field, start, len);
            field[len] = '\0';
            result[count++] = trim_whitespace(field);
            if (*ptr == ',') ptr++; // Skip comma
        }
    }

    *fields = result;
    *num_fields = count;
    return 0;
}

/**
 * Function to count the number of lines in a file.
 */
static size_t count_lines(FILE *fp) {
    size_t lines = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') lines++;
    }
    rewind(fp);
    return lines;
}

/**
 * Function to read a CSV file and create a DataFrame.
 * 
 * @param filename The path to the CSV file.
 * @param types An array specifying the DataType for each column.
 * @param num_columns The number of columns.
 * @return Pointer to the created DataFrame, or NULL on failure.
 */
DataFrame *read_csv(const char *filename, DataType *types, size_t num_columns) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file '%s'\n", filename);
        return NULL;
    }

    // Read the header line
    char line[1024];
    if (!fgets(line, sizeof(line), fp)) {
        fprintf(stderr, "Failed to read header from '%s'\n", filename);
        fclose(fp);
        return NULL;
    }

    // Split header into fields
    char **header_fields = NULL;
    size_t header_count = 0;
    if (split_csv_line(line, &header_fields, &header_count) != 0) {
        fclose(fp);
        return NULL;
    }

    // Validate column count
    if (header_count != num_columns) {
        fprintf(stderr, "Header column count (%zu) does not match expected (%zu)\n", header_count, num_columns);
        for (size_t i = 0; i < header_count; i++) free(header_fields[i]);
        free(header_fields);
        fclose(fp);
        return NULL;
    }

    // Create the DataFrame with an estimated number of rows
    size_t estimated_rows = count_lines(fp) - 1; // Subtract header
    DataFrame *df = create_dataframe(estimated_rows, num_columns);
    if (!df) {
        for (size_t i = 0; i < header_count; i++) free(header_fields[i]);
        free(header_fields);
        fclose(fp);
        return NULL;
    }

    // Add columns with specified types
    for (size_t i = 0; i < num_columns; i++) {
        if (add_column(df, types[i], i, header_fields[i]) != 0) {
            fprintf(stderr, "Failed to add column '%s'\n", header_fields[i]);
            // Cleanup
            for (size_t j = 0; j < header_count; j++) free(header_fields[j]);
            free(header_fields);
            destroy_dataframe(df);
            fclose(fp);
            return NULL;
        }
    }

    // Cleanup header fields
    for (size_t i = 0; i < header_count; i++) free(header_fields[i]);
    free(header_fields);

    // Read and parse each data line
    size_t current_row = 0;
    while (fgets(line, sizeof(line), fp)) {
        // Remove potential newline characters
        line[strcspn(line, "\r\n")] = 0;

        // Split the line into fields
        char **fields = NULL;
        size_t field_count = 0;
        if (split_csv_line(line, &fields, &field_count) != 0) {
            fprintf(stderr, "Failed to parse line %zu\n", current_row + 2); // +2 for header and 0-index
            destroy_dataframe(df);
            fclose(fp);
            return NULL;
        }

        // Validate field count
        if (field_count != num_columns) {
            fprintf(stderr, "Field count (%zu) does not match number of columns (%zu) at line %zu\n", field_count, num_columns, current_row + 2);
            for (size_t i = 0; i < field_count; i++) free(fields[i]);
            free(fields);
            destroy_dataframe(df);
            fclose(fp);
            return NULL;
        }

        // Set values in the DataFrame
        for (size_t i = 0; i < num_columns; i++) {
            if (types[i] == DATA_TYPE_INT) {
                int value = atoi(fields[i]);
                if (set_value(df, current_row, i, &value) != 0) {
                    fprintf(stderr, "Failed to set INT value at row %zu, column %zu\n", current_row, i);
                }
            } else if (types[i] == DATA_TYPE_FLOAT) {
                float value = atof(fields[i]);
                if (set_value(df, current_row, i, &value) != 0) {
                    fprintf(stderr, "Failed to set FLOAT value at row %zu, column %zu\n", current_row, i);
                }
            } else if (types[i] == DATA_TYPE_STRING) {
                if (set_value(df, current_row, i, fields[i]) != 0) {
                    fprintf(stderr, "Failed to set STRING value at row %zu, column %zu\n", current_row, i);
                }
            }
        }

        // Cleanup fields
        for (size_t i = 0; i < field_count; i++) free(fields[i]);
        free(fields);

        current_row++;
    }

    // Optionally, resize the dataframe if estimated rows were inaccurate
    if (current_row < df->num_rows) {
        df->num_rows = current_row;
    }

    fclose(fp);
    return df;
}