/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include "IOHandler.h"


struct Cliente{
  char Usuario[50];
  int idSocketPers;
  struct sockaddr_in idSocket;
  struct Cliente * Siguiente;
};
static int *contadorClientes = 0;
struct Cliente* Clientes;

struct Cliente encontrarCliente(unsigned short idSocketCliente){//encuentra en que posicion de la lista esta el cliente basado en el id de su socket
  //printf("%i,%i\n",idSocketCliente,*(&Clientes->idSocket));
  int contador = 0;
  while(contador != *contadorClientes){
    if(Clientes[contador].idSocket.sin_port == idSocketCliente){
      return Clientes[contador];
    }
    contador++;

  }
  return Clientes[contador];
}

int ClienteRepetido(char * Usuario){//Busca si el usuario ya existe en la lista de clientes, retorna 1 si ya existe 0 si no
  int contador = 0;
  while(contador!= *contadorClientes){
    if(strcmp(Clientes[contador].Usuario,Usuario)==0){
      return 1;
    }
    contador++;
    }
    return 0;
  }


struct Cliente encontrarClienteUsuario(char * Usuario){//Busca si el usuario ya existe en la lista de clientes, retorna 1 si ya existe 0 si no
  int contador = 0;
  while(contador!= *contadorClientes){
    if(strstr(Clientes[contador].Usuario,Usuario)!=NULL){
      return Clientes[contador];
    }
    contador++;
    }
    return Clientes[contador];
  }



int elimiarCliente(char * Usuario){
  int contadorActual = 0;
  int contadorAnterior=0;
  while(contadorActual!= *contadorClientes){
    if(strcmp(Clientes[contadorActual].Usuario,Usuario)==0){
       strcpy(Clientes[contadorActual].Usuario,"");
    }
    contadorActual++;
  }
  return 0;
}
int insertarNuevoCliente(struct Cliente * ClInsertar){
  printf("New:%s\n",ClInsertar->Usuario);
  Clientes[*contadorClientes]=*ClInsertar;
  /*
  if(*contadorClientes==0){
    printf("%s\n", "Hai");
    Clientes[0] = *ClInsertar;
    return 1;
  }

  struct Cliente * clienteActual = Clientes;

  while(clienteActual->Siguiente != NULL){
    printf("%s\n","No");
    clienteActual = (clienteActual->Siguiente);
  }
  printf("%s\n","Si");
  //clienteActual->Siguiente = malloc(sizeof(struct Cliente));
  clienteActual->Siguiente = ClInsertar;
  printf("%s\n","Si2");
  */
  return 0;
}

int imprimirClientes(){
  //struct Cliente * clienteActual = Clientes;
  int contador = 0;
  while(contador!=*contadorClientes){
    printf("%s\n",Clientes[contador].Usuario);
    contador++;
  }
  // while(clienteActual->Siguiente != NULL){
  //   printf("%s\n",clienteActual->Usuario);
  //   clienteActual = (clienteActual->Siguiente);
  // }
  // printf("%s\n",clienteActual->Usuario);
  // return 0;
}

void error(const char *msg)//muestra los diversos mensajes de error y cierra el server
{
    perror(msg);
    exit(1);
}

void loopConeccion(int newIdSocket,struct sockaddr_in dirCliente){
  char respuesta[50];
  char mensaje[256];
  int n;
  bzero(mensaje,256);//vacia el mensaje donde se recibira el mensaje
  n = recv(newIdSocket,mensaje,255,0);//lee el mensaje del usuario
  if (n < 0){
      error("Error leyendo inf de socket");
  }
  printf("%i\n",newIdSocket);
  if(strcmp(mensaje,"$S\n")==0){
    printf("%s\n","Se entro a adios");
    strcat(respuesta,"Adios");
    printf("%s\n","adios1");
    struct Cliente ClienteAEliminar = encontrarCliente(dirCliente.sin_port);
    printf("%s\n","adios2");
    elimiarCliente(ClienteAEliminar.Usuario);
    printf("%s\n","adios3");
    *contadorClientes-=1;

  }
  else if(mensaje[0]=='#'){
    int resultado = ClienteRepetido(mensaje);
    if(resultado==1){
      printf("%s\n","Repetido");
      strcat(respuesta,"El usuario ya existe");
    }
    else{
      struct Cliente nuevoCliente={};
      strcpy(nuevoCliente.Usuario,mensaje);
      nuevoCliente.idSocket = dirCliente;
      nuevoCliente.idSocketPers = newIdSocket;
      //*Clientes=nuevoCliente;
      insertarNuevoCliente(&nuevoCliente);
      *contadorClientes+=1;
      //imprimirClientes();
      printf("%i\n",*contadorClientes);
      printf("Se registro el Cliente:%s",mensaje);
      strcat(respuesta,"Se recibio el msj");
    }
  }
  else{
    if(strchr(mensaje,':')!= NULL){
      socklen_t tamanioCliente;
      char * usuario = strtok(mensaje,":");
      //printf("%s\n",usuario);
      char * msj = strtok(NULL,":");
      //printf("%s\n",msj);
      struct Cliente ClienteAEnviar = encontrarClienteUsuario(usuario);
      struct Cliente ClienteEnviando = encontrarCliente(dirCliente.sin_port);
      printf("%s:%i\n",ClienteAEnviar.Usuario,ClienteAEnviar.idSocketPers);
      printf("New id Socket bef:%i\n",newIdSocket);
      newIdSocket = ClienteAEnviar.idSocketPers;
      printf("New id Socket af:%i\n",newIdSocket);
      strcat(respuesta,ClienteEnviando.Usuario);
      strcat(respuesta,msj);
    }
    else{
      printf("%s\n","Se entro a print");
      //printf("%i",*contadorClientes);
      struct Cliente posCliente = encontrarCliente(dirCliente.sin_port);
      printf("%s:%s\n",posCliente.Usuario,mensaje);
      strcat(respuesta,"Se recibio el msj");
    }
  }
  printf("Se va a enviar a:%i\n",newIdSocket );
  n = send(newIdSocket,respuesta,strlen(respuesta),0);//se envia mensaje de exito al cliente
  if (n < 0){
      error("Error enviando inf a socket");
  }
  bzero(respuesta,50);

}

int main(int argc, char *argv[])
{
     contadorClientes = mmap(NULL, sizeof(* contadorClientes), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

     Clientes = (struct Cliente *)mmap(NULL,20*sizeof(struct Cliente), PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
     strcpy(Clientes[0].Usuario,"PAas");
     //printf("%s\n",Clientes[0].Usuario);
     strcpy(Clientes[1].Usuario,"PAasasdas");
     int numeroPuerto,idSocket,forkID;
     socklen_t tamanioCliente;
     char mensaje[256];//aqui se guarda el mensaje que se recibe del cliente
     struct sockaddr_in dirServer, dirCliente;//sockaddr_in es una strcutura de la netinite/in.h aqui se crean dos de ellas
     int n;
     if (argc < 2) {
         fprintf(stderr,"No se dio un puerto\n");
         exit(1);
     }
     idSocket = socket(AF_INET, SOCK_STREAM, 0);//socket() crea unnuevo socket AF_INET indica que se van a comunicar por internet,SOCK_STREAM es el tipo de socket que sera
     if (idSocket < 0){
       error("Error iniciando socket");
     }

     bzero((char *) &dirServer, sizeof(dirServer));//cambia el valor de dirServer a 0
     //numeroPuerto = atoi(argv[1]);//se toma el argumento dado de puerto y se pasa a int
     numeroPuerto = readPortInt("server.conf");
     printf("Puerto de archivo: %d\n", numeroPuerto);
     dirServer.sin_family = AF_INET;//se le asigna las propiedades necesaries a serv_addres
     dirServer.sin_addr.s_addr = INADDR_ANY;
     dirServer.sin_port = htons(numeroPuerto);
     if (bind(idSocket, (struct sockaddr *) &dirServer,sizeof(dirServer)) < 0){//bind conecnta el socket
         error("Error conectando con cliente");
     }


     listen(idSocket,5);//el sucket se frena y empieza a escuchar cualquier coneccion posible
     tamanioCliente = sizeof(dirCliente);
     while(1){
     int  newIdSocket;
     newIdSocket = accept(idSocket,(struct sockaddr *) &dirCliente,&tamanioCliente);//bloquea el programa hasta que un cliente se conecte al server
     printf("%hu\n",newIdSocket);
     if (newIdSocket < 0){
         error("Error aceptando nuevo socket");
     }
     forkID = fork();
     if(forkID<0){
       error("Fork Fallo");
     }
     if(forkID==0){
       //close(idSocket);
       while(1){
         loopConeccion(newIdSocket,dirCliente);
       }

       exit(0);
     }
     else{
       //close(newIdSocket);
     }
   }
   return 0;


}
