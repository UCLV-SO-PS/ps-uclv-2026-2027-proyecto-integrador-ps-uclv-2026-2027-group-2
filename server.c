#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PUERTO 6666

int main (){

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1){
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in dir_servidor;
    memset(&dir_servidor, 0, sizeof(dir_servidor));
    dir_servidor.sin_family      = AF_INET;
    dir_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    dir_servidor.sin_port        = htons(PUERTO);

    if(bind(server_socket, (struct sockaddr *)&dir_servidor, sizeof(dir_servidor)) == -1){
        perror("Error en bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Error en listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto: %d\n", PUERTO);
    pause();







    close(server_socket);


    return 0;

}