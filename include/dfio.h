#ifndef DFIO_H
#define DFIO_H

#include <stdlib.h>
#include "dataframe.h"

/**
 * Saves the DataFrame to a CSV file.
 *
 * @param df Pointer to the DataFrame.
 * @param filename The name of the CSV file.
 */
void save_to_csv(const DataFrame *df, const char *filename);

/**
 * Prints the DataFrame to the console (for debugging).
 *
 * @param df Pointer to the DataFrame.
 */
void print_dataframe(const DataFrame *df);


/**
 * @brief Creates a dataframe from a CSV file
 * 
 * @param filename 
 * @param types 
 * @param num_columns 
 * @return DataFrame* 
 */
DataFrame *read_csv(const char *filename, DataType *types, size_t num_columns);

#endif //DFIO_H