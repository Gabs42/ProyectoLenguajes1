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
  int idSocket;
  struct Cliente * Siguiente;
};
static int *contadorClientes = 0;
struct Cliente* Clientes;

struct Cliente encontrarCliente(int idSocketCliente){//encuentra en que posicion de la lista esta el cliente basado en el id de su socket
  printf("%i,%i\n",idSocketCliente,*(&Clientes->idSocket));
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

void error(const char *msg)//muestra los diversos mensajes de error y cierra el server
{
    perror(msg);
    exit(1);
}

void doStuff(int newIdSocket){
  char buffer[256];
  int n;
  bzero(buffer,256);//vacia el buffer donde se recibira el mensaje
  n = read(newIdSocket,buffer,255);//lee el mensaje del usuario
  if (n < 0){
      error("Error leyendo inf de socket");
  }
  if(buffer[0]=='#'){
    struct Cliente nuevoCliente;
    strcat(nuevoCliente.Usuario,buffer);
    nuevoCliente.idSocket = newIdSocket;
    *Clientes=nuevoCliente;
    *contadorClientes+=1;
    printf("Se registro el Cliente:%s",buffer);
  }
  else{
    printf("%i",*contadorClientes);
    struct Cliente posCliente = encontrarCliente(newIdSocket);
    printf("%s:%s\n",posCliente.Usuario,buffer);
  }

  n = write(newIdSocket,"Se recibio el msj",18);//se envia mensaje de exito al cliente
  if (n < 0){
      error("Error enviando inf a socket");
  }
  close(newIdSocket);

}

int main(int argc, char *argv[])
{
     contadorClientes = mmap(NULL, sizeof (* contadorClientes), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

     Clientes = (struct Cliente *)mmap(0,sizeof(struct Cliente), PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
     int numeroPuerto,idSocket, newIdSocket,forkID;
     socklen_t clientSize;
     char buffer[256];//aqui se guarda el mensaje que se recibe del cliente
     struct sockaddr_in serverAddres, cli_addr;//sockaddr_in es una strcutura de la netinite/in.h aqui se crean dos de ellas
     int n;
     if (argc < 2) {
         fprintf(stderr,"No se dio un puerto\n");
         exit(1);
     }
     idSocket = socket(AF_INET, SOCK_STREAM, 0);//socket() crea unnuevo socket AF_INET indica que se van a comunicar por internet,SOCK_STREAM es el tipo de socket que sera
     if (idSocket < 0){
       error("Error iniciando socket");
     }

     bzero((char *) &serverAddres, sizeof(serverAddres));//cambia el valor de serverAddres a 0
     numeroPuerto = atoi(argv[1]);//se toma el argumento dado de puerto y se pasa a int
     serverAddres.sin_family = AF_INET;//se le asigna las propiedades necesaries a serv_addres
     serverAddres.sin_addr.s_addr = INADDR_ANY;
     serverAddres.sin_port = htons(numeroPuerto);
     if (bind(idSocket, (struct sockaddr *) &serverAddres,sizeof(serverAddres)) < 0){//bind conecnta el socket
         error("Erro conectando con cliente");
     }


     listen(idSocket,5);//el sucket se frena y empieza a escuchar cualquier coneccion posible
     clientSize = sizeof(cli_addr);
     while(1){
     newIdSocket = accept(idSocket,(struct sockaddr *) &cli_addr,&clientSize);//bloquea el programa hasta que un cliente se conecte al server

     if (newIdSocket < 0){
         error("Error aceptando nuevo socket");
     }
     forkID = fork();
     if(forkID<0){
       error("Fork Fallo");
     }
     if(forkID==0){
       close(idSocket);
       doStuff(newIdSocket);
       exit(0);
     }
     else{
       close(newIdSocket);
     }
   }
   return 0;


}
