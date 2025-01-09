/**
 * @file Server.h
 * @brief This header file declares functions and data structures for the server
 * application, including network communication, address book management, user authentication, and logging.
 */
#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include "errno.h"
#include "../Utils/ServerUtils.h"


#define MAX_CLIENTS_ 5


#define BUFFER_SIZE 173


char * CONTACTS_PATH = "Server/files/contatti.txt";
char * USERS_PATH = "Server/files/utenti.txt";
char * LOG_PATH = "Server/files/log.txt";




//OUTCOME CODES

#define POSITIVE 0
#define ERROR_OCCURED 1
#define ALR_EXISTING_CONTACT 2
#define CONTACT_NOT_FOUND 3
#define ZERO_CONTACTS_SAVED 4
#define TRYING_ILLEGAL_ACCESS 5
#define PASSWORD_NOT_CORRECT 6
#define USER_NOT_FOUND 7
#define TOO_MANY_CLIENTS_CONNECTED 8

#define CONNECTION_ACCEPTED 9



//OPER. NUMS

#define LISTING '1'
#define SEARCH '2'
#define INSERT '3'
#define EDIT '4'
#define DELETE '5'
#define LOGIN '+'
#define LOGOUT '-'



//EXTRA NUMS

#define SUCCESSFUL_CONNECTION_ATTEMPT 1
#define UNSUCCESSFUL_CONNECTION_ATTEMPT 2
#define DISCONNECTION -1

#define SUCCESSFUL_LOGIN_ATTEMPT 3
#define UNSUCCESSFUL_LOGIN_ATTEMPT 4



// MESSAGE FIELDS INDEXES

#define POS_NAME 1
#define POS_LAST_NAME 21
#define POS_PHONE_NUM 41
#define POS_NEW_NAME 51
#define POS_NEW_LAST_NAME 71
#define POS_NEW_PHONE_NUM 91
#define POS_USERNAME 101
#define POS_PSW 121
#define POS_TOKEN 141



typedef struct{
    char operation;
    char name[20];
    char lastName[20];
    char phoneNumber[10];
    char new_name[20];
    char new_lastName[20];
    char new_phoneNumber[10];
    char username[20];
    char psw[20];
    char token[32];
}Message;



/**
 * @brief Global pointer to the current session token.  This token is used
 * to authenticate client requests after login.  It should be initialized during the login process.
 */
extern TOKEN * sessionToken;



/**
 * @brief Sockets for Server side and new clients
 */
extern int server_socket, client_socket;


/**
 * @brief address of the current served client
*/
extern char client_ip[20]; 


/**
 * @brief Global counter for the number of currently connected clients.
 */
extern int connectedClients;


/**
 * @brief pid used for logging
 */
extern int pid;


/**
 * @brief Sets up a socket for the server.
 *
 * @param option Socket option for `setsockopt`. Typically 1 to enable `SO_REUSEADDR`.
 * @return The file descriptor of the created socket, or a negative value on error.
 */
int socketSetUp(int option);



/**
 * @brief Binds a socket to a specified port.
 *
 * @param socket The file descriptor of the socket to bind.
 * @param port The port number to bind to.
 * @return A pointer to the sockaddr_in structure, or NULL on error.  Memory allocated for sockaddr_in is not freed by the function and should be handled by the caller.
 */
struct sockaddr_in * binding(int socket, int port);


/**
 * @brief Signal handler for SIGINT (Ctrl+C). Closes sockets and exits gracefully.
 * @param signum The signal number (should be SIGINT).
 */
void sigintHandler(int signum);


/**
 * @brief Signal handler for SIGCHLD (child process terminated). Decrements the connected client count.
 * @param signum The signal number (should be SIGCHLD).
 */
void sigchldHandler(int signum);


/**
 * @brief Signal handler for SIGPIPE (broken pipe). Logs the disconnection event and closes the client socket.
 * @param signum The signal number (should be SIGPIPE).
 */
void sigpipeHandler(int signum);


/**
 * @brief Rejects a client connection due to too many clients being connected.
 * Sends an appropriate error message to the client.
 * @param client_socket The client socket to reject.
 */
void rejectConnection(int client_socket);


/**
 * @brief Accepts a client connection after verifying available space.
 * @param client_socket The client socket to accept.
 */
void acceptConnection(int client_socket);


/**
 * @brief Executes the requested operation based on the 'data->operation' field.
 *
 * @param data  Message struct containing client request data.
 * @return An integer status code indicating the outcome of the operation.
 */
int execute_operation(Message * data);


/**
 * @brief Reads all contacts from the address book file.
 *
 * @return A character array containing the contacts. The first two bytes contain status information{3: "ZERO_CONTACTS_SAVED" or 0: "POSITIVE"}.
 */
char * readContacts();


/**
 * @brief Inserts a new contact into the address book.
 *
 * @param data  The contact data.
 * @return The outcome of the operation.
 */
int insertContact(Message * data);


/**
 * @brief Edits an existing contact in the address book.
 *
 * @param data  The old + new contact data.
 * @return The outcome of the operation.
 */
int editContact(Message * data);


/**
 * @brief Deletes a contact from the address book.
 *
 * @param data  The contact data.
 * @return The outcome of the operation.
 */
int deleteContact(Message * data);


/**
 * @brief Verifies if a given token matches the current session token.
 *
 * @param data  The token and username and psw to check.
 * @return 1 if the token matches, -1 otherwise.
 */
int login(Message * data);


/**
 * @brief Verifies if a given token matches the current session token.
 *
 * @param token  The token to check.
 * @return 1 if the token matches, -1 otherwise.
 */
int checkLoginSession(TOKEN token);



/**
 * @brief Handles user logout.
 *
 * @param data The session token of the user logging out.
 */
void logout(Message * data);


/**
 * @brief Parses a message string into a Message struct.
 *
 * @param msg The message string.
 * @return A pointer to the allocated Message struct, or NULL on error.
 */
Message * deconstruct_Message_String(char * msg);



/**
 * @brief Searches for a user and sets the file pointer.
 *
 * @param users File pointer to the users file.
 * @param data User data to search for.
 * @return fseek result, or -1 if not found.
 * @pre Users file open in read mode.
 * @post File pointer at user record, or undefined if not found.
 */
int search_And_Set_UserIndex(FILE * users, Message * data);


/**
 * @brief Searches for a contact and sets the file pointer.
 *
 * @param contacts File pointer to the contacts file.
 * @param data Contact data.
 * @return fseek result, or -1 if not found.
 * @pre Contacts file open in read mode.
 * @post File pointer at contact record, or undefined if not found.
 */
int search_And_Set_ContactIndex(FILE * contacts, Message * data);



/**
 * @brief Counts contacts in the address book file.
 *
 * @param file  File pointer to open file.
 * @param recordSize Size of each record in the file.
 * @return Number of contacts.
 * @pre Contacts file open in read mode.
 * @post File pointer at end of file.
 */
int totalNumberOfRecords(FILE * file, int recordSize);


/**
 * @brief  Updates or removes a contact in the address book file.
 * 
 * @param data Contact details (including new details for editing).
 * @return Operation outcome (success, not found, or error).
 * @pre Address book file must exist.
 */
int rewriteAddressBook(Message * data);


/**
 * @brief Logs an event to the server's log file.
 *
 * @param eventCode  The code representing the event.
 * @param data       A pointer to a Message struct containing data related to the event (may be NULL).
 * @param address    The IP address of the client involved in the event.
 * @param time       A string representing the timestamp of the event.
 * @return An integer status code indicating the outcome.
 */
int logAnEvent(int eventCode, Message * data, char * address, char * time);



#endif







