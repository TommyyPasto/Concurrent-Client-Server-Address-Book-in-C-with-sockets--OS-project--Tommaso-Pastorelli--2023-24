#include "Server.h"


int main() {
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
    if (listen(server_fd, 3) < 0) {
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

    // Legge i dati inviati dal client
    int valread = read(new_socket, buffer, BUFFER_SIZE);

    //restituisce i dati formattati sotto forma di struct Message
    Message * data = deconstruct_Message_String(buffer);
    
    if(data->operazione == VISUALIZZAZIONE){
        FILE * contatti = fopen("contatti.txt","a+");
        int nContatti = numeroDiContatti(contatti);
        fclose(contatti);
        char * lista = listaContatti();
        int numConvertito = htonl(nContatti);
        write(new_socket, numConvertito, sizeof(numConvertito));
        
    }else{
        execute_operation(data);
    }

    // Chiudi la connessione
    close(new_socket);
    close(server_fd);

    return 0;
}




Message * deconstruct_Message_String(char * msg){
    Message * data  = malloc(sizeof (Message));
    data->operazione = msg[0];
    char * mesg;
    mesg = &msg[POS_NOME];
    strncpy(data->nome, mesg, 20);
    printf("%s", data->nome);

    mesg = &msg[POS_COGNOME];
    strncpy(data->cognome, mesg, 20);
    printf("%s", data->cognome);

    mesg = &msg[POS_NUM_TELEFONO];
    strncpy(data->numTelefono, mesg, 10);
    printf("%s", data->numTelefono);
      
    mesg = &msg[POS_NEW_NOME];
    strncpy(data->new_nome, mesg, 20);
    printf("%s", data->new_nome);

    mesg = &msg[POS_NEW_COGNOME];
    strncpy(data->new_cognome, mesg, 20);
    printf("%s", data->new_cognome);
    
    mesg = &msg[POS_NEW_NUM_TELEFONO];
    strncpy(data->new_numTelefono, mesg, 10);
    printf("%s", data->new_numTelefono);

    mesg = &msg[POS_USERNAME];
    strncpy(data->username, mesg, 20);
    printf("%s", data->username);

    mesg = &msg[POS_PSW];
    strncpy(data->psw, mesg, 20);
    printf("%s", data->psw);
    
    return data;
}



int execute_operation(Message * data){

    
    if(data->operazione == VISUALIZZAZIONE){
        listaContatti();
    }
    else if(data->operazione == INSERIMENTO){
        inserisciContatto(data);
    }
    else if(data->operazione == MODIFICA){
        modificaContatto(data);
    }
    else if(data->operazione == CANCELLAZIONE){
        cancellaContatto(data);
    }
    else if(data->operazione == LOGIN){
        //login(data)
    }
    else if(data->operazione == LOGOUT){
        //logout()
    }
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
    int nCharWritten = fprintf(contatti, "%s %s %s\n", data->nome, data->cognome, data->numTelefono);
    //controllo che il contatto non esista già
    if(ricercaContatto(contatti, data) == -1){
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
    char * buffer;
    int esito;
    contatti = fopen("contatti.txt", "a+");

    //la funzione ricercaContatto restituisce la posizione di fseek in cui è posizionato il file in caso servisse per qualche utilizzo
    if(ricercaContatto(contatti, data) != -1){
        int nCharWritten = fprintf(contatti, "%s %s %s\n", data->new_nome, data->new_cognome, data->new_numTelefono);
        // controllo che la lunghezza corrisponda al contenuto mandato dal client
        if((strlen(data->nome)+strlen(data->cognome)+strlen(data->numTelefono)+3) != nCharWritten || nCharWritten == -1){
            printf("errore nella fase di scrittura");
            esito = ERROR_OCCURED;
        }else{
            printf("operazione avvenuta correttamente");
            esito = POSITIVE;
        }
    }else{
        esito = CONTACT_NOT_FOUND;
    }
    fclose(contatti);
    return esito;
}

int cancellaContatto(Message * data){
    FILE * contatti;
    char buffer[53];
    int esito;
    contatti = fopen("contatti.txt", "r+");
    
    //la funzione ricercaContatto restituisce la posizione di fseek in cui è posizionato il file in caso servisse per qualche utilizzo
    if(ricercaContatto(contatti, data) != -1){
        fseek(contatti, 0, SEEK_SET);
        FILE * copiaContatti;
        copiaContatti = fopen("temp.txt", "a+");
        while(fgets(buffer, sizeof(buffer), contatti) != NULL){
            char * nome, * cognome, * numTelefono;
            nome = strtok(buffer, " ");
            cognome = strtok(NULL, " ");
            numTelefono = strtok(NULL, "\n");
            printf("%s", nome);
            if(strcmp(nome, data->nome) != 0 && strcmp(cognome, data->cognome) != 0 && strcmp(numTelefono, data->numTelefono) != 0){
                fprintf(copiaContatti, "%s %s %s\n", nome, cognome, numTelefono);
                esito = POSITIVE;
            }   
        } 
        remove("contatti.txt");
        rename("temp.txt","contatti.txt");
    }else{
        esito = CONTACT_NOT_FOUND;
    }
    return esito;
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