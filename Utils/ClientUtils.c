#include "ClientUtils.h"


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
