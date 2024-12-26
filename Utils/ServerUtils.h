/**
 * @file ServerUtils.h
 * @brief This header file declares utility functions for the server application.
 * These functions handle tasks such as file locking, time retrieval, SHA256 hashing,
 * hexadecimal conversion, and token generation.
 */
#ifndef SERVERUTILS_H
#define SERVERUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>
#include <openssl/sha.h>


#define RED   "\033[1;31m"  /**< Macro for red terminal output. */
#define GRN   "\033[1;32m"  /**< Macro for green terminal output. */
#define YEL   "\033[1;33m"  /**< Macro for yellow terminal output. */
#define BLU   "\033[1;34m"  /**< Macro for blue terminal output. */
#define MAG   "\033[1;35m"  /**< Macro for magenta terminal output. */
#define CYN   "\033[1;36m"  /**< Macro for cyan terminal output. */
#define WHT   "\033[1;37m"  /**< Macro for white terminal output. */

#define RESET "\x1B[0m"     /**< Macro to reset terminal color. */



//TOKEN TYPE DEF.
/**
 * @brief Type definition for a session token.
 */
typedef char * TOKEN;
/**
 * @brief Length of the session token string.
 */
#define TOKEN_LENGTH_ 32



/**
 * @brief Generates a random alphanumeric token.
 * @param token Output buffer (must be large enough).
 * @param length Token length.
 */
void gen_token(TOKEN token, size_t length);


/**
 * @brief Computes SHA256 hash of a string.
 * @param str Input string.
 * @param hash Output buffer (at least 32 bytes).
 * @return Pointer to the hash.
 */
unsigned char * convertToSHA256(char * str, unsigned char * output);


/**
 * @brief Converts hash to hexadecimal string.
 * @param hash Hash value.
 * @param output Output buffer (must be large enough).
 * @param length Hash length.
 */
void to_hex(const unsigned char *hash, char *output, size_t length);


/**
 * @brief Gets the current time as a string.
 * @return A dynamically allocated string containing the current time.  The caller is responsible for freeing the memory.
 */
char * getCurrentTimeStr();


/**
 * @brief Locks a file for reading.
 *
 * @param stream  File pointer.
 * @param start  Starting offset for the lock.
 * @param end    Ending offset for the lock. Use EOF for the end of the file.
 * @param cmd    fcntl command (e.g., F_SETLKW, F_SETLK).
 * @return A pointer to the flock structure used for locking.  The memory allocated for flock should be handled by the caller.
 */
struct flock * lockRD(FILE * stream, int start, int end, int cmd);


/**
 * @brief Locks a file for writing.
 *
 * @param stream  File pointer.
 * @param start  Starting offset for the lock.
 * @param end    Ending offset for the lock. Use EOF for the end of the file.
 * @param cmd    fcntl command (e.g., F_SETLKW, F_SETLK).
 * @return A pointer to the flock structure used for locking.
 */
struct flock * lockWR(FILE * stream, int start, int end, int cmd);


/**
 * @brief Unlocks a previously locked file.
 *
 * @param stream File pointer.
 * @param fl     Pointer to the flock structure used for locking.
 * @return 0 if the file was locked, -1 if it was not locked.
 */
int unlockFile(FILE * stream, struct flock * fl);

#endif
