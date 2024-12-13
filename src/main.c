// File: src/main.c

#include "dataframe.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    size_t num_rows = 3;
    size_t num_columns = 2;
    DataFrame *df = create_dataframe(num_rows, num_columns);
    if (df == NULL) {
        return EXIT_FAILURE;
    }

    // Add columns with error checking
    if (add_column(df, 0, "Age", DATA_TYPE_INT) != 0) {
        destroy_dataframe(df);
        return EXIT_FAILURE;
    }
    if (add_column(df, 1, "Name", DATA_TYPE_STRING) != 0) {
        destroy_dataframe(df);
        return EXIT_FAILURE;
    }

    // Set values for Age
    int age_values[3] = {25, 30, 22};
    for (size_t row = 0; row < num_rows; row++) {
        if (set_value(df, row, 0, &age_values[row]) != 0) {
            destroy_dataframe(df);
            return EXIT_FAILURE;
        }
    }

    // Set values for Name
    const char *names[3] = {"Alice", "Bob", "Charlie"};
    for (size_t row = 0; row < num_rows; row++) {
        if (set_value(df, row, 1, (void *)names[row]) != 0) {
            destroy_dataframe(df);
            return EXIT_FAILURE;
        }
    }

    // Print dataframe to console
    print_dataframe(df);

    // Save to CSV
    save_to_csv(df, "data.csv");

    // Free allocated memory
    destroy_dataframe(df);

    return EXIT_SUCCESS;
}
