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
