
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
        exit(-1);
    }

    // Definizione delle informazioni del server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // Connessione al server
    
    while(1){
        if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            char riprovare [10];
            //printf(RED"***************************************************************************\n");
            printf(RED"-------> "CONNECTION_ERROR":  " RESET);
            //printf("********* ******************************************************************\n"RESET);
            scanf("%s",riprovare);
            if(strcmp(riprovare, "Y") == 0 ){
                printf(YEL"                  ritentando la connessione...\n\n"RESET);
            }else{
                printf(RED"\n\n                  *PROGRAMMA TERMINATO*\n\n"RESET);
                exit(-1);
            }
        }else{
            return sock;
        }
    }
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

        if(msg->operazione == VISUALIZZAZIONE){
            create_Message_String(message, msg);
        
            // Invia il messaggio al server
            write(sock, message, BUFFER_SIZE);
            printf("Messaggio inviato al server\n");

            //ricevo il numero di contatti inviati
            int32_t numElementi;
            int valread = read(sock, &numElementi, sizeof(numElementi));
            int numContatti = ntohl(numElementi);
            
            int size = sizeof(char) * numContatti * 53;
            char buffer[size];
            valread = read(sock, buffer, size);
            stampaContatti(buffer, numContatti);
            sleep(2);
            
            /****************************************************
            ARRIVATO QUI!!!!!!!!
            ******************************************/

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


void printMenu(int logged){
    if(logged == 1){
        //system("clear || cls"); // Pulisce lo schermo (cross-platform)
    
        printf("*************************************************\n");
        printf("*                                               *\n");
        printf("*"BLU"          BENVENUTO NELLA RUBRICA" RESET "              *\n" );
        printf("*                                               *\n");
        printf("*************************************************\n");
        printf("\n");
        printf(GRN"  Scegli un'operazione:"RESET"\n");
        printf("  ---------------------------------------------\n");
        printf(YEL"  [1]"RESET" Visualizza tutti i contatti\n");
        printf(YEL"  [2]"RESET" Aggiungi un nuovo contatto\n");
        printf(YEL"  [3]"RESET" Modifica un contatto esistente\n");
        printf(YEL"  [4]"RESET" Elimina un contatto\n");
        printf(YEL"  [5]"RESET" LOGOUT\n");
        printf(YEL"  [0]"RESET" Esci\n");
        printf("  ---------------------------------------------\n");
        printf("\n");
    }else{
        //system("clear || cls"); // Pulisce lo schermo (cross-platform)
    
        printf("*************************************************\n");
        printf("*                                               *\n");
        printf("*"BLU"          BENVENUTO NELLA RUBRICA" RESET "              *\n" );
        printf("*                                               *\n");
        printf("*************************************************\n");
        printf("\n");
        printf(GRN"  Scegli un'operazione:"RESET"\n");
        printf("  ---------------------------------------------\n");
        printf(YEL"  [1]"RESET" Visualizza tutti i contatti\n");
        printf(RED"  [🔒] PER VEDERE LE ALTRE OPERAZIONI DEVI EFFETTUARE IL LOGIN(5)\n");
        printf("\n");
    }
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

        //STAMPO IL MENU DA TERMINALE
        printMenu(logged);

        //SCELTA DELL'OPERAZIONE
        printf(CYN"  Inserisci la tua scelta: " RESET);
        scanf("%s", &scelta);
        printf("\n");

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
                    printf(BLU"• Nome: "RESET);
                    scanf("%s", msg->nome);
                    if(strlen(msg->nome) > 20){
                        printf(RED "*ERRORE: Il nome deve essere di massimo 20 caratteri*\n" RESET);
                        if(checkSpecialChar(msg->nome) == -1){
                            printf(RED "*ERRORE: Niente caratteri speciali*\n" RESET);
                        }
                        break;
                    }
                    if(checkSpecialChar(msg->nome) == -1){
                        printf(RED "*ERRORE: Niente caratteri speciali*\n" RESET);
                        break;
                    }
                    printf(BLU"• Cognome: "RESET);
                    scanf("%s", msg->cognome);
                    if(strlen(msg->cognome) > 20){
                        printf(RED "*ERRORE: Il cognome deve essere di massimo 20 caratteri*\n" RESET);
                        if(checkSpecialChar(msg->cognome) == -1){
                            printf(RED "*ERRORE: Niente caratteri speciali*\n" RESET);
                        }
                        break;
                    }
                    if(checkSpecialChar(msg->cognome) == -1){
                        printf(RED "*ERRORE: Niente caratteri speciali*\n" RESET);
                        break;
                    }
                    printf(BLU"• Num. di telefono: "RESET);
                    scanf("%s", msg->numTelefono);
                    if(strlen(msg->numTelefono) > 10){
                        printf(RED "*ERRORE: Il num. di telefono deve essere di massimo 10 caratteri*\n" RESET);
                        if(checkNumber(msg->numTelefono) == -1){
                            printf(RED "*ERRORE: SOLO NUMERI AMMESSI*\n" RESET);
                        }
                        break;
                    }
                    if(checkNumber(msg->numTelefono) == -1){
                        printf(RED "*ERRORE: SOLO NUMERI AMMESSI*\n" RESET);
                        break;
                    }

                    if(scelta == MODIFICA){
                        printf(GRN"• Nuovo nome: "RESET);
                        scanf("%s", msg->new_nome);
                        if(strlen(msg->new_nome) > 20){
                            printf(RED "*ERRORE: Il nuovo nome deve essere di massimo 20 caratteri*\n" RESET);
                            if(checkSpecialChar(msg->new_nome) == -1){
                                printf(RED "*ERRORE: Niente caratteri speciali*\n" RESET);
                                break;
                            }
                            break;
                        }
                        if(checkSpecialChar(msg->new_nome) == -1){
                            printf(RED "*ERRORE: Niente caratteri speciali*\n" RESET);
                            break;
                        }
                        printf(GRN"• Nuovo cognome: "RESET);
                        scanf("%s", msg->new_cognome);
                        if(strlen(msg->new_cognome) > 20){
                            printf(RED "*ERRORE:Il nuovo cognome deve essere di massimo 20 caratteri*\n" RESET);
                            if(checkSpecialChar(msg->new_cognome) == -1){
                                printf(RED "*ERRORE: Niente caratteri speciali*\n" RESET);
                                break;
                            }
                            break;
                        }
                        if(checkSpecialChar(msg->new_cognome) == -1){
                            printf(RED "*ERRORE: Niente caratteri speciali*\n" RESET);
                            break;
                        }
                        printf(GRN"• Nuovo n. di telefono: "RESET);
                        scanf("%s", msg->new_numTelefono);
                        if(strlen(msg->new_numTelefono) > 20){
                            printf(RED "*ERRORE: Il nuovo numero di telefono deve essere di massimo 10 caratteri*\n" RESET);
                            if(checkNumber(msg->new_numTelefono) == -1){
                                printf(RED "*ERRORE: SOLO NUMERI AMMESSI*\n" RESET);
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
                    printf(MAG"• Username: "RESET);
                    scanf("%s", msg->username);
                    if(strlen(msg->username) > 20){
                        printf(RED "*ERRORE: L'username deve essere di massimo 20 caratteri*\n" RESET);
                        break;
                    }
                    printf(MAG"• Password: "RESET);
                    scanf("%s", msg->psw);
                    if(strlen(msg->psw) > 20){
                        printf(RED "*ERRORE: la password deve essere di massimo 20 caratteri*\n" RESET);
                        break;
                    }
                    if(strchr(msg->psw, ' ')){
                        printf(RED "*ERRORE: Niente spazi nella password*\n" RESET);
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
                printf(RED"                  |\n"RESET);
                printf(RED"                  |   (!!!)\n"RESET);
                printf(RED"                  V\n"RESET);
                printf(RED"\n[!] DEVI PRIMA EFFETTUARE IL LOGIN\n\n"RESET);
                sleep(2);
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
        if(ch < 48 || (ch > 57 && ch < 65) || (ch > 90 && ch < 97) || ch > 122){
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


void stampaContatti(char * listaContatti, int numContatti){
    int i = 1;
    printf(BLU"\n\n[LISTA CONTATTI]\n"RESET);
    do{
        char * lista;
        lista = &listaContatti[(i-1)*53];
        char * nome, * cognome, * numTelefono;
        char contatto[53];
        strncpy(contatto, lista, 53);
        nome = strtok(contatto, " ");
        cognome = strtok(NULL, " ");
        numTelefono = strtok(NULL, "\n");
        printf(YEL"•"RESET" %s, %s, %s\n", nome, cognome, numTelefono);
        i++;

    }while(i <= numContatti);
    printf("\n\n");
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