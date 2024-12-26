#include "/home/tommy/ProgettoSO/Utils/ClientUtils.h"



// RESULTS PATH

#define RESULTS_PATH "/home/tommy/ProgettoSO/Client/results/AddressBook"



// MESSAGE LENGTH:

#define BUFFER_SIZE 173



// OUTCOME/ERROR CODES FROM SERVER:

#define POSITIVE 0
#define ERROR_OCCURED -1
#define ALR_EXISTING_CONTACT 1
#define CONTACT_NOT_FOUND 2
#define ZERO_CONTACTS_SAVED 3
#define TRYING_ILLEGAL_ACCESS 4
#define PASSWORD_NOT_CORRECT 5
#define USER_NOT_FOUND 6
#define CONNECTION_ACCEPTED 7
#define TOO_MANY_CLIENTS_CONNECTED 8



// OUTCOME/ERROR STRINGS FROM SERVER:

#define POSITIVE_STR "OPERATION SUCCESSFUL"
#define ERROR_OCCURED_STR "ERR_CODE:[-1] -> UNKOWN ERROR OCCURRED "
#define ALR_EXISTING_CONTACT_STR "ERR_CODE:[1] -> THE CONTACT IS ALREADY PRESENT"
#define CONTACT_NOT_FOUND_STR "ERR_CODE:[2] -> NO SUCH CONTACT FOUND"
#define ZERO_CONTACTS_SAVED_STR "ERR_CODE:[3] -> NO CONTACTS PRESENT IN THE ADDRESS BOOK"
#define TRYING_ILLEGAL_ACCESS_STR "ERR_CODE:[4] -> ILLEGAL ACCESS TRIED (such operation is only for logged clients)"
#define PASSWORD_NOT_CORRECT_STR "ERR_CODE:[5] -> INCORRECT PASSWORD! try again"
#define USER_NOT_FOUND_STR "ERR_CODE:[6] -> NO SUCH USER FOUND"
#define CONNECTION_ACCEPTED_STR "CONNECTION ACCEPTED"
#define TOO_MANY_CLIENTS_CONNECTED_STR "ERR_CODE:[8] -> TOO MANY CLIENTS CONNECTED"



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
#define EXIT '0'

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
typedef char * TOKEN;
#define TOKEN_LENGTH_ 32



// MESSAGE TYPE
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
}Message;



// FUNCTIONS PROTOTYPES
struct sockaddr_in * buildSocketaddress(char * serverAddress, int serverPort);
int connect_To_Server(char * serverAddress, int serverPort);
int tryConnection(int sock, struct sockaddr_in * serv_addr);

Message * choose_operation();
void create_Message_String(char messaggio[], Message * data);
void listContacts(char * listaContatti, int numContatti);

void printMenu(int logged);
void printOutcome(int outcome);

int checkAlphaNumeric(char * string);
int checkNumber(char * string);
int checkInsertedData(char * dataValue, char * typeOfData);

