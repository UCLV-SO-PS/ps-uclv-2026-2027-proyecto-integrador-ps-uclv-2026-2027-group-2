#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6666
#define IP_SERVIDOR "127.0.0.1"
#define BUFFER_SIZE 1024

int validar_episodio(const char *codigo)
{
    if (codigo == NULL || strlen(codigo) != 6)
        return 0;

    return codigo[0] == 'S' &&
        codigo[1] >= '0' && codigo[1] <= '9' &&
        codigo[2] >= '0' && codigo[2] <= '9' &&
        codigo[3] == 'E' &&
        codigo[4] >= '0' && codigo[4] <= '9' &&
        codigo[5] >= '0' && codigo[5] <= '9';
}

int main(int argc, char *argv[]){

    if(argc!=2){
        fprintf(stderr, "Debe insertar el codigo del episodio\n{uso: %s codigo de episodio }\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *cod_epis = argv[1];
    if(!validar_episodio(cod_epis)){
        fprintf(stderr, "El codigo del episodio no es valido(ej. S01E01)\n");
        exit(EXIT_FAILURE);
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(PORT); 
    server_addr.sin_addr.s_addr = inet_addr(IP_SERVIDOR); 


    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Eror al conectar con el servidor");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    if(send(client_socket, cod_epis, strlen(cod_epis), 0) < 0){
        perror("Error al enviar el codigo del episodio");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    int n = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error al recibir");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';
    printf("Respuesta del servidor: %s\n", buffer);
    close(client_socket);

    return 0;
}
