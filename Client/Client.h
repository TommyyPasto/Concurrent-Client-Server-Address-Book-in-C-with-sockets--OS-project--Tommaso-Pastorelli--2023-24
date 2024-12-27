/**
 * @file Client.h
 * @brief This header file declares functions and data structures for the client
 * application, including network communication, user interface elements, and message handling.
 */
#ifndef CLIENT_H
#define CLIENT_H

#include "/home/tommy/ProgettoSO/Utils/ClientUtils.h" 


// RESULTS PATH

#define RESULTS_PATH "/home/tommy/ProgettoSO/Client/results/AddressBook"
#define CHOICETMP_PATH "/home/tommy/ProgettoSO/Client/tmp/choice.txt"



// MESSAGE LENGTH:

#define BUFFER_SIZE 173



// OUTCOME/ERROR CODES FROM SERVER:

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



// OUTCOME/ERROR STRINGS:

#define POSITIVE_STR "OPERATION SUCCESSFUL"
#define ERROR_OCCURED_STR "ERR_CODE:[1] -> UNKOWN ERROR OCCURRED "
#define ALR_EXISTING_CONTACT_STR "ERR_CODE:[2] -> THE CONTACT IS ALREADY PRESENT"
#define CONTACT_NOT_FOUND_STR "ERR_CODE:[3] -> NO SUCH CONTACT FOUND"
#define ZERO_CONTACTS_SAVED_STR "ERR_CODE:[4] -> NO CONTACTS PRESENT IN THE ADDRESS BOOK"
#define TRYING_ILLEGAL_ACCESS_STR "ERR_CODE:[5] -> ILLEGAL ACCESS TRIED (such operation is only for logged clients)"
#define PASSWORD_NOT_CORRECT_STR "ERR_CODE:[6] -> INCORRECT PASSWORD! try again"
#define USER_NOT_FOUND_STR "ERR_CODE:[7] -> NO SUCH USER FOUND"
#define CONNECTION_ACCEPTED_STR "CONNECTION ACCEPTED"
#define TOO_MANY_CLIENTS_CONNECTED_STR "ERR_CODE:[8] -> TOO MANY CLIENTS CONNECTED"

//              |
//              V

// OUTCOME/ERROR STRINGS FROM SERVER:

const char * outcome_strings[] = {
    GRN"\n\n            "POSITIVE_STR RESET"\n",
    RED"\n\n            "ERROR_OCCURED_STR RESET"\n",
    RED "\n\n           "ALR_EXISTING_CONTACT_STR RESET"\n",
    RED"\n\n            "CONTACT_NOT_FOUND_STR RESET"\n",
    RED"\n\n            "ZERO_CONTACTS_SAVED_STR RESET"\n",
    RED"\n\n            "TRYING_ILLEGAL_ACCESS_STR RESET"\n",
    RED"\n\n            "PASSWORD_NOT_CORRECT_STR RESET"\n",
    RED"\n\n            "USER_NOT_FOUND_STR RESET"\n",
    GRN"\n\n            "CONNECTION_ACCEPTED_STR RESET"\n",
    RED"\n\n            "TOO_MANY_CLIENTS_CONNECTED_STR RESET"\n"
};



// CONNECTION ERROR STRINGS

#define SOCKET_CREATION_ERROR "an ERROR occurred during sockets creation!"
#define CONNECTION_ERROR "CONNECTION FAILED, RETRY? [Y][N or other]"
#define SERVER_DISCONNECTED_ERROR "ERROR: server has stopped running!\nso data was not sent!"



// OPERATION VALUES

#define LISTING '1'
#define INSERT '2'
#define EDIT '3'
#define DELETE '4'
#define LOGIN '+'
#define LOGOUT '-'
#define EXIT 27

#define checkConnessione 'c'



// MESSAGE ARRAY FIXED INDEXES

#define POS_NAME 1
#define POS_LAST_NAME 21
#define POS_PHONE_NUM 41
#define POS_NEW_NAME 51
#define POS_NEW_LAST_NAME 71
#define POS_NEW_PHONE_NUM 91
#define POS_USERNAME 101
#define POS_PSW 121
#define POS_TOKEN 141



// TOKEN TYPE AND LENGTH
/**
 * @brief Type definition for a session token.
 */
typedef char * TOKEN;
/**
 * @brief Length of the session token string.
 */
#define TOKEN_LENGTH_ 32



// MESSAGE TYPE
/**
 * @brief Structure for messages exchanged between client and server.
 * Contains fields for operation code, contact details, user credentials, etc.
 */
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
    char token[32];  // Add token field to the Message struct
}Message;



// FUNCTIONS PROTOTYPES
/**
 * @brief Builds a sockaddr_in structure for the server.
 * 
 * @param serverAddress The server's IP address.
 * @param serverPort The server's port number.
 * @return A pointer to the sockaddr_in structure.  The caller is responsible for freeing the allocated memory.
 */
struct sockaddr_in * buildSocketaddress(char * serverAddress, int serverPort);



/**
 * @brief Initiates a connection to the server.
 *
 * @param serverAddress The server's IP address.
 * @param serverPort The server's port number.
 * @return The socket file descriptor on success, a negative value on error.
 */
int connect_To_Server(char * serverAddress, int serverPort);



/**
 * @brief Attempts to connect to the server using the given socket and address.  This function will retry the connection if the initial attempt fails or if there are too many clients connected.
 *
 * @param sock The socket file descriptor.
 * @param serv_addr  Pointer to the sockaddr_in structure.
 * @return The socket file descriptor on successful connection, or it exits on fatal error.
 */
int tryConnection(int sock, struct sockaddr_in * serv_addr);



/**
 * @brief Presents the user with a menu of operations and returns their choice.
 * This function also handles user input for each operation.
 *
 * @return A pointer to the Message struct filled with user's choices. The caller is responsible for freeing the allocated memory.
 */
Message * choose_operation();



/**
 * @brief Creates a string representation of a Message struct.
 *
 * @param message The character array to store the string representation.
 * @param data A pointer to the Message struct.
 */
void create_Message_String(char message[], Message * data);



/**
 * @brief Lists contacts received from the server.
 *
 * @param listaContatti character pointer containing list of contacts and the outcome as its first byte
 * @param numContatti The number of contacts in the list.
 */
char * listContacts(char * listaContatti, int numContatti);



/**
 * @brief Prints the main menu for the user interface based on log in status.
 *
 * @param logged  1 if user is currently logged, 0 otherwise.
 */
void printMenu(const char **options, int num_options, int selected, char * nextString);


/**
 * @brief Prints the outcome of an operation to the console.
 *
 * @param outcome The outcome code to print.
 */
void printOutcome(int outcome);



/**
 * @brief Validates user input for name, last name, phone number, username, and password.
 *
 * @param dataValue The input string to validate.
 * @param typeOfData The type of data being validated ("name", "last name", "phone number", "username", or "password").
 * @return 0 if the input is valid, -1 otherwise, and -2 if typeOfData isn't one of the options above.
 */
int checkInsertedData(char * dataValue, char * typeOfData);



/**
 * @brief Prints the menu to the console.
 *
 * @param options The array of menu options to display.
 * @param num_options The number of options in the `options` array.
 * @param selected The index of the currently selected option.
 * @param nextString A string to be printed after the menu options for additional messages or information.
 */
void printMenu(const char **options, int num_options, int selected, char *nextString);



/**
 * @brief Displays an interactive menu and returns the user's selected option.
 *
 * @param logged An integer indicating whether the user is logged in (1) or not (0).  This determines which set of menu options to display.
 * @param options The array of menu options to display.
 * @param nextString A string to print below the menu for additional error messages.
 * @return The index (1-based) of the selected option. Returns 0 if the user chooses to exit.
 */
int menu(int logged, const char **options, char *nextString);



#endif