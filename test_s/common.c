#include "common.h"


void pp(char * input){
    char *output[strlen(input) + 3];

    sprintf(output,"\n%s\n",input);
    printf("%s", output);
}

/**
 * Recepcion de n bytes
 */
int readn(int fd, void *vptr, size_t n)
{
        size_t nleft;
        ssize_t nread;
        char *ptr;

        ptr = vptr;
        nleft = n;
        while (nleft > 0) {
                if ( (nread = read(fd, ptr, nleft)) < 0)
                        return (-1);
                else if (nread == 0)
                        break;
                nleft -= nread;
                ptr += nread;
        }
        return (n - nleft);
}
int dpos(char *data){
    int len =  strlen(data);
    for (int i = 0; i < len; i++){
        if (data[i] == ' ')
            return i;
    }
    return 0;
}

void parse_name(char *buff, char *name, int n){
    for (int i = 0; i < n; i++){
        name[i] = buff[i];
    }
}

/**
 * Envio de n bytes
 */
int writen(int fd, const void *vptr, size_t n)
{
        size_t nleft;
        ssize_t nwritten;
        const char *ptr;

        ptr = vptr;
        nleft = n;
        while (nleft > 0) {
                if ( (nwritten = write(fd, ptr, nleft)) < 0) {
                        return (-1);
                }
                nleft -= nwritten;
                ptr += nwritten;
        }
        return (n);
}


/* FTP INTERFACE
 */
int read_file(char * file_name, char *output){
    char rbuf[CHUNK];
    FILE *file;
    size_t nread;
    int size = 0;

    file = fopen(file_name, "r");

    //read file using CHUNKS
    if (file != NULL) {
        while ((nread = fread(rbuf, 1, CHUNK, file)) > 0){
            //fwrite(rbuf, 1, nread, stdout);
            memcpy(output, rbuf, nread);
            output+=nread;
            size+=nread;
        }

        if (ferror(file))
            pp("ERROR READING FILE");
    }
    fclose(file);
    return size;
}


int write_file(char * file_name, char *data, int len){

    char wbuf[CHUNK];
    FILE *file;
    size_t nwrite;
    int size = 0;

    strcpy(wbuf, data);  /* data */
    file = fopen(file_name, "w");

    if (file != NULL){
        //while ((nwrite = fwrite(wbuf, 1, sizeof wbuf, file)) > 0){
        //    size+=nwrite;
        //}
        size = fwrite(wbuf, 1, len, file);

        if (ferror(file))
            pp("ERROR READING FILE");
    }
    fclose(file);
    return size;
}

int delete_file(char * file_name){
    int ret;
    ret = remove(file_name);

    if(ret == 0) {
        printf("File deleted successfully");
    }
    else {
        printf("Error: unable to delete the file");
    }

    return ret;
}


/**/



unsigned short cdata_to_op(char * cdata)
{
    unsigned short op;

    if (strcmp(cdata, "get") == 0) { // match!
        pp("performing get operation");
        op = OP_GET;
    }
    else if (strcmp(cdata, "put") == 0) { // not matche
        pp("performing put operation");
        op = OP_PUT;
    }
    else if (strcmp(cdata, "rm") == 0) { // not matche
        pp("performing rm operation");
        op = OP_RM;
    }
    else if (strcmp(cdata, "ls") == 0) { // not matche
        pp("performing ls operation");
        op = OP_LS;
    }
    else {
        pp ("error!");
        op= OP_ERR;
    }

    return op;
}
