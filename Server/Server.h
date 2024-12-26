#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <bits/waitflags.h>
#include <signal.h>
#include <sys/wait.h>
#include "errno.h"
#include <openssl/sha.h>
#include "/home/tommy/ProgettoSO/Utils/ServerUtils.h"


#define MAX_CLIENTS_ 2

#define SERVER_PORT 13001
#define SERVER_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 173


char * CONTACTS_PATH = "/home/tommy/ProgettoSO/Server/files/contatti.txt";
char * USERS_PATH = "/home/tommy/ProgettoSO/Server/files/utenti.txt";
char * LOG_PATH = "/home/tommy/ProgettoSO/Server/files/log.txt";




//OUTCOME CODES

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



//OPER. NUMS

#define LISTING '1'
#define INSERT '2'
#define EDIT '3'
#define DELETE '4'
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


//TOKEN TYPE DEF.
typedef char * TOKEN;
#define TOKEN_LENGTH_ 32



/* struct UserSession{
    struct sockaddr_in userAddr;
    char token [32];
}; */


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


struct Contatto{
    char name[20];
    char cognome[20];
    char numTel[20];
};





int socketSetUp(int option);
struct sockaddr_in *binding(int socket, int port);

Message * deconstruct_Message_String(char * msg);
int execute_operation(Message *data);
int numberOfContacts(FILE * contatti);

int search_And_Set_UserIndex(FILE * contatti, Message * data);
int search_And_Set_ContactIndex(FILE * contatti, Message * data);

int rewriteAddressBook(Message * data);


char * readContacts();
int insertContact(Message * data);
int editContact(Message * data);
int deleteContact(Message * data);

int login(Message * data);
int checkLoginSession(TOKEN token);
void gen_token(TOKEN token, size_t length);
unsigned char * convertToSHA256(char * str, unsigned char * output);
void to_hex(const unsigned char *hash, char *output, size_t length);

char * getCurrentTimeStr();
int logAnEvent(int eventCode, Message * data, char * address, char * time);





