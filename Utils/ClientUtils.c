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


int saveRecordsInAFile(char * filepath, char * msg, int nOfRecords, int nOfFields, int recordsLength,  const char *__restrict__ delim, const char *__restrict__  endOfRecDelim){

    int outcome;
    FILE * resultsFile = fopen(filepath, "w+");

    //Connection to server
    for(int i = 0; i < nOfRecords; i++){
        char * fields[nOfRecords];
        char contact[recordsLength];
        strncpy(contact, &msg[i * recordsLength], recordsLength);

        int j = 0;
        for(j = 0; j < nOfFields-1; j++){
            fields[j] = (j == 0) ? strtok(contact, delim) : strtok(NULL, delim);
        }
        fields[j] = strtok(NULL, endOfRecDelim);


        int nCharWritten = 0;
        for(j = 0; j < nOfFields; j++){
            nCharWritten += (j < nOfFields-1) ? fprintf(resultsFile, "%s ", fields[j]) : fprintf(resultsFile, "%s", fields[j]);
        }
        nCharWritten += fprintf(resultsFile, "\n");
        
        if(nCharWritten == -1){
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
