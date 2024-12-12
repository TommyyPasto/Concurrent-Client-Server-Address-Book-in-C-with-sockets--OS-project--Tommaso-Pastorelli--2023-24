#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include </home/tommy/ProgettoSO/Utils/sha256.h>
#include <time.h>
#include <signal.h>

#define MAX_USERS_ 10

#define SERVER_PORT 13000
#define SERVER_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 173


char * CONTACTS_PATH = "/home/tommy/ProgettoSO/Server/contatti.txt";
char * USERS_PATH = "/home/tommy/ProgettoSO/Server/utenti.txt";


//VALORE DI ESITO:
#define POSITIVE 0
#define ERROR_OCCURED -1
#define ALR_EXISTING_CONTACT 1
#define CONTACT_NOT_FOUND 2
#define ZERO_CONTACTS_SAVED 3
#define TRYING_ILLEGAL_ACCESS 4
#define PASSWORD_NOT_CORRECT 5
#define USER_NOT_FOUND 6
#define TOO_MANY_CLIENTS_CONNECTED 7


//VALORI NUMERICI DELLE OPERAZIONI
#define VISUALIZZAZIONE '1'
#define INSERIMENTO '2'
#define MODIFICA '3'
#define CANCELLAZIONE '4'
#define LOGIN '5'
#define LOGOUT '6'
#define ESCI '0'

//POSIZIONI NELL'ARRAY DEL MESSAGGIO
#define POS_OPERAZIONE 0
#define POS_NOME 1
#define POS_COGNOME 21
#define POS_NUM_TELEFONO 41
#define POS_NEW_NOME 51
#define POS_NEW_COGNOME 71
#define POS_NEW_NUM_TELEFONO 91
#define POS_USERNAME 101
#define POS_PSW 121
#define POS_TOKEN 141

#define TOKEN_LENGTH_ 32

typedef char * TOKEN;

/* struct UserSession{
    struct sockaddr_in userAddr;
    char token [32];
}; */


typedef struct{
    char operazione;
    char nome[20];
    char cognome[20];
    char numTelefono[10];
    char new_nome[20];
    char new_cognome[20];
    char new_numTelefono[10];
    char username[20];
    char psw[20];
    char token[32];
}Message;

struct Contatto{
    char nome[20];
    char cognome[20];
    char numTel[20];
};

Message * deconstruct_Message_String(char * msg);
int execute_operation(Message * data);
int numberOfContacts(FILE * contatti);

int search_And_Set_UserIndex(FILE * contatti, Message * data);
int search_And_Set_ContactIndex(FILE * contatti, Message * data);

int rewriteAddressBook(FILE * contatti, Message * data);


char * readContacts();
int insertContact(Message * data);
int editContact(Message * data);
int cancellaContatto(Message * data);

int login(Message * data);
int checkLoginSession(TOKEN token);
void gen_token(TOKEN token, size_t length);
unsigned char * convertToSHA256(char * str, unsigned char * output);
void to_hex(const unsigned char *hash, char *output, size_t length);





