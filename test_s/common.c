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

int process_op(struct appdata operation, struct appdata *result){
    result->op = htons(OP_RES); /* op */
    int len;
    int error;

    switch (operation.op)
    {
        case OP_PUT: /* minusculas */

                len = 0;
                pp("estoy en put");
                result->len = htons(len); /* len */
                error = 0;

                break;
        case OP_GET: /* mayusculas */
                len = 0;
                pp("estoy en get");
                result->len = htons(len); /* len */
                error = 0;

                break;
        case OP_RM: /* mayusculas */

                len = 0;
                pp("estoy en rm");
                result->len = htons(len); /* len */
                error = 0;

                break;
        default: /* operacion desconocida */

                result->op = htons(OP_ERR); /* op */
                strcpy(result->data, "Operacion desconocida");  /* data */
                len = strlen (result->data);
                result->len = htons(len);  /* len */
                error = 1;

                break;
    }
    return error;
}

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
