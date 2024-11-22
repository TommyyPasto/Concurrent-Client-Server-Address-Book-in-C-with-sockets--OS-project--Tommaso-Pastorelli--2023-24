#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "/home/tommy/ProgettoSO/Utils/ClientUtils.h"

//DATI BASE
#define SERVER_PORT 13000
#define SERVER_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 141

//VALORE DI ESITO:
#define POSITIVE 0
#define ERROR_OCCURED -1
#define ALR_EXISTING_CONTACT 1
#define CONTACT_NOT_FOUND 2
#define ZERO_CONTACTS_SAVED 3


//NOMI STRINGHE ERRORI
#define MAX_PMTR_LEN_ERROR "La lunghezza di username o password deve essere INFERIORE a 20 caratteri"
#define SOCKET_CREATION_ERROR "Si è verificato un ERRORE nella creazione del socket"
#define CONNECTION_ERROR "CONNESSIONE FALLITA, RIPROVARE? [Y][N or other]"
#define ALR_EXISTING_CONTACT_ERROR "Il contatto è già presente"
#define ALR_EXISTING_CONTACT_ERROR "Il contatto è già presente"


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



typedef struct{
    char operazione;
    char nome[20];
    char cognome[20];
    char numTelefono[10];
    char new_nome[20];
    char new_cognome[20];
    char new_numTelefono[10];
    char loggato;
   
    char username[20];
    char psw[20];
}Message;


int logged = 0;
char logged_string[] = "[✔ Loggato]";
char notLogged_string[] = "[X NON Loggato!]";

int connect_To_Server();
Message * choose_operation();
//int send_Message();
//int login(int client_Socket, char * username, char * psw);
void create_Message_String(char messaggio[], Message * data);

int checkSpecialChar(char * string);
int checkNumber(char * string);

