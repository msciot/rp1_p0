#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PUERTO 3490     /* puerto en el servidor */

#define BACKLOG 10      /* numero maximo de conexiones pendientes en cola */

#define MAXDATASIZE 1024 /* maximo numero de bytes que podemos recibir */
#define CHUNK 512

#define READOP 4
#define BYTEPARAM (MAXDATASIZE - HEADER_LEN) - READOP

#define HEADER_LEN (sizeof(unsigned short) * 2)

/* formato de la unidad de datos de aplicacion para Stream*/
struct appdata
{
        unsigned short op;                     /* codigo de operacion */
        unsigned short len;                    /* longitud de datos */
        char data[MAXDATASIZE - HEADER_LEN];   /* datos */
};

#define ID_HEADER_LEN (sizeof (unsigned short) * 3)

/* formato de la unidad de datos de aplicacion para Datagramas*/
struct idappdata
{
        unsigned short op;                        /* codigo de operacion */
        unsigned short id;                        /* identificador */
        unsigned short len;                       /* longitud de datos */
        char data[MAXDATASIZE - ID_HEADER_LEN];   /* datos */
};

/* codigos de operacion (appdata.op) */
#define OP_GET           0x0001  /* GET  */
#define OP_PUT           0x0002  /* PUT */
#define OP_RM            0x0003  /* RM */
#define OP_LS            0x0004  /* RM */
#define OP_RES           0x1000  /* RES */
#define OP_ERR           0xFFFF  /* ERR */

unsigned short cdata_to_op(char * cdata);
char *op_to_cdata(unsigned short op);
int process_op(struct appdata operation, struct appdata *result);
int read_file(char * file_name, char * output);
int write_file(char * file_name, char *data, int len);


/* HELPERS */
void pp(char * input);
