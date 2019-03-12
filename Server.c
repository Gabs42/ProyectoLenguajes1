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


struct Cliente{
  char Usuario[50];
  unsigned short idSocket;
  struct Cliente * Siguiente;
};
static int *contadorClientes = 0;
struct Cliente* Clientes;

struct Cliente encontrarCliente(unsigned short idSocketCliente){//encuentra en que posicion de la lista esta el cliente basado en el id de su socket
  //printf("%i,%i\n",idSocketCliente,*(&Clientes->idSocket));
  struct Cliente clienteActual = *Clientes;
  int contador = 0;
  while(contador != *contadorClientes){
    if(clienteActual.idSocket == idSocketCliente){
      return clienteActual;
    }
    contador++;
    clienteActual = *(clienteActual.Siguiente);

  }
  return clienteActual;
}

int ClienteRepetido(char * Usuario){//Busca si el usuario ya existe en la lista de clientes, retorna 1 si ya existe 0 si no
  int contador = 0;
  struct Cliente clienteActual = *Clientes;
  while(contador!= *contadorClientes){
    if(strcmp(clienteActual.Usuario,Usuario)==0){
      return 1;
    }
    contador++;
    if((clienteActual.Siguiente) == NULL){
      return 0;
    }
    clienteActual = *(clienteActual.Siguiente);
  }
  return 0;
}

int elimiarCliente(char * Usuario){
  int contador = 0;
  struct Cliente * clienteActual = Clientes;
  struct Cliente * clienteAnterior = Clientes;
  while(contador!= *contadorClientes){
    if(strcmp(clienteActual->Usuario,Usuario)==0){
       if(contador==0){
         strcpy( Clientes->Usuario,"");
         return 1;
       }
       else{
         clienteAnterior->Siguiente = (clienteActual->Siguiente);
       }
    }
    contador++;
    clienteAnterior = clienteActual;
    clienteActual = (clienteActual->Siguiente);

  }
  return 0;
}
int insertarNuevoCliente(struct Cliente ClInsertar){
  if(*contadorClientes==0){
    *Clientes = ClInsertar;
    return 1;
  }

  int contador = 1;
  struct Cliente * clienteActual = Clientes;

  while(clienteActual->Siguiente != NULL){

    clienteActual = (clienteActual->Siguiente);
  }
  printf("%s\n","Si");
  clienteActual->Siguiente = malloc(sizeof(struct Cliente));
  strcpy(clienteActual->Siguiente->Usuario,ClInsertar.Usuario);
  clienteActual->Siguiente->idSocket = ClInsertar.idSocket;
  clienteActual->Siguiente->Siguiente= NULL;
  printf("%s\n","Si2");
  return 0;
}

int imprimirClientes(){
  struct Cliente * clienteActual = Clientes;

  while(clienteActual->Siguiente != NULL){
    printf("%s\n",clienteActual->Usuario);
    clienteActual = (clienteActual->Siguiente);
  }
  printf("%s\n",clienteActual->Usuario);
  return 0;
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
  printf("%s\n",mensaje);
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
      struct Cliente nuevoCliente;
      strcat(nuevoCliente.Usuario,mensaje);
      nuevoCliente.idSocket = dirCliente.sin_port;
      //*Clientes=nuevoCliente;
      insertarNuevoCliente(nuevoCliente);
      //imprimirClientes();
      *contadorClientes+=1;
      printf("Se registro el Cliente:%s",mensaje);
      strcat(respuesta,"Se recibio el msj");
    }
  }
  else{
    printf("%s\n","Se entro a print");
    //printf("%i",*contadorClientes);
    struct Cliente posCliente = encontrarCliente(dirCliente.sin_port);
    printf("%s:%s\n",posCliente.Usuario,mensaje);
  }

  n = send(newIdSocket,respuesta,strlen(respuesta),0);//se envia mensaje de exito al cliente
  if (n < 0){
      error("Error enviando inf a socket");
  }

}

int main(int argc, char *argv[])
{
     contadorClientes = mmap(NULL, sizeof (* contadorClientes), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

     Clientes = (struct Cliente *)mmap(NULL,sizeof(struct Cliente), PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

     int numeroPuerto,idSocket, newIdSocket,forkID;
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
     numeroPuerto = atoi(argv[1]);//se toma el argumento dado de puerto y se pasa a int
     dirServer.sin_family = AF_INET;//se le asigna las propiedades necesaries a serv_addres
     dirServer.sin_addr.s_addr = INADDR_ANY;
     dirServer.sin_port = htons(numeroPuerto);
     if (bind(idSocket, (struct sockaddr *) &dirServer,sizeof(dirServer)) < 0){//bind conecnta el socket
         error("Error conectando con cliente");
     }


     listen(idSocket,5);//el sucket se frena y empieza a escuchar cualquier coneccion posible
     tamanioCliente = sizeof(dirCliente);
     while(1){
     newIdSocket = accept(idSocket,(struct sockaddr *) &dirCliente,&tamanioCliente);//bloquea el programa hasta que un cliente se conecte al server
     printf("%hu\n",dirCliente.sin_port);
     if (newIdSocket < 0){
         error("Error aceptando nuevo socket");
     }
     forkID = fork();
     if(forkID<0){
       error("Fork Fallo");
     }
     if(forkID==0){
       close(idSocket);
       while(1){
         loopConeccion(newIdSocket,dirCliente);
       }

       exit(0);
     }
     else{
       close(newIdSocket);
     }
   }
   return 0;


}
