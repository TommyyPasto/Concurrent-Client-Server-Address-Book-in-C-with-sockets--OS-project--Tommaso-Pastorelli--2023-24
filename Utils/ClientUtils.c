#include "ClientUtils.h"

int checkFileOpened(char * pathname){
    int fd = open(pathname, O_WRONLY | O_EXCL);
    return fd;
}

void clear_last_n_lines(int n) {
    for (int i = 0; i < n; i++) {
        printf("\033[F"); // Sposta il cursore su di una riga
        printf("\033[K"); // Cancella la riga corrente
    }
}

//checks the presence of chars different from alpha-numeric ones in the string(0 if ok, -1 if not)
int checkAlphaNumeric(char * string){
    int length = strlen(string);
    for(int i = 0; i < length; i++){
        char ch = string[i];
        if(ch < 48 || (ch > 57 && ch < 65) || (ch > 90 && ch < 97) || ch > 122){
            return -1;
        }
    }
    return 0;
}

//check if the string is a number(0 if ok, -1 if not)
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

int saveRecordsInAFile(char * filepath, char * msg, int nOfRecords, int recordsLength){

    int outcome;
    FILE * resultsFile = fopen(filepath, "w+");

    //Connection to server
    for(int i = 0; i < nOfRecords; i++){
        char * name, * lastName, * phoneNumber;
        char contact[recordsLength];
        strncpy(contact, &msg[i * recordsLength], recordsLength);
        
        name = strtok(contact, " ");
        lastName = strtok(NULL, " ");
        phoneNumber = strtok(NULL, "\n");

        int nCharWritten = fprintf(resultsFile, "%s %s %s\n", name, lastName, phoneNumber);
        
        if(strlen(name) + strlen(lastName) + strlen(phoneNumber) + 3 != nCharWritten || nCharWritten == -1){
            printf(RED "            error during writing phase occurred\n\n" RESET);
            outcome = -1;
            break;
        }else{
            outcome = 0;
        }
    }
    fclose(resultsFile);
    
    return outcome;
}