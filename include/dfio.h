#ifndef DFIO_H
#define DFIO_H

#include <stdlib.h>


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