#include "ServerUtils.h"


struct flock * lockRD(FILE * stream, int start, int end, int cmd){

    if(end == EOF){
        //getting the end of file pointer value
        fseek(stream, 0, SEEK_END);
        end = ftell(stream);
    }

    //getting the file descr. of the add.book file.
    int fd = fileno(stream);
    
    struct flock * fl = malloc(sizeof(struct flock));
    fl->l_type = F_RDLCK;
    fl->l_whence = SEEK_SET;
    fl->l_start = start;
    fl->l_len = end;
    fl->l_pid = getpid();
    
    // If not able to lock the file exit, else proceed
    if (fcntl(fd, cmd, fl) == -1) 
    {
        perror("can't set lock\n\n");
        exit(1);
    }else{
        //printf("Process %d locked the file\n\n" , getpid());
    }

    return fl;
}



struct flock * lockWR(FILE * stream, int start, int end, int cmd){
    if(end == EOF){
        //getting the end of file pointer value
        fseek(stream, 0, SEEK_END);
        end = ftell(stream);
    }

    //getting the file descr. of the add.book file.
    int fd = fileno(stream);
    
    struct flock * fl = malloc(sizeof(struct flock));
    fl->l_type = F_WRLCK;
    fl->l_whence = SEEK_SET;
    fl->l_start = start;
    fl->l_len = end;
    fl->l_pid = getpid();
    
    // If not able to lock the file exit, else proceed
    if (fcntl(fd, cmd, fl) == -1) 
    {
        perror("can't set lock\n");
        fflush(stdout);
        exit(1);
    }else{
        //printf("Process %d locked the file exclusively\n" , getpid());
        fflush(stdout);
    }

    return fl;
}



int unlockFile(FILE * stream, struct flock * fl){
    if(fl->l_type != F_UNLCK){
        fl->l_type = F_UNLCK;
        //printf("file unlocked\n");
        fflush(stdout);
        return 0;
    }else{
        //printf("file was not locked\n");
        fflush(stdout);
        return -1;
    }
}



char * getCurrentTimeStr(){
    
    time_t rawtime;
	time ( &rawtime );
  	struct tm * timeinfo = localtime ( &rawtime );

	char * strTMP = asctime(timeinfo);
    int size = (int)strcspn(strTMP, "\n");
	
    //copying the time string without \n
    char * time_str = malloc((size+1) * sizeof(char));
	strncpy(time_str, strTMP, size);

	time_str[size] = '\0';

    return time_str;
}



unsigned char * convertToSHA256(char * str, unsigned char * hash)
{
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, strlen(str));
    SHA256_Final(hash, &sha256);
}



void to_hex(const unsigned char *hash, char *output, size_t length) {
    
    for (size_t i = 0; i < length; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[length * 2] = '\0';
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