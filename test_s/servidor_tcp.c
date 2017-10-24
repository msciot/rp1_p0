/* FICHERO: servidor_tcp.c
 * DESCRIPCION: codigo del servidor con sockets stream */

#include "common.h"

#define PUERTO_LOCAL PUERTO /* puerto local en el servidor al que se conectan los clientes */

int process_server_op(struct appdata operation, struct appdata *result);

int main (int argc, char* argv[])
{
        int sockfd;                             /* escucha sobre sock_fd */
        int new_fd;                             /* nueva conexion sobre new_fd */
        struct sockaddr_in my_addr;             /* informacion de mi direccion */
        struct sockaddr_in their_addr;          /* informacion de la direccion del cliente */
        char buf[MAXDATASIZE];                  /* buffer de recepcion */
        int numbytes;                           /* numero de bytes enviados o recibidos */
        struct appdata operation;               /* mensaje de operacion recibido */
        struct appdata result;               /* mensaje de respuesta enviado */
        size_t sin_size;
        int error;

        /* crea el socket */
        if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror ("socket");
                exit (1);
        }

        my_addr.sin_family = AF_INET;  /* Familia: ordenacion de bytes de la maquina */
        my_addr.sin_port = htons (PUERTO_LOCAL);/* Puerto: ordenacion de bytes de la red */
        my_addr.sin_addr.s_addr = INADDR_ANY;   /* IP: ordenacion de bytes de la red */
        memset (&(my_addr.sin_zero), '\0', 8);  /* Pone a cero el resto de la estructura */

        /* asigna el socket a un puerto local */
        if (bind (sockfd, (void*)&my_addr, sizeof (struct sockaddr_in)) == -1)
        {
                perror ("bind");
                exit (1);
        }

        /* escucha peticiones de conexion */
        if (listen (sockfd, BACKLOG) == -1)
        {
                perror ("listen");
                exit (1);
        }

        /* accept() loop...
         * this loop keeps waiting 4 connections*/
        while (1)
        {
                printf ("(server) Servicio FTP escuchando peticiones de conexion [puerto local %d]\n",
                        ntohs (my_addr.sin_port));

                /* acepta peticion de conexion de un cliente */
                sin_size = sizeof (struct sockaddr_in);
                if ((new_fd = accept (sockfd, (void*)&their_addr, &sin_size)) == -1)
                {
                        perror ("accept");
                        continue;
                }
                printf ("(server) conexion establecida desde cliente "
                        "[IP %s puerto remoto %d]\n",
                        inet_ntoa (their_addr.sin_addr), ntohs (their_addr.sin_port));

                //CLIENT IS CONNECTED
                while (1){

                    /* espera mensaje de operacion del cliente */
                    memset (buf, '\0', MAXDATASIZE);        /* Pone a cero el buffer inicialmente */
                    if ((numbytes = read (new_fd, buf, HEADER_LEN)) == -1)
                    {
                            perror ("read");
                            continue;
                    }
                    if (numbytes != HEADER_LEN) /* comprueba el número de bytes recibidos */
                    {
                            printf ("(server) cabecera de la unidad de datos recibida de manera incompleta "
                                    "[longitud esperada %ld longitud recibida %d]",
                                    HEADER_LEN, numbytes);
                            break;
                    }

                    /* tenemos el tipo de operacion y la longitud */
                    operation.op = ntohs((*((unsigned short *)(buf))));
                    operation.len = ntohs((*((unsigned short *)(buf + sizeof(unsigned short)))));
                    memset (operation.data, '\0', MAXDATASIZE - HEADER_LEN);

                    if ((numbytes = read (new_fd, operation.data, operation.len)) == -1)
                    {
                            perror ("read");
                            continue;
                    }
                    printf ("(servidor) mensaje recibido del cliente [longitud %ld]\n",
                            numbytes + HEADER_LEN);

                    if (numbytes != operation.len) /* comprueba el número de bytes recibidos */
                    {
                            printf ("(servidor) datos de la unidad de datos recibida de manera incompleta "
                                    "[longitud esperada %d longitud recibida %d]",
                                    operation.len, numbytes);
                            continue;
                    }
                    else
                            printf ("(servidor) operacion solicitada [op 0x%x longitud %d contenido %s]\n",
                                    operation.op, operation.len, operation.data);

                    /* realiza operacion solicitada por el cliente */
                    memset (result.data, '\0', MAXDATASIZE - HEADER_LEN);
                    error = process_server_op(operation, &result);
                    if (error == -1){
                        pp("cierro una conexion debido a que no he tenido tiempo a depurar y es lo mas easy");
                        pp("espero que el cliente sepa que no existe el archivo");
                        break;
                    }
                    /* envia resultado de la operacion solicitada por el cliente */
                    if ((numbytes = write (new_fd, (char *) &result, result.len + HEADER_LEN)) == -1)
                    {
                            perror ("write");
                            continue;
                    }
                    else
                            printf ("(servidor) mensaje enviado al cliente [longitud %d]\n", numbytes);

                    printf ("(servidor) resultado de la operacion solicitada"
                            "[res 0x%x longitud %d contenido %s]\n",
                            ntohs(result.op), result.len, result.data);
                }

                /* cierra socket */
                close (new_fd);
                printf ("(servidor) conexion cerrada con cliente\n");
        }
        /* cierra socket (no se ejecuta nunca) */
        close(sockfd);

        return 0;
}

int process_server_op(struct appdata operation, struct appdata *result){
    char buff[MAXDATASIZE-HEADER_LEN];
    int len;
    int error;
    char *name;

    switch (operation.op)
    {
        case OP_PUT: /* minusculas */
                result->op = htons(OP_RPUT); /* op */
                int n = dpos(operation.data);
                name = (char *)malloc(n);
                parse_name(operation.data, name, n);
                len = write_file(name, &(operation.data[++n]), operation.len - strlen(name));
                if (len == -1)
                    return len;
                len = 0;
                result->len = htons(len); /* len */
                error = 0;

                break;
        case OP_GET: /* mayusculas */
                result->op = htons(OP_RGET); /* op */
                len = read_file(operation.data, buff);
                if (len == -1)
                    return -1;
                printf("\n tengo esta longitud %d\n", len);
                strcpy(result->data, buff);
                result->len = htons(len); /* len */
                error = 0;
                break;
        case OP_RM: /* mayusculas */

                result->op = htons(OP_RES); /* op */
                len = 0;
                delete_file(operation.data);
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
