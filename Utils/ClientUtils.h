/**
 * @file ClientUtils.h
 * @brief This header file declares utility functions for the client application.
 * These functions handle tasks such as input validation, display management,
 * and file operations.
 */
#ifndef CLIENTUTILS_H
#define CLIENTUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

#define RED   "\033[1;31m"  /**< Macro for red terminal output. */
#define GRN   "\033[1;32m"  /**< Macro for green terminal output. */
#define YEL   "\033[1;33m"  /**< Macro for yellow terminal output. */
#define BLU   "\033[1;34m"  /**< Macro for blue terminal output. */
#define MAG   "\033[1;35m"  /**< Macro for magenta terminal output. */
#define CYN   "\033[1;36m"  /**< Macro for cyan terminal output. */
#define WHT   "\033[1;37m"  /**< Macro for white terminal output. */

#define RESET "\x1B[0m"     /**< Macro to reset terminal color. */

/**
 * @brief Checks if a string contains only alphanumeric characters.
 * 
 * @param string The string to check.
 * @return 0 if the string is alphanumeric, -1 otherwise.
 */
int checkAlphaNumeric(char * string);



/**
 * @brief Checks if a string contains only numeric characters.
 *
 * @param string The string to check.
 * @return 0 if the string is numeric, -1 otherwise.
 */
int checkNumber(char * string);



/**
 * @brief Clears the last n lines in the terminal output.
 *
 * @param n The number of lines to clear.
 */
void clear_last_n_lines(int n);



/**
 * @brief Checks if a file can be opened for writing.
 * This function attempts to open the file in write-only, exclusive creation mode.
 * If the file exists, this will fail.  This can be used to check if a file
 * already exists before attempting to create it.
 *
 * @param pathname The path to the file.
 * @return The file descriptor if the file can be opened, -1 otherwise.
 */
int checkFileOpened(char * pathname);



/**
 * @brief Saves a set of records into a file, formatting them as specified.
 *
 * @param filepath       The path to the file to save to.
 * @param msg           The data containing the records to save.
 * @param nOfRecords    The number of records to save.
 * @param nOfFields     The number of fields in each record.
 * @param recordsLength The length of each record in characters.
 * @param delim         The delimiter between fields within a record.
 * @param endOfRecDelim The delimiter between records.
 * @return 0 on success, -1 on error.
 */
int saveRecordsInAFile(char * filepath, char * msg, int nOfRecords, int nOfFields, int recordsLength,  const char *__restrict__ delim, const char *__restrict__  endOfRecDelim);



#endif