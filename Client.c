#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)//muestra los diversos mensajes de error que pueden ocurrir y cierra el cliente0
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int idSocket, numeroPuerto, n;
    struct sockaddr_in serv_addr;//aqui se guarda la direccion del server al que nos vamos a conectar
    struct hostent *server;//hostsent es una estructura de netdb.h

    char buffer[256];//buffer del mensaje que se va a enviar
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    numeroPuerto = atoi(argv[2]);//se consigue el numero de puerto del input
    idSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocket < 0)
        error("Error iniciando socket");
    server = gethostbyname(argv[1]);//con el nombre del host se obtiene el host,.
    if (server == NULL) {
        fprintf(stderr,"Error, server no existe\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));//mismo proposito que en el server
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(numeroPuerto);
    if (connect(idSocket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){//se llamma a connect para realizar una coneccion con el server el segundo argumento es el server
        error("No se pudo conectar");
      }
    printf("Ingrese su Usuario: ");
    bzero(buffer,256);//se reinicia el buffer antes de pedir el mensaje
    fgets(buffer,255,stdin);//se lee el mensaje
    char Mensaje[2] = "#";
    strcat(Mensaje,buffer);
    n = write(idSocket,Mensaje,strlen(Mensaje));//se le escribe al servidor el mensaje
    if (n < 0)
         error("Error al escribir al socket");
    bzero(buffer,256);
    n = read(idSocket,buffer,255);//se recibe respuesta de server
    if (n < 0)
         error("Error leyendo del socket");
    printf("%s\n",buffer);//se imprime la respuesta
    
    while(1){
      idSocket = socket(AF_INET, SOCK_STREAM, 0);
      if (connect(idSocket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){//se llamma a connect para realizar una coneccion con el server el segundo argumento es el server
          error("No se pudo conectar");
        }
      printf("Ingrese su Mensaje: ");
      bzero(buffer,256);//se reinicia el buffer antes de pedir el mensaje
      fgets(buffer,255,stdin);//se lee el mensaje
      n = write(idSocket,buffer,strlen(buffer));//se le escribe al servidor el mensaje
      if (n < 0)
           error("Error al escribir al socket");
      bzero(buffer,256);
      n = read(idSocket,buffer,255);//se recibe respuesta de server
      if (n < 0)
           error("Error leyendo del socket");
      printf("%s\n",buffer);//se imprime la respuesta
    }
    close(idSocket);
    return 0;
}
