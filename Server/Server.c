/**
 * File: Server.c
 * Project: Address Book in C Language
 *
 * Description:
 * This program implements a simple address book application
 * to manage contacts using the C programming language.
 *
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
#include <bits/waitflags.h>
#include <sys/wait.h>
#include "errno.h"


//we keep an array of tokens, specifically there will be 10, the max number of connected client allowed at the same time
TOKEN * sessionTokens;
int lastSessionToken = 0;



/// @brief used for socket set up
/// @param option, the option for socket setup 
/// @return the socket's file descriptor
int socketSetUp(int option){
    int server_fd;
    
    // Creation of socket's fd
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    //using option to indicate an optional value for socket options in "setsocketopt"
    //if SO socket is not closed we reuse it
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
        perror("Errore in setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    return server_fd;
}




/// @brief //used for binding
/// @param socket the sockets fd
/// @param port the port for connection
/// @return the internet socket address(sockaddr_in * type)
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





//server/client sockets fd
int server_fd, new_socket;

//Signal handler for the signal SIGINT
void sigintHandler(int signum) {
    close(server_fd);
    close(new_socket);
    exit(0);
}

//Signal handler for the signal SIGCHLD
void sigchldHandler(int signum) {
    printf("\nCIAOOOOOO, SI STO CHIUDENDO ORAAAAA");
    
    int sigv;
    if(waitpid(-1, &sigv, WUNTRACED) < 0){
        perror("wait error");
        exit(errno);
    }else{
        printf(" CON EXIT(%d)\n", sigv);
        fflush(stdout);
    }
}




//main function
int main(int argc, char *argv[]) {

    //setting up signal handlers
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigintHandler);
    signal(SIGCHLD, sigchldHandler);

    //we allocate memory for all the tokens
    sessionTokens = malloc(MAX_USERS_ * sizeof(TOKEN));
    for(int i = 0; i < MAX_USERS_; i++){
        sessionTokens[i] = malloc(TOKEN_LENGTH_ * sizeof(char) +1);
    } 

    //array for containing data during reading
    char buffer[BUFFER_SIZE] = {0};

    //address var
    struct sockaddr_in *address;

    
    
    //creating the socket on servers side
    server_fd = socketSetUp(1);

    //binding process
    address = binding(server_fd, SERVER_PORT);

    int addrlen = sizeof(*address);

    // Listening
    if (listen(server_fd, MAX_USERS_) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on PORT: %d\n", SERVER_PORT);

    

    //waiting for clients requests
    while(1){

        pid_t childpid; /* variable to store child's process id */

        // Accepting connections
        if ((new_socket = accept(server_fd, (struct sockaddr *)address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            fflush(stdout);
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        int option = 1;
        if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
            perror("Errore in setsockopt");
            fflush(stdout);
            close(server_fd);
            exit(EXIT_FAILURE);
        }

    
        if ((childpid = fork()) == -1)
        { // fork failed.
            close(new_socket);
            continue;
        }
        else if (childpid > 0)
        { // parent process
            printf("\n parent process: %d\n", childpid);
            close(new_socket);
        }
        else if (childpid == 0)
        { // child process
            printf("\n child process: %d\n", childpid);

            // reading data from client
            int valread = read(new_socket, buffer, BUFFER_SIZE);
            if(valread == -1){
                // closes connection
                close(new_socket);
                close(server_fd);
                perror("client disconnected");
                exit(-1);
            }

            // returns buffers data into a proper struct
            Message * data = deconstruct_Message_String(buffer);
            
            //if the op. is "LISTING" we have to send the num. of contacts before sending the array containing them 
            if(data->operation == LISTING){
                char * contactsList = readContacts();
                int32_t nContacts_uint = htonl(contactsList[0]);
                write(new_socket, &nContacts_uint, sizeof(nContacts_uint));
                write(new_socket, &contactsList[1], (sizeof(char) * contactsList[0] * 53));

            }else{   
                if(data->operation != LOGIN){

                    // SECURITY CHECK: I check if the access to logged users-only operations is being made normally
                    if(checkLoginSession(data->token) < 0){
                        int outcome = TRYING_ILLEGAL_ACCESS;
                        int32_t outcome_uint = htonl(outcome);
                        write(new_socket, &outcome_uint, sizeof(outcome_uint));
                    }else{
                        int outcome = execute_operation(data);
                        int32_t outcome_uint = htonl(outcome);
                        int val = write(new_socket, &outcome_uint, sizeof(outcome_uint));
                    }
                }else{
                    int outcome = execute_operation(data);
                    char * buffer = malloc(33 * sizeof(char));
                    buffer[0] = outcome;
                    if(outcome == POSITIVE)
                        strncpy((char *)&(buffer[1]), (const char *)sessionTokens[lastSessionToken-1], strlen(sessionTokens[lastSessionToken-1])); 
                    write(new_socket, buffer, 33 * sizeof(char));
                }
            }
            close(new_socket);
            exit(0);
        }
        printf("server: got connection from %s\n", inet_ntoa(address->sin_addr));
            

    }

}



/// @brief Functions that simply decides which operation to perform based on the "operation" field contained in data
/// @param data variable of Message struct type that contains all data received from a client
/// @return an integer containing a value representing the outcome of the operation
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
        //logout()
    }
    return 0;
}



/// @brief Function which reads all contacts contained on the address book file and puts them in an array of size 53*n+2, where n is the number of contacts and where the 2 additional positions contain outcome of the op. and number of contacts.
/// @return returns the new array containing outcome and n.of contancts in the first 2 positions, and the contacts of the address book in the rest of the array.\n P.S. : the outcome value can be 3(ZERO_CONTACTS_SAVED) or 0(POSITIVE)
char * readContacts(){
    int nContacts;
    int outcome;
    char buffer[53];
    char * contactsList;
    FILE * contacts;

    contacts = fopen(CONTACTS_PATH, "a+");
    contactsList = malloc(2 + nContacts * 53 * sizeof(char));

    nContacts = numberOfContacts(contacts);
    if(nContacts == 0){
        contactsList[1] = ZERO_CONTACTS_SAVED;
        return contactsList;
    }

    fseek(contacts, 0, SEEK_SET);

    int i = 0;
    //filling the buffer with the current line data and adding it to "contactsList" array
    while(fgets(buffer, sizeof(buffer), contacts)){
        char * str = &contactsList[i * 53 + 2];
        strncpy(str, buffer, 53);
        printf("%s", str);
        i++;
    }

    contactsList[1] = POSITIVE;
    contactsList[0] = nContacts;
    return contactsList;
}



/// @brief function that takes a Message struct as input and inserts the contact contained into the address book file
/// @param data variable of Message struct type that contains all data received from a client
/// @return the outcome
int insertContact(Message * data){
    
    // opening file in append mode
    FILE * contacts;
    contacts = fopen(CONTACTS_PATH, "a+");

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
    fclose(contacts);
    return outcome;
}



/// @brief function that takes a Message struct as input and edits a contact of the address book
/// @param data variable of Message struct type that contains all data received from a client
/// @return the outcome
int editContact(Message * data){
    FILE * contacts;
    char buffer[53];
    int outcome;

    contacts = fopen(CONTACTS_PATH, "r+");
    if(contacts == NULL){
        contacts = fopen(CONTACTS_PATH, "a+");
        fclose(contacts);
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
        printf("contact already exists");
        return outcome;
    }

    free(&dataTemp);

    return rewriteAddressBook(contacts, data);
}



/// @brief function that takes a Message struct as input and deletes a contact of the address book
/// @param data variable of Message struct type that contains all data received from a client
/// @return the outcome
int deleteContact(Message * data){
    FILE * contacts;
    contacts = fopen(CONTACTS_PATH, "r+");
    return rewriteAddressBook(contacts, data);;
}



/// @brief function which cheks wheter the username and password data sent by the client are correspondent to a certain user account and in case creates and sends back to the client a uniquely identified session token
/// @param data variable of Message struct type that contains all data received from a client
/// @return the outcome
int login(Message * data){
    int loginOutcome;
    char buffer[100];
    FILE * users = fopen(USERS_PATH, "r+");
    
    //checks if the user is present in the users file
    if(search_And_Set_UserIndex(users, data) != -1){
        fseek(users, 0, SEEK_CUR);


        //Variables used just for checking if the psw contained in the file is the same as the one sent by the user after hashing
        char * pswSHA256hex;

        //variables for converting data
        unsigned char * hash;
        char * convertedPsw;
        
        fgets(buffer, sizeof(buffer), users);
        strtok(buffer, " ");
        pswSHA256hex = strtok(NULL, "\n");
        
        //takes space to contain the hashed(sha256) password
        hash = malloc(SHA256_BLOCK_SIZE * sizeof(unsigned char));
        convertToSHA256(data->psw, hash);
        
        //takes space to contain the hex(ed) password 
        convertedPsw = malloc(1 + sizeof(unsigned char) * SHA256_BLOCK_SIZE * 2);
        to_hex(hash, convertedPsw, SHA256_BLOCK_SIZE);
        
        if(strcmp(pswSHA256hex, convertedPsw) == 0){
            printf("POSITIVE LOGIN");
            if(lastSessionToken < (MAX_USERS_-1)){
                
                //insrting the new session token into the array after checking wheter we have a limit of max logged users
                TOKEN str = malloc(32 * sizeof(char));
                gen_token(sessionTokens[lastSessionToken], TOKEN_LENGTH_);
                lastSessionToken++;
                loginOutcome = POSITIVE;

            }else{
                printf("\nTOO MANY CLIENTS CONNECTED\n\n");
                loginOutcome = TOO_MANY_CLIENTS_CONNECTED;
            } 
            
        }else{
            printf("\nPASSWORD NOT CORRECT\n\n");
            loginOutcome = PASSWORD_NOT_CORRECT;
        } 
    }else{
        printf("\nSUER NOT FOUND\n\n");
        loginOutcome = USER_NOT_FOUND;
    }
    return loginOutcome;  
}



/// @brief function that checks login session, serves only a security purpose
/// @param token token we want to compare
/// @return the outcome of the checking operation
int checkLoginSession(TOKEN token){
    for(int i = 0; i < lastSessionToken; i++){
        if(strcmp(token, sessionTokens[i]) == 0){
            return 1;
        }
    }
    return -1;
}



/// @brief function for logging out. takes the session token as input
/// @param token 
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



//simple function that builds a Message struct variable using clients message array data
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
    strncpy(data->token, mesg, 32);
    
    return data;
}



//after passing an already opened file, this function searches for a user in the users file and returns the outcome of the fseek operation
int search_And_Set_UserIndex(FILE * users, Message * data){
    char buffer[54];
    fseek(users, 0, SEEK_SET);
    while(fgets(buffer, sizeof(buffer), users) != NULL){
        char * username;
        username = strtok(buffer, " ");
        if(strcmp(username, data->username) == 0)
            return fseek(users, -(strlen(username) + 34), SEEK_CUR);
    }
    return -1;
}



//after passing an already opened file, this function searches for a contact in the address book file and returns the outcome of the fseek operation
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



//after passing an already opened file, this function counts the number of Contacts in the address book file
int numberOfContacts(FILE * contacts){
    char buffer[53];
    int count = 0;
    fseek(contacts, 0, SEEK_SET);
    for (char c = getc(contacts); c != EOF; c = getc(contacts))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;

    return count;
}



//after passing an already opened file, this function changes the data of the address book, updating or removing a certain contact
int rewriteAddressBook(FILE * contacts, Message * data){
    int outcome;
    char buffer[53];
    if(data->operation == DELETE){
        if(search_And_Set_ContactIndex(contacts, data) != -1){
            fseek(contacts, 0, SEEK_SET);

            //we create a temp file for containing the already existing data except the one deleted, and then we rename the file
            FILE * tmpFile;
            tmpFile = fopen("temp.txt", "a+");
            while(fgets(buffer, sizeof(buffer), contacts) != NULL){
                char * name, * lastName, * phoneNumber;
                name = strtok(buffer, " ");
                lastName = strtok(NULL, " ");
                phoneNumber = strtok(NULL, "\n");
                
                //if the contact is not the same we can write it down also in the new file
                if(strcmp(name, data->name) != 0 && strcmp(lastName, data->lastName) != 0 && strcmp(phoneNumber, data->phoneNumber) != 0){
                    int nCharWritten = fprintf(tmpFile, "%s %s %s\n", name, lastName, phoneNumber);
                    if((strlen(name)+strlen(lastName)+strlen(phoneNumber)+3) != nCharWritten || nCharWritten == -1){
                        printf("errore nella fase di scrittura");
                        outcome = ERROR_OCCURED;
                    }else{
                        printf("operation avvenuta correttamente");
                        outcome = POSITIVE;
                    }
                }   
            } 
            //we delete the original file and rename the temporary one
            fclose(contacts);
            remove(CONTACTS_PATH);
            fclose(tmpFile);
            rename("temp.txt",CONTACTS_PATH);
            
        }else{
            outcome = CONTACT_NOT_FOUND;
        }

        return outcome;

    //else if we want to edit a contact...
    }else if(data->operation == EDIT){

        if(search_And_Set_ContactIndex(contacts, data) != -1){
            fseek(contacts, 0, SEEK_SET);

            FILE * tmpFile;
            tmpFile = fopen("temp.txt", "a+");

            while(fgets(buffer, sizeof(buffer), contacts) != NULL){
                char * name, * lastName, * phoneNumber;
                name = strtok(buffer, " ");
                lastName = strtok(NULL, " ");
                phoneNumber = strtok(NULL, "\n");

                //controllo ogni ciclo se questa riga contiene o meno il contatto ricercato e in caso stampo il contatto modificato sul file temporaneo
                if(strcmp(name, data->name) == 0 && strcmp(lastName, data->lastName) == 0 && strcmp(phoneNumber, data->phoneNumber) == 0){
                    int nCharWritten = fprintf(tmpFile, "%s %s %s\n", data->new_name, data->new_lastName, data->new_phoneNumber);
                    if((int)(strlen(data->new_name)+strlen(data->new_lastName)+strlen(data->new_phoneNumber)+3) != nCharWritten || nCharWritten == -1){
                        printf("error during writing phase occurred");
                        outcome = ERROR_OCCURED;
                    }else{
                        printf("operation successfully completed");
                        outcome = POSITIVE;
                    }
                }else{
                    int nCharWritten = fprintf(tmpFile, "%s %s %s\n", name, lastName, phoneNumber);
                    if((strlen(name)+strlen(lastName)+strlen(phoneNumber)+3) != nCharWritten || nCharWritten == -1){
                        printf("error during writing phase occurred");
                        outcome = ERROR_OCCURED;
                    }else{
                        printf("operation successfully completed");
                        outcome = POSITIVE;
                    }
                } 
            } 
            //we delete the original file and rename the temporary one
            fclose(contacts);
            remove(CONTACTS_PATH);
            fclose(tmpFile);
            rename("temp.txt",CONTACTS_PATH);
        
        }else{
            outcome = CONTACT_NOT_FOUND;
        }
        return outcome;
    }
}



//utility function for converting a given string to hex format
void to_hex(const unsigned char *hash, char *output, size_t length) {
    for (size_t i = 0; i < length; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[length * 2] = '\0';
}



//utility function for converting a given string to sha256 format. returns an unsigned char array of 32 bytes
unsigned char * convertToSHA256(char * str, unsigned char * hash)
{
    SHA256_CTX sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, str, strlen(str));
    sha256_final(&sha256, hash);
}
  


//utility function that only generates a random alpha-numeric token(random string) of a certain given "length"
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