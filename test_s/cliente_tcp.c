/* FICHERO: cliente_tcp.c
 * DESCRIPCION: codigo del cliente con sockets stream */

#include "common.h"

#define PUERTO_REMOTO PUERTO      /* puerto remoto en el servidor al que conecta el cliente */

int process_client_op(unsigned short op, struct appdata response, char *aux);

int main (int argc, char *argv[])
{
        int sockfd;                                           /* conexion sobre sockfd */
        char buf[MAXDATASIZE];                                /* buffer de recepcion */
        char send_buff[MAXDATASIZE - ID_HEADER_LEN];          /* buffer de recepcion */
        char rop[READOP];
        char ftp_argv[BYTEPARAM];
        struct sockaddr_in their_addr;  /* informacion de la direccion del servidor */
        struct appdata operation;       /* mensaje de operacion enviado */
        struct appdata result;       /* mensaje de respuesta recibido */
        unsigned short op;
        int numbytes;                   /* numero de bytes recibidos o enviados */
        int len;

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
            printf("processing: %s %s",rop, ftp_argv);


            /* envia mensaje de operacion al servidor */
            op = cdata_to_op(rop);
            operation.op = htons(op);   /* op */
            strcpy(operation.data, ftp_argv);  /* data */
            len = strlen (operation.data);
            operation.data[len] = '\0';
            operation.len = htons(len);  /* len */

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
            if (numbytes != HEADER_LEN) /* comprueba el n�mero de bytes recibidos */
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


            if (numbytes != result.len) /* comprueba el n�mero de bytes recibidos */
                    printf ("(cliente) datos de la unidad de datos recibida de manera incompleta "
                            "[longitud esperada %d longitud recibida %d]",
                            result.len, numbytes);
            else
                    printf ("(cliente) result de la operacion solicitada" 
                            "[res 0x%x longitud %d contenido %s]\n",
                            result.op, result.len, result.data);

            process_client_op(op, result, ftp_argv);
        }
        /* cierra el socket */
        close (sockfd);
        printf ("(cliente) conexion cerrada con servidor\n");

        return 0;
}

int process_client_op(unsigned short op, struct appdata response, char *aux){
    int error = 0;
    switch (op)
    {
        case OP_PUT: /* minusculas */
            pp("cliente put");
            break;
        case OP_GET: /* mayusculas */
            write_file(aux, response.data, response.len);

            break;
        case OP_RM: /* mayusculas */
            pp("cliente rm");
            break;
        default: /* operacion desconocida */
            break;
    }
    return error;
}
