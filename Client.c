#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <sys/ioctl.h>

#define Green "\x1B[32m"

void error(const char *msg)//muestra los diversos mensajes de error que pueden ocurrir y cierra el cliente0
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{

    int idSocket, numeroPuerto, n,forkID;
    struct sockaddr_in dirServer;//aqui se guarda la direccion del server al que nos vamos a conectar
    struct hostent *server;//hostsent es una estructura de netdb.h

    char mensaje[256];//mensaje del mensaje que se va a enviar
    struct winsize ventana;
    if (argc < 3) {
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
    bzero((char *) &dirServer, sizeof(dirServer));//mismo proposito que en el server
    dirServer.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&dirServer.sin_addr.s_addr,
         server->h_length);
    dirServer.sin_port = htons(numeroPuerto);
    if (connect(idSocket,(struct sockaddr *) &dirServer,sizeof(dirServer)) < 0){//se llamma a connect para realizar una coneccion con el server el segundo argumento es el server
        error("No se pudo conectar");
      }
    ioctl(STDOUT_FILENO,TIOCGWINSZ,&ventana);
    printf("Ingrese su Usuario: ");
    bzero(mensaje,256);//se reinicia el mensaje antes de pedir el mensaje
    fgets(mensaje,255,stdin);//se lee el mensaje
    char Mensaje[2] = "#";
    strcat(Mensaje,mensaje);
    n = send(idSocket,Mensaje,strlen(Mensaje),0);//se le escribe al servidor el mensaje
    if (n < 0)
         error("Error al escribir al socket");
    bzero(mensaje,256);
    n = recv(idSocket,mensaje,255,0);//se recibe respuesta de server
    if (n < 0)
         error("Error leyendo del socket");
    printf("%s\n",mensaje);//se imprime la respuesta
    if(strcmp(mensaje,"El usuario ya existe")==0){
      exit(0);
    }
    while(1){
      bzero(mensaje,256);//se reinicia el mensaje antes de pedir el mensaje
      printf("Ingrese su Mensaje con el formato Usuario:Mensaje ($S para salir):\n");
      fgets(mensaje,255,stdin);//se lee el mensaje
      n = send(idSocket,mensaje,strlen(mensaje),0);//se le escribe al servidor el mensaje
      if (n < 0)
           error("Error al escribir al socket");

      bzero(mensaje,256);
      forkID = fork();
      if(forkID<0){
        error("Fork Fallo");
      }
      if(forkID==0){
        while(1){
          n = recv(idSocket,mensaje,255,0);//se recibe respuesta de server
          if (n < 0)
               error("Error leyendo del socket");
          printf(Green "%*s\n",ventana.ws_col, mensaje);
          //printf("%s\n",mensaje);//se imprime la respuesta
          if(strstr(mensaje,"Adios")!=NULL){
            kill(forkID,SIGKILL);
            break;
          }
        }
      }

    }
    //close(idSocket);
    return 0;
}
