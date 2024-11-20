
#include "Client.h"



/* //restituisce 0 se il login è andato a buon fine, altrimenti restituisce -1
int login(int client_Socket, char * username, char * psw){
    /*if(strlen(username)>20 || strlen(psw)>20){
        printf(MAX_PMTR_LEN_ERROR "\n");
        return -1;
    }*//*
    Message dati_login;
    strcpy(dati_login.username, username);
    strcpy(dati_login.psw, psw);
    return 0;
} */


void clear_last_n_lines(int n) {
    for (int i = 0; i < n; i++) {
        // Muove il cursore di una riga verso l'alto e cancella la riga
        printf("\033[A");
    }
    printf("\033[J");
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

    /* Message msg;
    msg.operazione = '1';
    msg.esito = '1';
    msg.loggato = '1';
    strcpy(msg.nome, "tommy");
    strcpy(msg.cognome, "pastorelli");
    strcpy(msg.numTelefono, "3392082832");
    strcpy(msg.new_nome, "mhanz");
    strcpy(msg.new_cognome, "pollo");
    strcpy(msg.new_numTelefono, "3389291921");
    strcpy(msg.username, "Mhanz");
    strcpy(msg.psw, "mhanzino"); */

    //DEFINISCO msg per contenere i dati inseriti dall'utente
    Message * msg;
    msg = malloc(sizeof(Message));

    //DEFINISCO message per copiarci dentro i dati di "msg" e poi inviarlo tramite socket al server
    char message [BUFFER_SIZE];

    //FINCHE IL SOCKET E' APERTO CONTINUO A FAR GIRARE IL PROGRAMMA
    while(1){//sock != -1){
        msg = choose_operation();

        if(msg->operazione == 1){
            create_Message_String(message, msg);
        
            // Invia il messaggio al server
            write(sock, message, BUFFER_SIZE);
            printf("Messaggio inviato al server\n");

        }else{
            
            create_Message_String(message, msg);
            
            // Invia il messaggio al server
            write(sock, message, BUFFER_SIZE);
            printf("Messaggio inviato al server\n"); 

        }

    }
    //free(msg);
    // Chiudi il socket(FORSE VA TOLTO)
    //close(sock);

    return 0;
}



Message * choose_operation(){
    int sceltaNelRangeCorretto;
    int datiInseritiCorrettamente = 1;
    char scelta;
    char * stringalogged;

    //SISTEMARE IL LOGIN
    logged = 1;
    char * colore;
    do{
        sceltaNelRangeCorretto = 1;
        if(logged==1){
            stringalogged = logged_string;
            colore = GRN;
        }else{
            stringalogged = notLogged_string;
            colore = RED;
        }
        printf(YEL "***********************\n");
        printf("***SCELTA OPERAZIONE***\n");
        printf("***********************\n\n" RESET);
        printf("1)Visualizza contatti\n 2)Aggiungi contatto %s%s" RESET "\n 3)Modifica contatto %s%s" RESET "\n 4)Rimuovi contatto %s%s" RESET, colore,stringalogged, colore,stringalogged, colore,stringalogged);
        if(logged){   
            printf(RED "\n5)LOGOUT" RESET);
        }else{
            printf(YEL "\n5)LOGIN" RESET);
        }

        //SCELTA DELL'OPERAZIONE
        printf("\n\nScegli operazione: ");
        scanf("%s", &scelta);
        
        if(logged){
            Message * msg;
            msg = malloc(sizeof(Message));
            sceltaNelRangeCorretto = 1;
            datiInseritiCorrettamente = 1;
            switch(scelta){
                case VISUALIZZAZIONE:
                    msg->operazione = VISUALIZZAZIONE;
                    return msg;

                case INSERIMENTO: 
                case CANCELLAZIONE:
                case MODIFICA: 
                    msg->operazione = scelta;
                    /* msg.esito = '1';
                    msg.loggato = '1'; */
                    printf("Nome: ");
                    scanf("%s", msg->nome);
                    if(strlen(msg->nome) > 20){
                        printf(RED "*Il nome deve essere di massimo 20 caratteri*\n" RESET);
                        if(checkSpecialChar(msg->nome) == -1){
                            printf(RED "*Niente caratteri speciali*\n" RESET);
                        }
                        break;
                    }
                    if(checkSpecialChar(msg->nome) == -1){
                        printf(RED "*Niente caratteri speciali*\n" RESET);
                        break;
                    }
                    printf("Cognome: ");
                    scanf("%s", msg->cognome);
                    if(strlen(msg->cognome) > 20){
                        printf(RED "*Il cognome deve essere di massimo 20 caratteri*\n" RESET);
                        if(checkSpecialChar(msg->cognome) == -1){
                            printf(RED "*Niente caratteri speciali*\n" RESET);
                        }
                        break;
                    }
                    if(checkSpecialChar(msg->cognome) == -1){
                        printf(RED "*Niente caratteri speciali*\n" RESET);
                        break;
                    }
                    printf("Num. di telefono: ");
                    scanf("%s", msg->numTelefono);
                    if(strlen(msg->numTelefono) > 10){
                        printf(RED "*Il num. di telefono deve essere di massimo 10 caratteri*\n" RESET);
                        if(checkNumber(msg->numTelefono) == -1){
                            printf(RED "*SOLO NUMERI AMMESSI*\n" RESET);
                        }
                        break;
                    }
                    if(checkNumber(msg->numTelefono) == -1){
                        printf(RED "*SOLO NUMERI AMMESSI*\n" RESET);
                        break;
                    }

                    if(scelta == MODIFICA){
                        printf("Nuovo nome: ");
                        scanf("%s", msg->new_nome);
                        if(strlen(msg->new_nome) > 20){
                            printf(RED "*Il nuovo nome deve essere di massimo 20 caratteri*\n" RESET);
                            if(checkSpecialChar(msg->new_nome) == -1){
                                printf(RED "*Niente caratteri speciali*\n" RESET);
                                break;
                            }
                            break;
                        }
                        if(checkSpecialChar(msg->new_nome) == -1){
                            printf(RED "*Niente caratteri speciali*\n" RESET);
                            break;
                        }
                        printf("Nuovo cognome: ");
                        scanf("%s", msg->new_cognome);
                        if(strlen(msg->new_cognome) > 20){
                            printf(RED "*Il nuovo cognome deve essere di massimo 20 caratteri*\n" RESET);
                            if(checkSpecialChar(msg->new_cognome) == -1){
                                printf(RED "*Niente caratteri speciali*\n" RESET);
                                break;
                            }
                            break;
                        }
                        if(checkSpecialChar(msg->new_cognome) == -1){
                            printf(RED "*Niente caratteri speciali*\n" RESET);
                            break;
                        }
                        printf("Nuovo telefono: ");
                        scanf("%s", msg->new_numTelefono);
                        if(strlen(msg->new_numTelefono) > 20){
                            printf(RED "*Il nuovo numero di telefono deve essere di massimo 10 caratteri*\n" RESET);
                            if(checkNumber(msg->new_numTelefono) == -1){
                                printf(RED "*SOLO NUMERI AMMESSI*\n" RESET);
                            }
                            break;
                        }
                        if(checkNumber(msg->new_numTelefono) == -1){
                            printf(RED "*SOLO NUMERI AMMESSI*\n" RESET);
                            break;
                        }
                    }
                    return msg;
                

                case LOGIN: 
                    msg->operazione = LOGIN;
                    /* msg.esito = '1';
                    msg.loggato = '1'; */
                    printf("Username: ");
                    scanf("%s", msg->username);
                    if(strlen(msg->username) > 20){
                        printf(RED "*L'username deve essere di massimo 20 caratteri*\n" RESET);
                        break;
                    }
                    printf("Password: ");
                    scanf("%s", msg->psw);
                    if(strlen(msg->psw) > 20){
                        printf(RED "*la password deve essere di massimo 20 caratteri*\n" RESET);
                        break;
                    }
                    if(strchr(msg->psw, " ")){
                        printf(RED "*Niente spazi nella password*\n" RESET);
                        break;
                    }
                    return msg;

                case LOGOUT: 
                    //CHIEDI AL SERVER DI CHIUDERE IL FILE SOCKET CORRISPONDENTE E CHIUDI IL TUO LATO CLIENT
                    return msg;
                    
                case ESCI: 
                    //SE SEI LOGGATO FAI LOGOUT E POI CHIUDI, ALTRIMENTI CHIUDI E BASTA
                    return msg;
                    
                default: 
                    sceltaNelRangeCorretto = 0;
                    printf("\033c");
                    printf(RED "\n*************************************************************** \n");
                    printf("**  IL VALORE INSERITO NON E' CORRETTO, PERFAVORE RIPROVARE! ** \n");
                    printf("*************************************************************** \n");
                    printf("\n");
                    for(int i = 0; i < 3; i++)
                        printf("                       ||\n");
                    printf("                       \\/\n\n" RESET);
                    break;
            }
        }else{
            if(scelta == VISUALIZZAZIONE){
                Message * msg;
                msg = malloc(sizeof(Message));
                msg->operazione = VISUALIZZAZIONE; 
                return msg;

            }else{
                printf("Perfavore effettuare il LOGIN prima di eseguire questa operazione!");
            }
        }
        
    }while(sceltaNelRangeCorretto == 0 || datiInseritiCorrettamente == 0); 
}

/*Funzione per creare il buffer(messaggio) da inviare al server con tutti i dati necessari riguardanti le varie operazioni*/
void create_Message_String(char messaggio[], Message * data){
    char * msg = &messaggio[0];
    msg[0] = data->operazione;
    msg = &messaggio[POS_NOME];
    strcpy(msg, data->nome);
    msg = &messaggio[POS_COGNOME];
    strcpy(msg, data->cognome);
    msg = &messaggio[POS_NUM_TELEFONO];
    strcpy(msg, data->numTelefono);
      
    msg = &messaggio[POS_NEW_NOME];
    strcpy(msg, data->new_nome);
    msg = &messaggio[POS_NEW_COGNOME];
    strcpy(msg, data->new_cognome);
    msg = &messaggio[POS_NEW_NUM_TELEFONO];
    strcpy(msg, data->new_numTelefono);

    msg = &messaggio[POS_USERNAME];
    strcpy(msg, data->username);
    msg = &messaggio[POS_PSW];
    strcpy(msg, data->psw);
}





int checkSpecialChar(char * string){
    int length = strlen(string);
    for(int i = 0; i < length; i++){
        char ch = string[i];
        if(ch < 65 || (ch > 90 && ch < 97) || ch > 122){
            return -1;
        }
    }
    return 0;
}


int checkNumber(char * string){
    int length = strlen(string);
    for(int i = 0; i < length; i++){
        char ch = string[i];
        if(ch < 48 || ch > 57){
            return -1;
        }
    }
    return 0;
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