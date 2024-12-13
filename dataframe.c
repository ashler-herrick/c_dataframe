#include <stdlib.h>
#include <stdio.h>

#define MAX_COLUMN_NAME_LENGTH 64

// Enum for supported data types
typedef enum {
    INT,
    FLOAT,
    STRING
} DataType;

// Structure for a single column
typedef struct {
    char name[MAX_COLUMN_NAME_LENGTH]; // Column name
    DataType type;                     // Data type of the column
    void *data;                        // Pointer to the data array
} Column;

// Structure for the dataframe
typedef struct {
    Column *columns;    // Array of columns
    unsigned int num_columns; // Number of columns
    unsigned int num_rows;    // Number of rows
} DataFrame;


DataFrame *create_dataframe(unsigned int num_rows, unsigned int num_columns) {
    DataFrame *df = (DataFrame *)malloc(sizeof(DataFrame));
    df->columns = (Column *)malloc(num_columns * sizeof(Column));
    df->num_columns = num_columns;
    df->num_rows = num_rows;

    // Initialize columns
    for (unsigned int i = 0; i < num_columns; i++) {
        df->columns[i].data = NULL;
    }
    return df;
}


void add_column(DataFrame *df, unsigned int column_index, const char *name, DataType type) {
    if (column_index >= df->num_columns) {
        fprintf(stderr, "Column index out of bounds\n");
        return;
    }

    Column *col = &df->columns[column_index];
    strncpy(col->name, name, MAX_COLUMN_NAME_LENGTH);
    col->type = type;

    // Allocate memory for the column data
    switch (type) {
        case INT:
            col->data = malloc(df->num_rows * sizeof(int));
            break;
        case FLOAT:
            col->data = malloc(df->num_rows * sizeof(float));
            break;
        case STRING:
            col->data = malloc(df->num_rows * sizeof(char *));
            break;
    }
}


void set_value(DataFrame *df, size_t row, size_t column, void *value) {
    if (column >= df->num_columns || row >= df->num_rows) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    Column *col = &df->columns[column];
    switch (col->type) {
        case INT:
            ((int *)col->data)[row] = *(int *)value;
            break;
        case FLOAT:
            ((float *)col->data)[row] = *(float *)value;
            break;
        case STRING:
            ((char **)col->data)[row] = strdup((char *)value);
            break;
    }
}


void get_value(DataFrame *df, size_t row, size_t column, void *output) {
    if (column >= df->num_columns || row >= df->num_rows) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    Column *col = &df->columns[column];
    switch (col->type) {
        case INT:
            *(int *)output = ((int *)col->data)[row];
            break;
        case FLOAT:
            *(float *)output = ((float *)col->data)[row];
            break;
        case STRING:
            *(char **)output = ((char **)col->data)[row];
            break;
    }
}


void save_to_csv(DataFrame *df, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Could not open file");
        return;
    }

    // Write column names
    for (size_t i = 0; i < df->num_columns; i++) {
        fprintf(file, "%s%s", df->columns[i].name, (i == df->num_columns - 1) ? "\n" : ",");
    }

    // Write data rows
    for (size_t row = 0; row < df->num_rows; row++) {
        for (size_t col = 0; col < df->num_columns; col++) {
            Column *column = &df->columns[col];
            switch (column->type) {
                case INT:
                    fprintf(file, "%d", ((int *)column->data)[row]);
                    break;
                case FLOAT:
                    fprintf(file, "%f", ((float *)column->data)[row]);
                    break;
                case STRING:
                    fprintf(file, "%s", ((char **)column->data)[row]);
                    break;
            }
            fprintf(file, "%s", (col == df->num_columns - 1) ? "\n" : ",");
        }
    }

    fclose(file);
}


int main() {
    DataFrame *df = create_dataframe(3, 2);

    add_column(df, 0, "Age", INT);
    add_column(df, 1, "Name", STRING);

    int age = 25;
    set_value(df, 0, 0, &age);
    set_value(df, 1, 0, &age);

    char *name = "Alice";
    set_value(df, 0, 1, name);
    set_value(df, 1, 1, name);

    save_to_csv(df, "data.csv");

    return 0;
}
