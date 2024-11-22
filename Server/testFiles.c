#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int ricercaContatto(FILE * contatti);

int main(){
    /* char buffer[200];
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
    system("clear || cls"); // Pulisce lo schermo (cross-platform)
    
    printf("*************************************************\n");
    printf("*                                               *\n");
    printf("*          \033[1;34mBENVENUTO NELLA RUBRICA\033[0m              *\n");
    printf("*                                               *\n");
    printf("*************************************************\n");
    printf("\n");
    printf("  \033[1;32mScegli un'operazione:\033[0m\n");
    printf("  ---------------------------------------------\n");
    printf("  \033[1;33m[1]\033[0m Aggiungi un nuovo contatto\n");
    printf("  \033[1;33m[2]\033[0m Modifica un contatto esistente\n");
    printf("  \033[1;33m[3]\033[0m Elimina un contatto\n");
    printf("  \033[1;33m[4]\033[0m Cerca un contatto\n");
    printf("  \033[1;33m[5]\033[0m Visualizza tutti i contatti\n");
    printf("  \033[1;33m[0]\033[0m Esci\n");
    printf("  ---------------------------------------------\n");
    printf("\n");
    printf("  \033[1;36mInserisci la tua scelta:\033[0m ");

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











    /* //la funzione ricercaContatto restituisce la posizione di fseek in cui è posizionato il file in caso servisse per qualche utilizzo
    if(ricercaContatto(contatti, data) != -1){
        printf("%s!%s!%s\n", data->new_nome, data->new_cognome, data->new_numTelefono);
        int nCharWritten = fprintf(contatti, "%s %s %s", data->new_nome, data->new_cognome, data->new_numTelefono);
        
        // controllo che la lunghezza corrisponda al contenuto mandato dal client
        if((strlen(data->new_nome)+strlen(data->new_cognome)+strlen(data->new_numTelefono)+2) != nCharWritten || nCharWritten == -1){
            printf("errore nella fase di scrittura");
            esito = ERROR_OCCURED;
        }else{
            printf("operazione avvenuta correttamente");
            esito = POSITIVE;
        }
    }else{
        esito = CONTACT_NOT_FOUND;
    } 
    fclose(contatti);*/