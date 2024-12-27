/**
 * File: Client.c
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

#include "Client.h"


//here we are defining some global values which come in handful since are to be accessed by many functions
TOKEN sessionTOKEN;
int client_sock;
int logged = 0; //used only for UI representation, the real login checking happens server's side
struct sockaddr_in serv_addr;
int first = 1;
int fileNumCounter = 1;
int intChoice = 0;



// Menu options for logged users
const char *logged_in_options[] = {
    "[1] LIST all contacts",
    "[2] ADD a new contact",
    "[3] MODIFY an existing contact",
    "[4] DELETE a contact",
    "[-] LOGOUT",
    "[ EXIT ]"
};


//menu options for non logged users
const char *logged_out_options[] = {
    "[1] LIST all contacts",
    "[🔒] LOGIN TO SEE OTHER AVAILABLE OPERATIONS [press '+']",
    "[esc] EXIT"
};


// values
const int numeric_menu_options[] = {
    '1',
    '2',
    '3',
    '4',
    '-',
    27
};

//we use this because its easier to use when calling menu function
const char **logged_options[] = {logged_out_options, logged_in_options};


void printMenu(const char **options, int num_options, int selected, char * nextString) {
    system("clear || cls");
    printf("*****************************************************\n");
    printf("*                                                   *\n");
    printf("*%s            WELCOME TO THE ADDRESS BOOK%s            *\n", BLU, RESET);
    printf("*                                                   *\n");
    printf("*****************************************************\n\n");
    //printf("%s\n", options[0]);
    printf("%s  Choose an operation:%s\n", GRN, RESET);
    printf("  ---------------------------------------------\n");

    for (int i = 0; i < num_options; i++) {
        if (i == selected) {
            printf("  %s %s<%s\n", options[i], YEL, RESET);
        } else {
            printf("  %s\n", options[i]);
        }
    }

    printf("  ---------------------------------------------\n");

    printf("%s", nextString);
    fflush(stdout);
}


int menu(int logged, const char **options, char * nextString) {
    int selected = 0;
    char input = 0;

    // raw mode(no blocking input)
    enable_raw_mode();


    while (1) {
        int num_options = (logged == 1) ? 6 : 3;
        printMenu(options, num_options, selected, nextString);

        input = getchar();
        printf("input: %d\n\n\n\n\n",input);
        fflush(stdout);
        sleep(1);

        if (input == '\u001b') { // ESC
            disable_raw_mode();
            exit(0); // Indica uscita
        }
        
        if (input == '\033') { 
            getchar();         
            switch (getchar()) {
                case 'A': // arrow up
                    selected = (selected - 1 + num_options) % num_options;
                    break;
                case 'B': // arrow down
                    selected = (selected + 1) % num_options;
                    break;
            }
        } else if (input == '\n') { // enter
            disable_raw_mode();
            return selected + 1; // returns the selected option
        } 
    }
}


char parseSelected(int selected, int logged){
    if(logged == 1){
        printf("Selected: %c\n", numeric_menu_options[selected - 1]);
        return numeric_menu_options[selected - 1];
    }else{
        switch(selected){
            case 1:
                return '1';
            case 2:
                return '+';
            case 3:
                return 27;
        }
    }
}


struct sockaddr_in * buildSocketaddress(char * serverAddress, int serverPort){
    struct sockaddr_in * serv_addr;
    serv_addr = malloc(sizeof(struct sockaddr_in));

    //Server's data
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(serverPort);
    serv_addr->sin_addr.s_addr = inet_addr(serverAddress);

    return serv_addr;
}


int connect_To_Server(char * serverAddress, int serverPort){
    int sock = 0;
    
    
    //Socket's creation
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf(SOCKET_CREATION_ERROR "\n");
        exit(-1);
    }

    //we build the sockaddr_in struct
    struct sockaddr_in * serv_addr = buildSocketaddress(serverAddress, serverPort);

    //Connection to server
    return tryConnection(sock, serv_addr);
}


int tryConnection(int sock, struct sockaddr_in * serv_addr){
    int outcome;
    //Connection to server
    while(1){
        if(connect(sock, serv_addr, sizeof(*serv_addr)) < 0 || (outcome = checkTooManyClientsConnected(sock, serv_addr)) != CONNECTION_ACCEPTED) {
            printOutcome(outcome);

            //recreating sock
            close(sock);
            //Socket's recreation
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf(SOCKET_CREATION_ERROR "\n");
                exit(-1);
            }

            char retry [10];
            printf(RED"-------> "CONNECTION_ERROR":  " RESET);
            scanf("%s",retry);
            
            //if the inserted value in the terminal is y(non case sensitive) we retry connection. we exit otherwise
            if(strcmp(retry, "Y") == 0 || strcmp(retry, "y") == 0 ){
                printf(YEL"                  retrying connection...\n\n"RESET);
            }else{
                printf(RED"\n\n                  *PROGRAM TERMINATED*\n\n"RESET);
                close(sock);
                exit(-1);
            }
        }else{
            first = 1;
            printOutcome(outcome);
            sleep(1.5);
            return sock;
        }
    }
}
    

int checkTooManyClientsConnected(int client_socket, struct sockaddr_in * serv_addr) {
    
    //receiving outcome
    int32_t outcome_uint;
    int valread = read(client_socket, &outcome_uint, sizeof(outcome_uint));

    if (valread <= 0) {
        
        // Handle potential read error or disconnection
        printf(RED SERVER_DISCONNECTED_ERROR RESET);
        close(client_socket);
        exit(-1); 
        return;
    }

    return ntohl(outcome_uint);
}


void sigintHandler(int signum) {
    printf("\n\n");
    printf(YEL"               you are now exiting the program..."RESET);
    printf("\n");
    close(client_sock);
    exit(0);
}


char * getOutcomeString(int outcome){
    return outcome_strings[outcome];
}


void printOutcome(int outcome){

    switch (outcome){
        case POSITIVE:
            printf(GRN"\n\n            "POSITIVE_STR RESET"\n\n\n");
            break;
        case ERROR_OCCURED:
            printf(RED"\n\n            "ERROR_OCCURED_STR RESET"\n\n\n");
            break;
        case ALR_EXISTING_CONTACT:
            printf(RED "\n\n           "ALR_EXISTING_CONTACT_STR RESET"\n\n\n");
            break;
        case CONTACT_NOT_FOUND:
            printf(RED"\n\n            "CONTACT_NOT_FOUND_STR RESET"\n\n\n");
            break;
        case ZERO_CONTACTS_SAVED:
            printf(RED"\n\n            "ZERO_CONTACTS_SAVED_STR RESET"\n\n\n");
            break;
        case TRYING_ILLEGAL_ACCESS:
            printf(RED"\n\n            "TRYING_ILLEGAL_ACCESS_STR RESET"\n\n\n");
            break;
        case PASSWORD_NOT_CORRECT:
            printf(RED"\n\n            "PASSWORD_NOT_CORRECT_STR RESET"\n\n\n");
            break;
        case USER_NOT_FOUND:
            printf(RED"\n\n            "USER_NOT_FOUND_STR RESET"\n\n\n");
            break;
        case CONNECTION_ACCEPTED:
            printf(GRN"\n\n            "CONNECTION_ACCEPTED_STR RESET"\n\n\n");
            break;
        case TOO_MANY_CLIENTS_CONNECTED:
            printf(RED"\n\n            "TOO_MANY_CLIENTS_CONNECTED_STR RESET"\n\n\n");
            break;
        
    }
}


Message * choose_operation(){

    if(first == 1){
        intChoice = menu(logged, logged_options[logged], " ");
        first = 0;
    }
    

    printMenu(logged_options[logged], (logged == 1) ? 6 : 3, intChoice, " ");

    char choice = parseSelected(intChoice, logged);

    //variables used for range and data checks for teminal menu printing, so basically only for users interface
    int choiceInCorrectRange = 1;
    int dataInsertedCorrectly = 1;

    /*using a while loop to print out menu and eventual errors of sort
    until the user has chosen a certain operation and submitted correct data*/
    do{ 

        if(logged){

            //defining and taking memory space to save message datas
            Message * data;
            data = malloc(sizeof(Message));

            //initializing the interface check variables
            choiceInCorrectRange = 1;
            dataInsertedCorrectly = 1;

            //building the message and doing specific checks on data based on the chosen operation
            switch(choice){
                
                case LISTING:
                    data->operation = choice;
                    return data;

                case INSERT: 
                case DELETE:
                case EDIT: 
                    
                    data->operation = choice;

                    char * name, * lastName, * phoneNumber;
                    name = malloc(200 * sizeof(char));
                    lastName = malloc(200 * sizeof(char));
                    phoneNumber = malloc(200 * sizeof(char));

                    //checking name
                    printf(BLU"• Name: "RESET);
                    scanf("%s", name);
                    if(checkInsertedData(name, "name") == 0){
                        
                        strcpy(data->name, name);
                    }else{
                        
                        dataInsertedCorrectly = 0;
                        break;
                    }
                    
                    //checking last name
                    printf(BLU"• Last name: "RESET);
                    scanf("%s", lastName);
                    if(checkInsertedData(lastName, "last name") == 0){
                        strcpy(data->lastName, lastName);
                    }else{
                        dataInsertedCorrectly = 0;
                        break;
                    }
                        
                    //checking phone number
                    printf(BLU"• Phone number: "RESET);
                    scanf("%s", phoneNumber);
                    if(checkInsertedData(phoneNumber, "phone number") == 0){
                        strcpy(data->phoneNumber, phoneNumber);
                    }else{
                        dataInsertedCorrectly = 0;
                        break;
                    }
                        

                    //we also have to consider new values in case of editing of contacts
                    if(choice == EDIT){
                        
                        //checking name
                        printf(BLU"• Name: "RESET);
                        scanf("%s", name);
                        if(checkInsertedData(name, "name") == 0){
                            strcpy(data->new_name, name);
                        }else{
                            dataInsertedCorrectly = 0;
                            break;
                        }

                        //checking last name
                        printf(BLU"• Last name: "RESET);
                        scanf("%s", lastName);
                        if(checkInsertedData(lastName, "last name") == 0){
                            strcpy(data->new_lastName, lastName);
                        }else{
                            dataInsertedCorrectly = 0;
                            break;
                        }

                        //checking phone number
                        printf(BLU"• Phone number: "RESET);
                        scanf("%s", phoneNumber);
                        if(checkInsertedData(phoneNumber, "phone number") == 0){
                            strcpy(data->new_phoneNumber, phoneNumber);
                        }else{
                            dataInsertedCorrectly = 0;
                            break;
                        }
                    }
                    return data;
                
                case EXIT:
                case LOGOUT: 

                    if(logged == 1){
                        data->operation = choice;

                        logged = 0;

                        free(sessionTOKEN);
                        sessionTOKEN = malloc(TOKEN_LENGTH_ * sizeof(char));
                    }//if we only want to logout, we execute the above, else if we are not logged in, we execute only this
                    if(data->operation == EXIT){
                        close(client_sock);
                        exit(0);
                    }
                    return data;

                default: 
                    break;
            }
        }else{
            //same as above....
            //initializing the interface check variables
            choiceInCorrectRange = 1;
            dataInsertedCorrectly = 1;

            Message * data;
            data = malloc(sizeof(Message));
            switch(choice){ 
                
                case LISTING:
                    data->operation = LISTING; 
                    return data;

                case LOGIN: 
                    data->operation = LOGIN;

                    //checking username
                    printf(MAG"• Username: "RESET);
                    scanf("%s", data->username);
                    if(checkInsertedData(data->username, "username") == 0){
                        strcpy(data->username, data->username);
                    }else{
                        dataInsertedCorrectly = 0;
                        break;
                    }

                    //checking password
                    printf(MAG"• Password: "RESET);
                    scanf("%s", data->psw);
                    if(checkInsertedData(data->psw, "password") == 0){
                        strcpy(data->psw, data->psw);
                    }else{
                        dataInsertedCorrectly = 0;
                        break;
                    }

                    return data;

                case EXIT:
                    close(client_sock);
                    exit(0);
    
                default:
                    break;
                }
        }
        
    }while(choiceInCorrectRange == 0 || dataInsertedCorrectly == 0); 
}


void create_Message_String(char message[], Message * data){
    char * msg = &message[0];
    msg[0] = data->operation;
    msg = &message[POS_NAME];
    strcpy(msg, data->name);
    msg = &message[POS_LAST_NAME];
    strcpy(msg, data->lastName);
    msg = &message[POS_PHONE_NUM];
    strcpy(msg, data->phoneNumber);
      
    msg = &message[POS_NEW_NAME];
    strcpy(msg, data->new_name);
    msg = &message[POS_NEW_LAST_NAME];
    strcpy(msg, data->new_lastName);
    msg = &message[POS_NEW_PHONE_NUM];
    strcpy(msg, data->new_phoneNumber);

    msg = &message[POS_USERNAME];
    strcpy(msg, data->username);
    msg = &message[POS_PSW];
    strcpy(msg, data->psw);

    msg = &message[POS_TOKEN];
    strncpy(msg, sessionTOKEN, TOKEN_LENGTH_);   
}


char * listContacts(char * contactList, int numContacts){
    int i = 1;
    char * contactString = malloc((57 * numContacts) * sizeof(char));
    sprintf(contactString, BLU"\n[LISTA CONTATTI]\n"RESET);
    do{
        char * list;
        list = &contactList[(i-1)*53];
        char * name, * lastName, * phoneNumber;
        char contacts[53];
        strncpy(contacts, list, 53);
        name = strtok(contacts, " ");
        lastName = strtok(NULL, " ");
        phoneNumber = strtok(NULL, "\n");

        char * tmp = malloc(57 * sizeof(char));
        sprintf(tmp, YEL"•"RESET" %s, %s, %s\n", name, lastName, phoneNumber);

        strcat(contactString, (const char *)tmp);

        i++;
    }while(i <= numContacts);

    return contactString;
}


int checkInsertedData(char * dataValue, char * typeOfData){
    
    char * nextString = malloc(200 * sizeof(char)); 
    nextString[0] = '\0'; 

    //case 1 --> it is name or last name
    if(strcmp(typeOfData, "name") == 0 || strcmp(typeOfData, "last name") == 0){
        if(strlen(dataValue) > 20){
            
            //filling the error string
            strcat(nextString, RED "        *ERROR: name must be max 20 chars*\n" RESET);
            if(checkAlphaNumeric(dataValue) == -1){
                strcat(nextString, (RED "        *ERROR: No special chars allowed*\n" RESET));
            }
            
            while(getchar() != '\n');
            intChoice = menu(logged, logged_options[logged], nextString);
            return -1;
        }
        if(checkAlphaNumeric(dataValue) == -1){
            
            strcat(nextString, RED "        *ERROR: No special chars allowed*\n" RESET);
            while(getchar() != '\n');
            intChoice = menu(logged, logged_options[logged], nextString);
            return -1;
        }
        return 0;
    }

    //case 2 --> it is phone number
    else if (strcmp(typeOfData, "phone number") == 0){
        if(strlen(dataValue) > 10){

            //filling the error string
            strcat(nextString, RED "        *ERROR: phone number must be max 10 chars*\n" RESET);
            if(checkNumber(dataValue) == -1){
                strcat(nextString, (RED "        *ERROR: only insert numbers!*\n" RESET));
            }

            while(getchar() != '\n');
            intChoice = menu(logged, logged_options[logged], nextString);
            return -1;
        }
        if(checkNumber(dataValue) == -1){
            
            strcat(nextString, RED "        *ERROR: only insert numbers!*\n" RESET);
            while(getchar() != '\n');
            intChoice = menu(logged, logged_options[logged], nextString);
            return -1;
        }
        return 0;
    }

    //case 3 --> it is username or password
    else if (strcmp(typeOfData, "username") == 0 || strcmp(typeOfData, "password") == 0){
        
        if(strlen(dataValue) > 20){
            
            strcat(nextString, RED "        *ERROR: username must be max 20 chars*\n" RESET);
            while(getchar() != '\n');
            intChoice = menu(logged, logged_options[logged], nextString);
            return -1;
        }
        if(strchr(dataValue, ' ')){
            
            strcat(nextString, RED "        *ERROR: no commas!*\n" RESET);
            while(getchar() != '\n');
            intChoice = menu(logged, logged_options[logged], nextString);
            return -1;
        }
        return 0;
    }

    return -2;    
}



/**
 * @brief Main function of the server.  Handles client connections, forks child processes, and processes client requests.
 *
 * @param argc Argument count.
 * @param argv Argument array.
 * @return 0 on successful termination, or a error code.
 */
int main(int argc, char *argv[]) {
    
    char * address = (strcmp(argv[1], "localhost") == 0) ? "127.0.0.1" : argv[1];
    int port = atoi(argv[2]);

    system("clear || cls");

    signal(SIGINT, sigintHandler);
    //signal(SIGPIPE, sigpipeHandler);

    //defining a space in memory for the token to be used for login and operations
    sessionTOKEN = malloc(TOKEN_LENGTH_ * sizeof(char));

    //connecting to server with users data passed on terminal line
    client_sock = connect_To_Server(address, port);

    //Defining a Message variable to contain user inserted data
    Message * data;
    data = malloc(sizeof(Message));
    
    //Defining a fixed size char array to fill it with the "data" values in known and fixed indexes
    char message [BUFFER_SIZE];

    struct sockaddr_in * serv_addr = buildSocketaddress(address, port);

    //While the socket is still working the program keeps running
    int val = 0;
    while(1){
        
        //using the choose operation function to build the data object values
        data = choose_operation();

        if(data->operation == LISTING){
            
            create_Message_String(message, data);
            
            //Sending the message to the server and checking if there has been some problem in the writing of data into the socket file
            val = write(client_sock, message, BUFFER_SIZE);
            if(val > 0){

                /*first we receive from the server the number of contacts we will be receiving
                so that we can prepare an adequate size array to contain all of those*/
                int32_t numContacts_t;

                //still checking eventual reading/writing problems...
                val = read(client_sock, &numContacts_t, sizeof(numContacts_t));
                if(val > 0){

                    int numContacts = ntohl(numContacts_t);
                    int size = sizeof(char) * numContacts * 53 + 1;
                   
                    char buffer[size];

                    val = read(client_sock, buffer, size);
                    if(val > 0){

                        //here we print the outcome of the operation, wheter it is positive or there has been some kind of server side error during operation
                        printOutcome(buffer[0]); 
                        if(numContacts > 0){

                            //printing menu on terminal
                            char * nextString = malloc((57 * numContacts + 1000) * sizeof(char));
                            nextString = listContacts(&buffer[1], numContacts);
        
                            printf("%s\n", nextString);

                            //asking if the user wants to save the results in a file
                            char c;
                            printf("Do you want to save these results in a file?[Y/n] ");
                            scanf("%c", &c);
                            
                            while (getchar() != '\n'); // Emptying the buffer since it happens to remain some residual input "\n"

                                                        
                            if(c == 'Y')
                            {
                                char * newResultsFilePath = malloc(100 * sizeof(char));
                                sprintf(newResultsFilePath, "%s%d.txt", RESULTS_PATH, fileNumCounter);
                                int res;
                                if((res = saveRecordsInAFile(newResultsFilePath, &buffer[1], numContacts, 3, 53, " ", "\n")) == POSITIVE){
                                    char * tmp = malloc(400 * sizeof(char));
                                    sprintf(tmp, GRN "\n\n                  RESULTS SAVED IN FILE" RESET" '%s%d.txt'\n\n" , RESULTS_PATH, fileNumCounter);
                                    strcat(nextString, tmp);                            
                                }
                                
                                fileNumCounter++;
                            }
                            
                            intChoice = menu(logged, logged_options[logged], nextString);
                            
                        }
                    }else{
                        printf(RED SERVER_DISCONNECTED_ERROR RESET);
                        tryConnection(client_sock, serv_addr);
                        continue;
                    }

                }else{
                    printf(RED SERVER_DISCONNECTED_ERROR RESET);
                    tryConnection(client_sock, serv_addr);
                    continue;
                }
            }
            else{
                printf(RED SERVER_DISCONNECTED_ERROR RESET);
                tryConnection(client_sock, serv_addr);
                continue;
            }
        }else{

            //creating message 
            create_Message_String(message, data);

            //Sending the message to the server and checking if there has been some problem in the writing of data into the socket file
            val = write(client_sock, message, BUFFER_SIZE);
            if(val <= 0){
                printf(RED SERVER_DISCONNECTED_ERROR RESET);
                tryConnection(client_sock, serv_addr);
                continue;
            }
            
            //checking if the op. is LOGIN or not
            if(data->operation == LOGIN){

                //defining a 33 chars array to contain 1 byte of outcome and 32 bytes for the eventual token
                char outcome[33];
                val = read(client_sock, outcome, sizeof(outcome));
                if(val <= 0){
                    
                    system("clear || cls");
                    printf(RED SERVER_DISCONNECTED_ERROR RESET);
                    tryConnection(client_sock, serv_addr);
                    continue;
                }
                else if(outcome[0] == POSITIVE && val > 0){
                    
                    //printing menu on terminal
                    if(outcome[0] == POSITIVE)
                        logged = 1;
                    else
                        logged = 0;
                    
                    //if the operation was completed successfully we write the login token for the current session into the sessionTOKEN variable
                    strcpy(sessionTOKEN, &outcome[1]); 

                    //and we modify the UI value for logged(or not) user
                    logged = 1;
                    printf("token: %s\n\n", sessionTOKEN);
                    
                    char * outcomeString = malloc(100 * sizeof(char));
                    outcomeString[0] = '\0';
                    strcat(outcomeString,getOutcomeString(outcome[0])); 
                    strcat(outcomeString,GRN"\ntoken: "RESET);
                    strcat(outcomeString, sessionTOKEN); 

                    while(getchar() != '\n');
                    intChoice = menu(logged, logged_options[logged], outcomeString);

                    free(outcomeString);

                    
                }
            }else{
                //doing the same thing for other operations
                int32_t outcome_t;
                val = read(client_sock, &outcome_t, sizeof(outcome_t));
                if(val <= 0){
                    system("clear || cls");
                    printf(RED SERVER_DISCONNECTED_ERROR RESET);
                    tryConnection(client_sock, serv_addr);
                    continue;
                }else{
                    int outcome = ntohl(outcome_t);
                    
                    //printing menu on terminal
                    if(data->operation != LOGOUT){ 

                        char * outcomeString = getOutcomeString(outcome); 
                        while(getchar() != '\n'); 
                        intChoice = menu(logged, logged_options[logged], outcomeString);
                    }else{

                        logged = 0;
                        intChoice = menu(logged, logged_options[logged], " ");

                    }   
                       
                }   
            }
            
        }

    }
}
