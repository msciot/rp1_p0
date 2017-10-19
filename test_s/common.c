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

unsigned short cdata_to_op(char * cdata)
{
    if (strcmp(cdata, "get") == 0) { // match!
        pp("get");
    }
    else if (strcmp(cdata, "put") == 0) { // not matche
        pp("put");
    }
    else if (strcmp(cdata, "rm") == 0) { // not matche
        pp("rm");
    }
    else if (strcmp(cdata, "ls") == 0) { // not matche
        puts("else");
    }

    return 0;
}
