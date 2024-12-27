/**
 * @file Server.c
 *
 * @brief
 * This program implements a simple address book application
 * to manage contacts using the C programming language.
 *
 * @details
 * Project: Address Book in C Language
 * Author: Tommaso Pastorelli
 * Student ID: 7119242
 * Email: tommaso.pastorelli1@edu.unifi.it
 *
 * Date Started: [Inserire Data di Inizio]
 * Date Finished: [Inserire Data di Fine]
 * Version: 1.0.0
 *
 * License: No license
 * Compiler: GCC (GNU Compiler Collection)
 *
 * Compilation Instructions:
 * compile using the make file
 *
 * Notes:
 * - Ensure GCC is installed on your system.
 * - Modify the program as needed to meet additional requirements.
 */


#include "Server.h"


/**
 * @brief Global pointer to the current session token.  This token is used
 * to authenticate client requests after login.  It should be initialized during the login process.
 */
TOKEN * sessionToken;


/**
 * @brief Sockets for Server side and new clients
 */
int server_socket, client_socket;


/*
* @brief address of the current served client
*/
char client_ip[20]; 


/**
 * @brief Global counter for the number of currently connected clients.
 */
int connectedClients = 0;


/**
 * @brief pid used for logging
 */
int pid;


int socketSetUp(int option){
    int server_socket;
    
    // Creation of socket's fd
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    //using option to indicate an optional value for socket options in "setsocketopt"
    //if SO socket is not closed we reuse it
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("Errore in setsockopt");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    return server_socket;
}



struct sockaddr_in * binding(int socket, int port){
    struct sockaddr_in * address;
    address = malloc(sizeof(struct sockaddr_in));

    // Assigning address and port
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);

    // binding...
    if (bind(socket, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("bind failed");
        close(socket);
        exit(EXIT_FAILURE);
    }

    return address;
}



void sigintHandler(int signum) {
    close(server_socket);
    close(client_socket);
    exit(0);
}



void sigchldHandler(int signum) {
    printf("\n\nEXITING NOW WITH EXIT CODE:");
    
    int sigv;
    if(waitpid(-1, &sigv, WUNTRACED) < 0){
        perror("wait error");
        exit(errno);
    }else{
        printf(" [%d]\n\n", sigv);
        fflush(stdout);
        if(signum == SIGCHLD){
            connectedClients--;
        }
    }
}



void sigpipeHandler(int signum) {
    printf("CLIENT DISCONNECTED\n\n");
    close(client_socket);
    logAnEvent(DISCONNECTION, (Message *) NULL, client_ip, getCurrentTimeStr());
}



void rejectConnection(int client_socket){
    int outcome = TOO_MANY_CLIENTS_CONNECTED;
    int32_t outcome_uint = htonl(outcome);
    write(client_socket, &outcome_uint, sizeof(outcome_uint));
}



void acceptConnection(int client_socket){
    int outcome = CONNECTION_ACCEPTED;
    int32_t outcome_uint = htonl(outcome);
    write(client_socket, &outcome_uint, sizeof(outcome_uint));
}



int execute_operation(Message * data){
    if(data->operation == INSERT){
        return insertContact(data);
    }
    else if(data->operation == EDIT){
        return editContact(data);
    }
    else if(data->operation == DELETE){
        return deleteContact(data);
    }
    else if(data->operation == LOGIN){
       
        return login(data);
    }
    else if(data->operation == LOGOUT){
        logout(data->token);
        return 0;
    }
    return POSITIVE;
}



char * readContacts(){
    logAnEvent(LISTING, (Message *) NULL, client_ip, getCurrentTimeStr());
    int nContacts;
    int outcome;
    char buffer[53];
    char * contactsList;
    FILE * contacts;

    contacts = fopen(CONTACTS_PATH, "a+");
    
    //locking file
    struct flock * fl = lockRD(contacts, 0, EOF, F_SETLKW);

    nContacts = numberOfRecords(contacts, 53);

    contactsList = malloc(2 + nContacts * 53 * sizeof(char));

    if(nContacts == 0){
        unlockFile(contacts, fl);
        contactsList[1] = ZERO_CONTACTS_SAVED;
        return contactsList;
    }

    fseek(contacts, 0, SEEK_SET);

    int i = 0;
    //filling the buffer with the current line data and adding it to "contactsList" array
    while(fgets(buffer, sizeof(buffer), contacts)){
        char * str = &contactsList[i * 53 + 2];
        strncpy(str, buffer, 53);
        printf("%s\n", str);
        i++;
    }

    unlockFile(contacts, fl);
    fclose(contacts);

    contactsList[1] = POSITIVE;
    contactsList[0] = nContacts;
    return contactsList;
}



int insertContact(Message * data){

    fflush(stdout);

    logAnEvent(INSERT, data, client_ip, getCurrentTimeStr());

    // opening file in append mode
    FILE * contacts;
    contacts = fopen(CONTACTS_PATH, "a+");

    //locking file
    struct flock * fl = lockWR(contacts, 0, EOF, F_SETLKW);

    int outcome;

    // Here, we are checking if the contact is already present in the file before inserting it
    if(search_And_Set_ContactIndex(contacts, data) < 0){

        int nCharWritten = fprintf(contacts, "%s %s %s\n", data->name, data->lastName, data->phoneNumber);
        
        // checking that the length of the written data corrisponds to the actual length of the string we wanted to print into the file
        if((strlen(data->name)+strlen(data->lastName)+strlen(data->phoneNumber)+3) != nCharWritten || nCharWritten == -1){
            printf("error during writing phase occurred");
            outcome = ERROR_OCCURED;
        }else{
            printf("operation successfully completed");
            outcome = POSITIVE;
        }
    }else{
        outcome = ALR_EXISTING_CONTACT;
    }

    //unlocking and closing file
    printf("%d is unlocking file...\n", getpid());
    unlockFile(contacts, fl);
    fclose(contacts);

    return outcome;
}



int editContact(Message * data){
    
    logAnEvent(EDIT, data, client_ip, getCurrentTimeStr());

    FILE * contacts;
    char buffer[53];
    int outcome;

    contacts = fopen(CONTACTS_PATH, "r+");
    if(contacts == NULL){
        contacts = fopen(CONTACTS_PATH, "a+");
        fclose(contacts);
        contacts = fopen(CONTACTS_PATH, "r+");
    }

    //Now we are checking if the after-editing contact is one that already exists in the address book, and if so we stop the operation
    Message dataTemp;

    //copying the data
    strcpy(dataTemp.name, data->new_name);
    strcpy(dataTemp.lastName, data->new_lastName);
    strcpy(dataTemp.phoneNumber, data->new_phoneNumber);

    //checking if its already present
    if(search_And_Set_ContactIndex(contacts, &dataTemp) != -1){
        outcome = ALR_EXISTING_CONTACT;
        printf("contact already exists\n");
        return outcome;
    }
    fclose(contacts);

    //this function also closes the original contacts file, so theres no need to close it here
    outcome = rewriteAddressBook(data);

    return outcome;
}



int deleteContact(Message * data){
    logAnEvent(DELETE, data, client_ip, getCurrentTimeStr());
    return rewriteAddressBook(data);
}



int login(Message * data){

    //we allocate memory for the token
    sessionToken = malloc(TOKEN_LENGTH_ * sizeof(char));

    int loginOutcome;
    char buffer[100];
    FILE * users = fopen(USERS_PATH, "r+");

    //locking file
    struct flock * fl = lockRD(users, 0, EOF, F_SETLKW);
    
    //checks if the user is present in the users file
    if(search_And_Set_UserIndex(users, data) != -1){

        //Variables used just for checking if the psw contained in the file is the same as the one sent by the user after hashing
        char * pswSHA256hex;

        //variables for converting data
        unsigned char * hash;
        char * convertedPsw;
        
        fgets(buffer, sizeof(buffer), users);
        char * username = strtok(buffer, " ");
        pswSHA256hex = strtok(NULL, "\n");
        
        //takes space to contain the hashed(sha256) password
        hash = malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));
        convertToSHA256(data->psw, hash);
        
        //takes space to contain the hex(ed) password 
        convertedPsw = malloc(1 + sizeof(unsigned char) * SHA256_DIGEST_LENGTH * 2);
        to_hex(hash, convertedPsw, SHA256_DIGEST_LENGTH);

        free(hash);

        if(strcmp(pswSHA256hex, convertedPsw) == 0){
            free(convertedPsw);

            logAnEvent(SUCCESSFUL_LOGIN_ATTEMPT, data, client_ip, getCurrentTimeStr());
            printf("\nPOSITIVE LOGIN\n\n");
                   
            //inserting the new session token into the array after checking wheter we have a limit of max logged users
            TOKEN str = malloc(32 * sizeof(char));
            gen_token(sessionToken, TOKEN_LENGTH_);
            loginOutcome = POSITIVE;

        }else{
            logAnEvent(UNSUCCESSFUL_LOGIN_ATTEMPT, data, client_ip, getCurrentTimeStr());
            printf("\nPASSWORD NOT CORRECT\n\n");
            loginOutcome = PASSWORD_NOT_CORRECT;
        } 
    }else{
        logAnEvent(UNSUCCESSFUL_LOGIN_ATTEMPT, data, client_ip, getCurrentTimeStr());
        printf("\nUSER NOT FOUND\n\n");
        loginOutcome = USER_NOT_FOUND;
    }

    unlockFile(users, fl);
    fclose(users);
    
    return loginOutcome;  
}



int checkLoginSession(TOKEN token){
    printf("ecco il token: (length = %d)%s | (length = %d)%s\n", strlen(token), token, strlen(sessionToken), sessionToken);
    printf("%d", strcmp(token, sessionToken));
    if(strcmp(token, sessionToken) == 0)
        return 1;
    return -1;
}



void logout(Message * data){
    logAnEvent(LOGOUT, data, client_ip, getCurrentTimeStr());
    free(sessionToken);
}



Message * deconstruct_Message_String(char * msg){
    Message * data  = malloc(sizeof (Message));
    data->operation = msg[0];
    char * mesg;
    mesg = &msg[POS_NAME];
    strncpy(data->name, mesg, 20);
    
    mesg = &msg[POS_LAST_NAME];
    strncpy(data->lastName, mesg, 20);
   
    mesg = &msg[POS_PHONE_NUM];
    strncpy(data->phoneNumber, mesg, 10);
    
    mesg = &msg[POS_NEW_NAME];
    strncpy(data->new_name, mesg, 20);
    
    mesg = &msg[POS_NEW_LAST_NAME];
    strncpy(data->new_lastName, mesg, 20);
    
    mesg = &msg[POS_NEW_PHONE_NUM];
    strncpy(data->new_phoneNumber, mesg, 10);
    
    mesg = &msg[POS_USERNAME];
    strncpy(data->username, mesg, 20);
    
    mesg = &msg[POS_PSW];
    strncpy(data->psw, mesg, 20);
    
    mesg = &msg[POS_TOKEN];
    /*  */

    char * token = malloc(32 * sizeof(char));
    
    strncpy(data->token, mesg, 32);
    data->token[TOKEN_LENGTH_] = '\0';
    
    printf("ecco il token: (length = %d)%s | (length = %d)%s\n", strlen(data->token), data->token, strlen(mesg), mesg);
    fflush(stdout);
    
    return data;
}



int search_And_Set_UserIndex(FILE * users, Message * data){
    char buffer[86];
    fseek(users, 0, SEEK_SET);
    while(fgets(buffer, sizeof(buffer), users) != NULL){
        char * username;
        username = strtok(buffer, " ");
        if(strcmp(username, data->username) == 0)
            return fseek(users, -(strlen(username) + 66), SEEK_CUR);
    }
    return -1;
}



int search_And_Set_ContactIndex(FILE * contacts, Message * data){
    char buffer[53];
    fseek(contacts, 0, SEEK_SET);
    while(fgets(buffer, sizeof(buffer), contacts) != NULL){
        char * name, * lastName, * phoneNumber;
        name = strtok(buffer, " ");
        lastName = strtok(NULL, " ");
        phoneNumber = strtok(NULL, "\n");
        if(strcmp(name, data->name) == 0 && strcmp(lastName, data->lastName) == 0 && strcmp(phoneNumber, data->phoneNumber) == 0)
            return fseek(contacts, -(strlen(name)+strlen(lastName)+strlen(phoneNumber)+3), SEEK_CUR);
    }
    return -1;
}



int numberOfRecords(FILE * file, int recordSize){
    char buffer[recordSize];
    int count = 0;
    fseek(file, 0, SEEK_SET);
    for (char c = getc(file); c != EOF; c = getc(file))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;

    return count;
}



int rewriteAddressBook(Message * data){
    int outcome;
    char buffer[53];

    //opening file
    FILE * contacts = fopen(CONTACTS_PATH, "r+");

    //locking file
    struct flock * fl = lockWR(contacts, 0, EOF, F_SETLKW);

    if(data->operation == DELETE){
        if(search_And_Set_ContactIndex(contacts, data) != -1){
            fseek(contacts, 0, SEEK_SET);

            //we create a temp file for containing the already existing data except the one deleted, and then we rename the file
            FILE * tmpFile;
            tmpFile = fopen("temp.txt", "a+");

            //locking temporary file so we can safely write data on it
            struct flock * flTmp = lockWR(tmpFile, 0, EOF, F_SETLKW);

            while(fgets(buffer, sizeof(buffer), contacts) != NULL){
                char * name, * lastName, * phoneNumber;
                name = strtok(buffer, " ");
                lastName = strtok(NULL, " ");
                phoneNumber = strtok(NULL, "\n");
                
                //if the contact is not the same we can write it down also in the new file
                if(strcmp(name, data->name) != 0 || strcmp(lastName, data->lastName) != 0 || strcmp(phoneNumber, data->phoneNumber) != 0){
                    int nCharWritten = fprintf(tmpFile, "%s %s %s\n", name, lastName, phoneNumber);
                    if((strlen(name)+strlen(lastName)+strlen(phoneNumber)+3) != nCharWritten || nCharWritten == -1){
                        printf("\nerror during writing phase occurred\n");
                        fflush(stdout);
                        outcome = ERROR_OCCURED;
                        
                    }else{
                        printf("\noperation successfully completed\n");
                        fflush(stdout);
                        outcome = POSITIVE;
                        
                    }
    
                }   
            } 

            //we delete the original file and rename the temporary one
            //but unlocking file contacts file first
            unlockFile(contacts, fl);
            fclose(contacts);
            remove(CONTACTS_PATH);

            unlockFile(tmpFile, flTmp); //also unlocking the temporary file
            fclose(tmpFile);
            rename("temp.txt",CONTACTS_PATH);
        }else{
            outcome = CONTACT_NOT_FOUND;
            unlockFile(contacts, fl);
            fclose(contacts);
        }

        return outcome;

    //else if we want to edit a contact...
    }else if(data->operation == EDIT){

        if(search_And_Set_ContactIndex(contacts, data) != -1){
            fseek(contacts, 0, SEEK_SET);

            FILE * tmpFile;
            tmpFile = fopen("temp.txt", "a+");

            //locking temporary file so we can safely write data on it
            struct flock * flTmp = lockWR(tmpFile, 0, EOF, F_SETLKW);

           
            while(fgets(buffer, sizeof(buffer), contacts) != NULL){
                char * name, * lastName, * phoneNumber;
                name = strtok(buffer, " ");
                lastName = strtok(NULL, " ");
                phoneNumber = strtok(NULL, "\n");

                //controllo ogni ciclo se questa riga contiene o meno il contatto ricercato e in caso stampo il contatto modificato sul file temporaneo
                if(strcmp(name, data->name) == 0 && strcmp(lastName, data->lastName) == 0 && strcmp(phoneNumber, data->phoneNumber) == 0){
                    int nCharWritten = fprintf(tmpFile, "%s %s %s\n", data->new_name, data->new_lastName, data->new_phoneNumber);
                    if((int)(strlen(data->new_name)+strlen(data->new_lastName)+strlen(data->new_phoneNumber)+3) != nCharWritten || nCharWritten == -1){
                        printf("\nerror during writing phase occurred\n");
                        fflush(stdout);
                        outcome = ERROR_OCCURED;
                    }else{
                        printf("\noperation successfully completed\n");
                        fflush(stdout);
                        outcome = POSITIVE;
                    }
                }else{
                    int nCharWritten = fprintf(tmpFile, "%s %s %s\n", name, lastName, phoneNumber);
                    if((strlen(name)+strlen(lastName)+strlen(phoneNumber)+3) != nCharWritten || nCharWritten == -1){
                        printf("\nerror during writing phase occurred\n");
                        fflush(stdout);
                        outcome = ERROR_OCCURED;
                    }else{
                        printf("\noperation successfully completed\n");
                        fflush(stdout);
                        outcome = POSITIVE;
                    }
                } 
            } 
            //we delete the original file and rename the temporary one
            //but unlocking file contacts file first
            unlockFile(contacts, fl);
            fclose(contacts);
            remove(CONTACTS_PATH);
            unlockFile(tmpFile, flTmp); //also unlocking the temporary file
            fclose(tmpFile);
            rename("temp.txt",CONTACTS_PATH);
        
        }else{
            outcome = CONTACT_NOT_FOUND;
            unlockFile(contacts, fl);
            fclose(contacts);
        }
        return outcome;
    }
}



int logAnEvent(int eventCode, Message * data, char * address, char * time){
    FILE * logfile = fopen(LOG_PATH, "a+");
    struct flock * fl = lockWR(logfile, 0, EOF, F_SETLKW);

    int outcome = POSITIVE;
    switch(eventCode){
        case SUCCESSFUL_CONNECTION_ATTEMPT:
            outcome = fprintf(logfile, "[%s] Client %s connected successfully (process with PID: %d created)\n", time, address, getpid());
            printf("errore: %d\n", outcome);
            fflush(stdout);
            break;
        case UNSUCCESSFUL_CONNECTION_ATTEMPT:
            outcome = fprintf(logfile, "[%s] Client %s connected unsuccessfully(too many clients connected)\n", time, address);
            break;
        case DISCONNECTION:
            outcome = fprintf(logfile, "[%s] Client %s disconnected(process with PID: %d terminated)\n", time, address, getpid());
            break;
        case LISTING:
            outcome = fprintf(logfile, "[%s] Client %s requested LISTING operation (PID: %d)\n", time, address, pid);
            break;
        case INSERT:
            outcome = fprintf(logfile, "[%s] Client %s requested INSERT of contact {%s %s %s}(PID: %d)\n", time, address, data->name, data->lastName, data->phoneNumber, getpid());
            break;
        case EDIT:
            outcome = fprintf(logfile, "[%s] Client %s requested EDIT of contact {%s %s %s} to {%s %s %s} (PID: %d)\n", time, address,  data->name, data->lastName, data->phoneNumber, data->new_name, data->new_lastName, data->new_phoneNumber, getpid());
            break;
        case DELETE:
            outcome = fprintf(logfile, "[%s] Client %s requested DELETE of contact {%s %s %s} (PID: %d)\n", time, address,  data->name, data->lastName, data->phoneNumber, getpid());
            break;
        case SUCCESSFUL_LOGIN_ATTEMPT:
            outcome = fprintf(logfile, "[%s] Client %s LOGGED IN successfully as USER:{%s} (PID: %d)\n", time, address,  data->username, getpid());
            break;
        case UNSUCCESSFUL_LOGIN_ATTEMPT:
            outcome = fprintf(logfile, "[%s] Client %s unsuccessful login as USER:{%s} (PID: %d)\n", time, address,  data->username, getpid());
            break;
        case LOGOUT:
            outcome = fprintf(logfile, "[%s] Client %s logged out as USER:{%s} (PID: %d)\n", time, address,  data->username, getpid());
            break;
        default:
            outcome = ERROR_OCCURED;
            break;
    }
    
    unlockFile(logfile, fl);
    printf("ci sono\n");
    fflush(stdout);
    
    fclose(logfile);

    return outcome;
}



/**
 * @brief Main function of the server.  Handles client connections, forks child processes, and processes client requests.
 *
 * @param argc Argument count.
 * @param argv Argument array.
 * @return 0 on successful termination, or a error code.
 */
int main(int argc, char *argv[]) {

    //setting up signal handlers
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigintHandler);
    signal(SIGCHLD, sigchldHandler);

    //array for containing data during reading
    char buffer[BUFFER_SIZE] = {0};

    //address var
    struct sockaddr_in * client_address;

    //creating the socket on servers side
    server_socket = socketSetUp(1);

    //binding process
    client_address = binding(server_socket, SERVER_PORT);

    int addrlen = sizeof(*client_address);

    // Listening
    if (listen(server_socket, MAX_CLIENTS_) < 0) {
        perror("listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on PORT: %d\n", SERVER_PORT);

    //waiting for clients requests
    while(1){

        pid_t childpid; /* variable to store child's process id */
        char * acceptTime = malloc(30 * sizeof(char));

        // Accepting connections
        if ((client_socket = accept(server_socket, (struct sockaddr *)client_address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            fflush(stdout);
            close(server_socket);
            exit(EXIT_FAILURE);
        }else{
            printf("%d ha accettato", getpid());
            fflush(stdout);
            if((connectedClients + 1) > MAX_CLIENTS_){
                logAnEvent(UNSUCCESSFUL_CONNECTION_ATTEMPT, (Message *) NULL, client_ip, getCurrentTimeStr());
                rejectConnection(client_socket);            
                close(client_socket);
                continue;
            }else{
                acceptTime = getCurrentTimeStr();
                acceptConnection(client_socket);
                connectedClients++;
            }
        }

        // Conversione dell'indirizzo IP del client in stringa
        if (inet_ntop(AF_INET, (struct in_addr*)&(client_address->sin_addr.s_addr), client_ip, INET_ADDRSTRLEN) == NULL) {
            perror("IP conversion error: ");
            close(client_socket);
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        int option = 1;
        if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
            perror("Errore in setsockopt");
            fflush(stdout);
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        if ((childpid = fork()) == -1)
        { // fork failed.
            close(client_socket);
            continue;
        }

        else if (childpid > 0)
        { // parent process
            //saving the pid(used for logging)
            pid = getpid();
            printf("\n parent process: %d\n", childpid);
            close(client_socket);
        }

        // child process
        else if (childpid == 0)
        { 
            signal(SIGPIPE, sigpipeHandler);

            //saving the pid(used for logging right after)

            pid = getpid();
            logAnEvent(SUCCESSFUL_CONNECTION_ATTEMPT, (Message *) NULL, client_ip, acceptTime);

            printf("\n child process: %d\n", pid);

            close(server_socket);
            
            while(1){   
                int valread = -1;

                //NOTE: generally the first read doesnt receive a -1 if client disconnects...
                //so we have to put a check afterward so that the disconnection is registered in the log and the process exits
                valread = read(client_socket, buffer, BUFFER_SIZE);
                if(valread <= 0){
                    printf("read from client failed(valread = %d)\n", valread);
                    fflush(stdout);
                    logAnEvent(DISCONNECTION, (Message *) NULL, client_ip, getCurrentTimeStr());
                    close(client_socket);
                    exit(-1);
                }
                
                printf("\n\n(valread = %d)buffer:\n\n", valread);
                fflush(stdout);
                for(int i = 0; i < valread; i++){
                    printf("%c\n", buffer[i]);
                    fflush(stdout);
                } 
                
                // returns buffers data into a proper struct
                Message * data = deconstruct_Message_String(buffer);
                printf("%d ha ricevuto ricevuta: %c\n", getpid(), data->operation);
                fflush(stdout);

                //if the op. is "LISTING" we have to send the num. of contacts before sending the array containing them 
                if(data->operation == LISTING){
                    char * contactsList = readContacts();
                    int32_t nContacts_uint = htonl(contactsList[0]);
                    write(client_socket, &nContacts_uint, sizeof(nContacts_uint));
                    write(client_socket, &contactsList[1], (sizeof(char) * contactsList[0] * 53 + 1));
                    free(contactsList);
                    free(data);
                }else{ 
                    if(data->operation != LOGIN){

                        // SECURITY CHECK: I check if the access to logged users-only operations is being made normally
                        if(checkLoginSession(data->token) < 0 && data->operation != LOGOUT){
                            int outcome = TRYING_ILLEGAL_ACCESS;
                            int32_t outcome_uint = htonl(outcome);
                            write(client_socket, &outcome_uint, sizeof(outcome_uint));
                            free(data);
                        }else{
                            
                            int outcome = execute_operation(data);
                            int32_t outcome_uint = htonl(outcome);
                            write(client_socket, &outcome_uint, sizeof(outcome_uint));
                            free(data);
                        }
                    }else{
                        int outcome = execute_operation(data);
                        char * buff = malloc(33 * sizeof(char));
                        buff[0] = outcome;
                        if(outcome == POSITIVE)
                            strncpy((char *)&(buff[1]), (const char *)sessionToken, strlen(sessionToken)); 
                        write(client_socket, buff, 33 * sizeof(char));
                        free(data);
                        free(buff);
                    }
                }
            }
        }
        printf("server: got connection from %s\n", inet_ntoa(client_address->sin_addr));     
    }
}