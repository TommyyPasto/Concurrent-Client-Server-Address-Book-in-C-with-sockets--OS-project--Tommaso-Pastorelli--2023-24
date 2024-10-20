
#include "Client.h"



//restituisce 0 se il login è andato a buon fine, altrimenti restituisce -1
int login(int client_Socket, char * username, char * psw){
    /*if(strlen(username)>20 || strlen(psw)>20){
        printf(MAX_PMTR_LEN_ERROR "\n");
        return -1;
    }*/
    Message dati_login;
    strcpy(dati_login.username, username);
    strcpy(dati_login.psw, psw);
    return 0;
}


int connect_To_Server(){
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    // Creazione del socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf(SOCKET_CREATION_ERROR "\n");
        return -1;
    }

    // Definizione delle informazioni del server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // Conversione dell'indirizzo IP in binario
    /*if (inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr) <= 0) {
        printf("\n Indirizzo non valido o non supportato \n");
        return -1;
    }*/

    // Connessione al server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf(CONNECTION_ERROR "n");
        return -1;
    }

    return sock;
}

int main(int argc, char *argv[]) {
    int sock = connect_To_Server();
    choose_Operation();

    char * message = "1";
    

    // Invia il messaggio al server
    send(sock, message, strlen(message), 0);
    printf("Messaggio inviato al server\n");

    // Chiudi il socket
    close(sock);

    return 0;
}



void choose_Operation(){
    int scelta;
    char * stringalogged;
    //SISTEMARE IL LOGIN
    logged = 1;
    char * colore;
    do{
        if(logged==1){
            stringalogged = logged_string;
            colore = GRN;
        }else{
            stringalogged = notLogged_string;
            colore = RED;
        }
        printf(YEL "***********************\n");
        printf("***SCELTA OPERAZIONE***\n***********************\n\n" RESET "1)Visualizza contatti\n 2)Aggiungi contatto %s%s" RESET "\n 3)Modifica contatto %s%s" RESET "\n 4)Rimuovi contatto %s%s" RESET, colore,stringalogged, colore,stringalogged, colore,stringalogged);
        if(logged){   
            printf(RED "\n5)LOGOUT" RESET);
        }else{
            printf(YEL "\n5)LOGIN" RESET);
        }
        printf("\n\nScegli operazione: ");

        scanf("%d", &scelta);
        printf("scelta: %d\n", scelta);
        switch (scelta){
            case 1: break;
            case 2: break;
            case 3: break;
            case 4: break;
            case 5: break;
            default: 
                printf(RED "\n*************************************************************** \n");
                printf("**  IL VALORE INSERITO NON E' CORRETTO, PERFAVORE RIPROVARE! ** \n");
                printf("*************************************************************** \n");
                for(int i = 0; i < 3; i++)
                    printf("                       ||\n");
                printf("                       \\/\n\n" RESET);
                break;
        }
        printf("\033[45A"); // sistemare
        printf("\033[J");
    }while(scelta < 1 || scelta > 5); 


    exit(-1);
}

/*Funzione per creare il buffer(messaggio) da inviare al server con tutti i dati necessari riguardanti le varie operazioni*/
char * create_Message_String(Message data){
    char messaggio [BUFFER_SIZE];
    char * msg = &messaggio[0];
    strcpy(msg, data.operazione);
    msg = &messaggio[POS_NOME];
    strcpy(msg, data.nome);
    msg = &messaggio[POS_COGNOME];
    strcpy(msg, data.cognome);
    msg = &messaggio[POS_NUM_TELEFONO];
    strcpy(msg, data.numTelefono);
      
    msg = &messaggio[POS_NEW_NOME];
    strcpy(msg, data.new_cognome);
    msg = &messaggio[POS_NEW_COGNOME];
    strcpy(msg, data.new_numTelefono);
    msg = &messaggio[POS_NEW_NUM_TELEFONO];
    strcpy(msg, data.new_nome);

    msg = &messaggio[POS_USERNAME];
    strcpy(msg, data.username);
    msg = &messaggio[POS_PSW];
    strcpy(msg, data.psw);



}





/*COSE DA FARE PROSSIMA VOLTA:
    0.VEDERE SE LA CONNESSIONE FUNZIONA ANCORA A MODO
    1.PROVARE A MANDARE MESSAGGI AL SERVER
    2.IMPLEMENTARE QUINDI UNA BASICA RICEZIONE DEI MESSAGGI NEL PROGRAMMA DEL SERVER
    3. A->COMINCIARE A IMPLEMENTARE IL LOGIN NEL MODO CORRETTO
                OPPURE
                    B->PROVARE A INIZIARE A GESTIRE I FILE(DEGLI UTENTI(CON PSWRDS HASHATE), O DEI CONTATTI);
    
                                    ----->{{TEMPO STIMATO}: 3 ORE}<-----
*/