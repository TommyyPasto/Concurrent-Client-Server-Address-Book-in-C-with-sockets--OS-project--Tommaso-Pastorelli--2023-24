#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int ricercaContatto(FILE * contatti);

int main(){
    char buffer[200];
    int esito;
    FILE * contatti;
    contatti = fopen("contatti.txt", "r+");

    int lines = numeroDiContatti(contatti);
    printf("%d", lines);
    /*int nCharWritten = fprintf(contatti, "%s,%s,%s\n", "data->nome", "data->cognome", "data->numTelefono");
    if((strlen("data->nome")+strlen("data->cognome")+strlen("data->numTelefono")+3) != nCharWritten || nCharWritten == -1){
        printf("errore nella scrittura");
        esito = -1;
    }else{
        printf("scrittura avvenuta correttamente");
    }
    esito = nCharWritten;
    fclose(contatti);
    return nCharWritten;*/

    /* if(ricercaContatto(contatti)!=-1)
    {    
        printf("ciao");
        int nCharWritten = fprintf(contatti, "%s %s %s\n", "data->nome6", "data->cognome6", "data->numTelefono6");
        // controllo che la lunghezza corrisponda al contenuto mandato dal client
        
        if((strlen("data->nome6")+strlen("data->cognome6")+strlen("data->numTelefono6")+3) != nCharWritten || nCharWritten == -1){
            printf("errore nella fase di scrittura");
        }else{
            printf("operazione avvenuta correttamente");
        }
    } */
       
}

int ricercaContatto(FILE * contatti){
    char buffer[200];
    while(fgets(buffer, sizeof(buffer), contatti) != NULL){
        char * nome, * cognome, * numTelefono;
        nome = strtok(buffer, " ");
        cognome = strtok(NULL, " ");
        numTelefono = strtok(NULL, "\n");
        printf("%s,", nome);
        printf("%s,", cognome);
        printf("%s,", numTelefono);
        
        if(strcmp(nome, "tp") == 0 && strcmp(cognome, "pasto") == 0 && strcmp(numTelefono, "333") == 0)
            return fseek(contatti, -(strlen(nome)+strlen(cognome)+strlen(numTelefono)+3), SEEK_CUR);
    }
    return -1;
}

int numeroDiContatti(FILE * contatti){
    char buffer[53];
    int count = 0;
    for (char c = getc(contatti); c != EOF; c = getc(contatti))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;

    return count;
}

