#include "Server.h"






TOKEN * sessionTokens;
int lastSessionToken = 0;

int main() {
    sessionTokens = malloc(MAX_USERS_ * sizeof(TOKEN));
    for(int i = 0; i < MAX_USERS_; i++){
        sessionTokens[i] = malloc(TOKEN_LENGTH_ * sizeof(char) +1);
    } 

    int opt = 1;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creazione del socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //se la socket di SO non si è ancora chiusa la riusiamo
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("Errore in setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Assegnazione dell'indirizzo e della porta al socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    // Binding del socket al porto definito
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // In ascolto di connessioni
    if (listen(server_fd, MAX_USERS_) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto sulla porta %d\n", SERVER_PORT);

    // Accetta la connessione in entrata
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

     


    while(1){
        // Legge i dati inviati dal client
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if(valread == -1){
            // Chiudi la connessione
            close(new_socket);
            close(server_fd);
            perror("client disconnesso");
            exit(-1);
        }

        //restituisce i dati formattati sotto forma di struct Message
        Message * data = deconstruct_Message_String(buffer);
        
        // se l'operazione è quella di visualizzazione allora gestisco l'op in modo diverso 
        if(data->operazione == VISUALIZZAZIONE){
            FILE * contatti = fopen("contatti.txt","a+");
            int nContatti = numeroDiContatti(contatti);
            fclose(contatti);
            char * lista = listaContatti();
            int32_t numConvertito = htonl(nContatti);
            write(new_socket, &numConvertito, sizeof(numConvertito));
            write(new_socket, lista, (sizeof(char) * nContatti * 53));

        }else{   
            if(data->operazione != LOGIN){
                if(checkLoginSession(data->token) < 0){
                    int esito = TRYING_ILLEGAL_ACCESS;
                    int32_t esitoConv = htonl(esito);
                    write(new_socket, &esitoConv, sizeof(esitoConv));
                }else{
                    int esito = execute_operation(data);
                    int32_t esitoConv = htonl(esito);
                    int val = write(new_socket, &esitoConv, sizeof(esitoConv));
                }
            }else{
                int esito = execute_operation(data);
                char * buffer = malloc(33 * sizeof(char));
                buffer[0] = esito;
                if(esito == POSITIVE)
                    strncpy((char *)&(buffer[1]), (const char *)sessionTokens[lastSessionToken-1], strlen(sessionTokens[lastSessionToken-1])); 
                write(new_socket, buffer, 33 * sizeof(char));
            }
        }
    }
    // Chiudi la connessione
    close(new_socket);
    close(server_fd);
    return 0;
}





int execute_operation(Message * data){
    if(data->operazione == INSERIMENTO){
        return inserisciContatto(data);
    }
    else if(data->operazione == MODIFICA){
        return modificaContatto(data);
    }
    else if(data->operazione == CANCELLAZIONE){
        return cancellaContatto(data);
    }
    else if(data->operazione == LOGIN){
       
        return login(data);
    }
    else if(data->operazione == LOGOUT){
        //logout()
    }
    return 0;
}


char * listaContatti(){
    FILE * contatti;
    contatti = fopen("contatti.txt", "a+");
    int numContatti = numeroDiContatti(contatti);
    char * listaContatti = malloc(numContatti * 53 * sizeof(char));
    int i = 0;
    char buffer[53];
    fseek(contatti, 0, SEEK_SET);

    while(fgets(buffer, sizeof(buffer), contatti)){
        char * str = &listaContatti[i * 53];
        strncpy(str, buffer, 53);
        printf("%s", str);
        i++;
    }
    return listaContatti;
}


int inserisciContatto(Message * data){
    
    FILE * contatti;
    contatti = fopen("contatti.txt", "a+");
    int esito;

    int esitoRicerca = ricercaContatto(contatti, data);
    printf("esito ricerca: %d", esitoRicerca);
    //controllo che il contatto non esista già
    if(esitoRicerca < 0){

        int nCharWritten = fprintf(contatti, "%s %s %s\n", data->nome, data->cognome, data->numTelefono);
        
        // controllo che la lunghezza corrisponda al contenuto mandato dal client
        if((strlen(data->nome)+strlen(data->cognome)+strlen(data->numTelefono)+3) != nCharWritten || nCharWritten == -1){
            printf("errore nella fase di scrittura");
            esito = ERROR_OCCURED;
        }else{
            printf("operazione avvenuta correttamente");
            esito = POSITIVE;
        }
    }else{
        esito = ALR_EXISTING_CONTACT;
    }
    fclose(contatti);
    return esito;
}




int modificaContatto(Message * data){
    FILE * contatti;
    char buffer[53];
    int esito;
    contatti = fopen("contatti.txt", "r+");
    if(contatti == NULL){
        contatti = fopen("contatti.txt", "a+");
        fclose(contatti);
    }
    //ORA CONTROLLO PRIMA SE IL CONTATTO CHE VOGLIO INSERIRE(MODIFICANDO) E' GIA ESISTENTE
    char nomeTmp[20], cognomeTmp[20], numTmp[10];
    strcpy(nomeTmp, data->nome);
    strcpy(cognomeTmp, data->cognome);
    strcpy(numTmp, data->numTelefono);
    strcpy(data->nome, data->new_nome);
    strcpy(data->cognome, data->new_cognome);
    strcpy(data->numTelefono, data->new_numTelefono);
    if(ricercaContatto(contatti, data) != -1){
        esito = ALR_EXISTING_CONTACT;
        printf("il contatto esiste già");
        return esito;
    }else{
        strcpy(data->nome, nomeTmp);
        strcpy(data->cognome, cognomeTmp);
        strcpy(data->numTelefono, numTmp);
    }
    return riscriviRubrica(contatti, data);
}







int cancellaContatto(Message * data){
    FILE * contatti;
    //int esito;
    contatti = fopen("contatti.txt", "r+");
    
    int esitoModifica = riscriviRubrica(contatti, data);
    printf("ESITO canc: %d", esitoModifica);
    return esitoModifica;
}







int login(Message * data){
    /*FILE * utenti = fopen("utenti.txt", "r+");
    fclose(utenti);*/
    int esitoLogin;
      
    char buffer[100];
   
    FILE * utenti = fopen("utenti.txt", "r+");
    
    if(ricercaUtente(utenti, data) != -1){
        fseek(utenti, 0, SEEK_SET);
    
        char * nomeUtente;
        char * pswSHA256hex;
        
        fgets(buffer, sizeof(buffer), utenti);
        nomeUtente = strtok(buffer, " ");
        pswSHA256hex = strtok(NULL, "\n");
        
        unsigned char * hash = malloc(SHA256_BLOCK_SIZE * sizeof(unsigned char));
        convertiInSHA256(data->psw, hash);
        
        char * pswConvertita = malloc(1 + sizeof(unsigned char) * SHA256_BLOCK_SIZE * 2);
        to_hex(hash, pswConvertita, SHA256_BLOCK_SIZE);
        
        printf("stringhe: %s | %s\n", pswSHA256hex, pswConvertita);
    
        if(strcmp(pswSHA256hex, pswConvertita) == 0){
            printf("ESITO LOGIN POSITIVO");
            if(lastSessionToken < 9){
                //Inserisco il token di sessione nell'array relativo
                TOKEN str = malloc(32 * sizeof(char));
                gen_token(sessionTokens[lastSessionToken], TOKEN_LENGTH_);
                lastSessionToken++;
                esitoLogin = POSITIVE;
            }else{
                esitoLogin = TOO_MANY_CLIENTS_CONNECTED;
            } 

            
        }else{
            printf("\nESITO LOGIN NEGATIVO1\n\n");
            esitoLogin = PASSWORD_NOT_CORRECT;
        } 
    }else{
        printf("\nESITO LOGIN NEGATIVO2\n\n");
        esitoLogin = USER_NOT_FOUND;
    }
    return esitoLogin;  
}





int checkLoginSession(TOKEN token){
    for(int i = 0; i < lastSessionToken; i++){
        if(strcmp(token, sessionTokens[i]) == 0){
            return 1;
        }
    }
    return -1;
}




void logout(TOKEN token){
    for(int i = 0; i < lastSessionToken; i++){
        if(strcmp(token, sessionTokens[i]) == 0){
            for(int j = i; j < (lastSessionToken-1); j++){
                sessionTokens[j] = sessionTokens[j+1];
            }
        }
    }
    lastSessionToken--;
}




Message * deconstruct_Message_String(char * msg){
    Message * data  = malloc(sizeof (Message));
    data->operazione = msg[0];
    char * mesg;
    mesg = &msg[POS_NOME];
    strncpy(data->nome, mesg, 20);
    //printf("%s", data->nome);

    mesg = &msg[POS_COGNOME];
    strncpy(data->cognome, mesg, 20);
    //printf("%s", data->cognome);

    mesg = &msg[POS_NUM_TELEFONO];
    strncpy(data->numTelefono, mesg, 10);
    //printf("%s", data->numTelefono);
      
    mesg = &msg[POS_NEW_NOME];
    strncpy(data->new_nome, mesg, 20);
    //printf("%s", data->new_nome);

    mesg = &msg[POS_NEW_COGNOME];
    strncpy(data->new_cognome, mesg, 20);
    //printf("%s", data->new_cognome);
    
    mesg = &msg[POS_NEW_NUM_TELEFONO];
    strncpy(data->new_numTelefono, mesg, 10);
    //printf("%s", data->new_numTelefono);

    mesg = &msg[POS_USERNAME];
    strncpy(data->username, mesg, 20);
    //printf("%s", data->username);

    mesg = &msg[POS_PSW];
    strncpy(data->psw, mesg, 20);
    //printf("%s", data->psw);

    mesg = &msg[POS_TOKEN];
    strncpy(data->token, mesg, 32);
    //printf("%s", data->token);
    
    return data;
}




int ricercaUtente(FILE * utenti, Message * data){
    char buffer[54];
    fseek(utenti, 0, SEEK_SET);
    while(fgets(buffer, sizeof(buffer), utenti) != NULL){
        char * nomeUtente;
        nomeUtente = strtok(buffer, " ");
        if(strcmp(nomeUtente, data->username) == 0)
            return fseek(utenti, -(strlen(nomeUtente) + 34), SEEK_CUR);
    }
    return -1;
}




int ricercaContatto(FILE * contatti, Message * data){
    char buffer[53];
    fseek(contatti, 0, SEEK_SET);
    while(fgets(buffer, sizeof(buffer), contatti) != NULL){
        char * nome, * cognome, * numTelefono;
        nome = strtok(buffer, " ");
        cognome = strtok(NULL, " ");
        numTelefono = strtok(NULL, "\n");
        if(strcmp(nome, data->nome) == 0 && strcmp(cognome, data->cognome) == 0 && strcmp(numTelefono, data->numTelefono) == 0)
            return fseek(contatti, -(strlen(nome)+strlen(cognome)+strlen(numTelefono)+3), SEEK_CUR);
    }
    return -1;
}





int numeroDiContatti(FILE * contatti){
    char buffer[53];
    int count = 0;
    fseek(contatti, 0, SEEK_SET);
    for (char c = getc(contatti); c != EOF; c = getc(contatti))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;

    return count;
}


//Funzione utilizzata dalla modifica e dalla cancellazione
int riscriviRubrica(FILE * contatti, Message * data){
    int esito;
    char buffer[53];
    if(data->operazione == CANCELLAZIONE){
        if(ricercaContatto(contatti, data) != -1){
            fseek(contatti, 0, SEEK_SET);
            FILE * copiaContatti;
            copiaContatti = fopen("temp.txt", "a+");
            while(fgets(buffer, sizeof(buffer), contatti) != NULL){
                char * nome, * cognome, * numTelefono;
                nome = strtok(buffer, " ");
                cognome = strtok(NULL, " ");
                numTelefono = strtok(NULL, "\n");

                if(strcmp(nome, data->nome) != 0 && strcmp(cognome, data->cognome) != 0 && strcmp(numTelefono, data->numTelefono) != 0){
                    int nCharWritten = fprintf(copiaContatti, "%s %s %s\n", nome, cognome, numTelefono);
                    if((strlen(nome)+strlen(cognome)+strlen(numTelefono)+3) != nCharWritten || nCharWritten == -1){
                        printf("errore nella fase di scrittura");
                        esito = ERROR_OCCURED;
                    }else{
                        printf("operazione avvenuta correttamente");
                        esito = POSITIVE;
                    }
                }   
            } 
            fclose(contatti);
            remove("contatti.txt");
            fclose(copiaContatti);
            rename("temp.txt","contatti.txt");
            
        }else{
            esito = CONTACT_NOT_FOUND;
        }
        return esito;
    }else if(data->operazione == MODIFICA){

        if(ricercaContatto(contatti, data) != -1){
            fseek(contatti, 0, SEEK_SET);
            FILE * copiaContatti;
            copiaContatti = fopen("temp.txt", "a+");
            while(fgets(buffer, sizeof(buffer), contatti) != NULL){
                char * nome, * cognome, * numTelefono;
                nome = strtok(buffer, " ");
                cognome = strtok(NULL, " ");
                numTelefono = strtok(NULL, "\n");
                //controllo ogni ciclo se questa riga contiene o meno il contatto ricercato e in caso stampo il contatto modificato sul file temporaneo
                if(strcmp(nome, data->nome) == 0 && strcmp(cognome, data->cognome) == 0 && strcmp(numTelefono, data->numTelefono) == 0){
                    int nCharWritten = fprintf(copiaContatti, "%s %s %s\n", data->new_nome, data->new_cognome, data->new_numTelefono);
                    if((strlen(data->new_nome)+strlen(data->new_cognome)+strlen(data->new_numTelefono)+3) != nCharWritten || nCharWritten == -1){
                        printf("errore nella fase di scrittura");
                        esito = ERROR_OCCURED;
                    }else{
                        printf("operazione avvenuta correttamente");
                        esito = POSITIVE;
                    }
                }else{
                    int nCharWritten = fprintf(copiaContatti, "%s %s %s\n", nome, cognome, numTelefono);
                    if((strlen(nome)+strlen(cognome)+strlen(numTelefono)+3) != nCharWritten || nCharWritten == -1){
                        printf("errore nella fase di scrittura");
                        esito = ERROR_OCCURED;
                    }else{
                        printf("operazione avvenuta correttamente");
                        esito = POSITIVE;
                    }
                } 
            } 
            fclose(contatti);
            remove("contatti.txt");
            fclose(copiaContatti);
            rename("temp.txt","contatti.txt");
        
        }else{
            esito = CONTACT_NOT_FOUND;
        }
        return esito;
    }
}


void to_hex(const unsigned char *hash, char *output, size_t length) {
    for (size_t i = 0; i < length; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[length * 2] = '\0';
}





unsigned char * convertiInSHA256(char * str, unsigned char * hash)
{
    //unsigned char * hash = malloc(SHA256_BLOCK_SIZE * sizeof(unsigned char));
    SHA256_CTX sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, str, strlen(str));
    sha256_final(&sha256, hash);
    //output = hash;
    
}
  


void gen_token(TOKEN token, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int index;
	int i = 0;
    srand(time(NULL));
    while (length-- > 0) {
        index = rand() % (sizeof(charset) - 1);
        token[i++]= charset[index];
    }
    token[i] = '\0';
} 