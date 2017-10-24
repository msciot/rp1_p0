/* FICHERO: cliente_tcp.c
 * DESCRIPCION: codigo del cliente con sockets stream */

#include "common.h"

#define PUERTO_REMOTO PUERTO      /* puerto remoto en el servidor al que conecta el cliente */

int process_client_res(struct appdata response, char *aux);
int process_client_op(struct appdata *operation, char *op, char *arg_ftp);

int main (int argc, char *argv[])
{
        int sockfd;                                           /* conexion sobre sockfd */
        char buf[MAXDATASIZE];                                /* buffer de recepcion */
        char send_buff[MAXDATASIZE - ID_HEADER_LEN];          /* buffer de recepcion */
        char rop[READOP];
        char ftp_argv[MAXDATASIZE - HEADER_LEN];
        struct sockaddr_in their_addr;  /* informacion de la direccion del servidor */
        struct appdata operation;       /* mensaje de operacion enviado */
        struct appdata result;       /* mensaje de respuesta recibido */
        unsigned short op;
        int numbytes;                   /* numero de bytes recibidos o enviados */
        int len;
        int err;

        /* obtiene parametros */
        if (argc != 2)
        {
                fprintf (stderr, "uso: cliente hostname\n");
                exit (1);
        }

        /* crea el socket */
        if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror ("socket");
                exit (1);
        }

        their_addr.sin_family = AF_INET; /* Familia: ordenacion de bytes de la maquina */
        their_addr.sin_port = htons (PUERTO_REMOTO);    /* Puerto: ordenacion de bytes de la red */
        their_addr.sin_addr.s_addr =            /* IP: ordenacion de bytes de la red */
                inet_addr ( argv[1] );
        memset (&(their_addr.sin_zero), '\0', 8); /* Pone a cero el resto de la estructura */

        memset (buf, '\0', MAXDATASIZE); /* Pone a cero el buffer inicialmente */

        /* conecta con el servidor */
        if (connect (sockfd, (void*)&their_addr, sizeof (struct sockaddr_in)) == -1)
        {
                perror ("connect");
                exit (1);
        }

        printf ("(cliente) conexion establecida con servidor "
                "[nombre %s IP %s puerto remoto %d]\n",
                argv[1], inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port));

        while(1){

            memset (send_buff, '\0', MAXDATASIZE); /* Pone a cero el buffer inicialmente */


            // read stdin 4 any operation + args
            if ((len = read(0, send_buff, MAXDATASIZE - ID_HEADER_LEN)) == -1){
                perror("(cliente) error input");
                exit(1);
            }
            // will be deprecated
            if (send_buff[0] == 'q') break;

            //read from the stdin buffer, gets two pointers to char
            sscanf(send_buff,"%s %s", rop, ftp_argv);
            len = process_client_op(&operation ,rop, ftp_argv);
            if (len == -1)
                break;

            if ((numbytes = write (sockfd, (char *) &operation, len + HEADER_LEN)) == -1)
                    perror ("write");
            else
                    printf ("(cliente) mensaje enviado al servidor [longitud %d]\n", numbytes);

            printf ("(cliente) operacion solicitada [op 0x%x longitud %d contenido %s]\n",
                    ntohs(operation.op), len, operation.data);

            /* espera resultado de la operacion */
            if ((numbytes = read (sockfd, buf, HEADER_LEN)) == -1)  /* leemos tipo de respuesta y la longitud */
            {
                    perror ("read");
                    exit (1);
            }
            if (numbytes != HEADER_LEN) /* comprueba el número de bytes recibidos */
            {
                    printf ("(cliente) cabecera de la unidad de datos recibida de manera incompleta "
                            "[longitud esperada %d longitud recibida %d]",
                            HEADER_LEN, numbytes);
                    exit (1);
            }

            /* tenemos el tipo de respuesta y la longitud */
            result.op = ntohs((*((unsigned short *)(buf))));
            result.len = ntohs((*((unsigned short *)(buf + sizeof(unsigned short)))));
            memset (result.data, '\0', MAXDATASIZE - HEADER_LEN);


            if ((numbytes = read (sockfd, result.data, result.len)) == -1) /* leemos los datos */
            {
                    perror ("read");
                    exit (1);
            }
            printf ("(cliente) mensaje recibido del servidor [longitud %d]\n", numbytes + HEADER_LEN);


            if (numbytes != result.len) /* comprueba el número de bytes recibidos */
                    printf ("(cliente) datos de la unidad de datos recibida de manera incompleta "
                            "[longitud esperada %d longitud recibida %d]",
                            result.len, numbytes);
            else
                    printf ("(cliente) result de la operacion solicitada" 
                            "[res 0x%x longitud %d contenido %s]\n",
                            result.op, result.len, result.data);

            err = process_client_res(result, ftp_argv);
            if ( err == -1)
                break;

        }
        /* cierra el socket */
        close (sockfd);
        printf ("(cliente) conexion cerrada con servidor\n");

        return 0;
}

int process_client_op(struct appdata *operation, char *rop, char *arg_ftp){
    /* envia mensaje de operacion al servidor */
    char read_buff[MAXDATASIZE-HEADER_LEN];
    char buff[MAXDATASIZE-HEADER_LEN];
    unsigned short op = cdata_to_op(rop);
    int len = 0;
    switch (op)
    {
        case OP_GET:
            strcpy(operation->data, arg_ftp);  /* data */
            len = strlen (operation->data);
            break;
        case OP_PUT:
            len = read_file(arg_ftp, read_buff);
            if (len == -1)
                return len;
            arg_ftp[strlen(arg_ftp)] = ' ';
            strcat(arg_ftp, read_buff);
            strcpy(operation->data, arg_ftp);
            len = strlen(operation->data);
              /* print at most first three characters (safe) */
            break;
        case OP_RM:
            strcpy(operation->data, arg_ftp);  /* data */
            len = strlen (operation->data);
            break;
    }


    operation->data[len] = '\0';
    operation->op = htons(op);   /* op */
    operation->len = htons(len);  /* len */

    return len;
}

int process_client_res(struct appdata response, char *aux){
    int error = 0;
    switch (response.op)
    {
        case OP_RPUT: /* minusculas */
            pp("file uploaded!");
            break;
        case OP_RGET: /* mayusculas */
            error = write_file(aux, response.data, response.len);

            break;
        case OP_RM: /* mayusculas */
            pp("cliente rm");
            break;
        default: /* operacion desconocida */
            break;
    }
    return error;
}
