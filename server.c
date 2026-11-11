#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "monitor.h"
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define PUERTO 6666

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

void atender_cliente(int client_socket)
{
    char buffer[1024];
    int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        perror("Error al recibir datos del cliente");
        return;
    }

    buffer[bytes_read] = '\0';
    printf("Mensaje recibido del cliente: %s\n", buffer);

    if (!validar_episodio(buffer)) {
        fprintf(stderr, "El codigo del episodio no es valido(ej. S01E01)\n");
        return;
    }

    char respuesta[1024];
    const Episodio *episodio = buscar_episodio(buffer);
    int bytes_formateados;

    if (episodio == NULL) {
        bytes_formateados = snprintf(respuesta, sizeof(respuesta),
                                    "Episodio no encontrado: %s\n", buffer);
    } else {
        bytes_formateados = formatear_episodio(episodio, respuesta, sizeof(respuesta));
    }

    if (bytes_formateados < 0 || bytes_formateados >= (int)sizeof(respuesta)) {
        fprintf(stderr, "Error al formatear la respuesta\n");
        return;
    }

    send(client_socket, respuesta, (size_t)bytes_formateados, 0);
}

void manejador_sigchld(int sig)
{
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}
int main(void)
{

        struct sigaction sa_int;
    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = SIG_IGN;
    sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("Error en sigaction SIGINT");
        exit(EXIT_FAILURE);
    }

        struct sigaction sa_chld;
    memset(&sa_chld, 0, sizeof(sa_chld));
    sa_chld.sa_handler = manejador_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("Error en sigaction SIGCHLD");
        exit(EXIT_FAILURE);
    }

    int server_socket, client_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in dir_servidor, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&dir_servidor, 0, sizeof(dir_servidor));
    dir_servidor.sin_family      = AF_INET;
    dir_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    dir_servidor.sin_port        = htons(PUERTO);

    if (bind(server_socket, (struct sockaddr *)&dir_servidor, sizeof(dir_servidor)) == -1) {
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

    for (;;) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error en accept");
            continue;
        }
        int tuberia[2];
        if (pipe(tuberia) == -1) {
            perror("Error en pipe");
            close(client_socket);
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("Error en fork");
            close(client_socket);
            close(tuberia[0]);
            close(tuberia[1]);
            continue;
        } else if (pid == 0) {
            close(server_socket);
            close(tuberia[0]);
            char *ip = inet_ntoa(client_addr.sin_addr);
            write(tuberia[1], ip, strlen(ip) + 1); 
            close(tuberia[1]);
            atender_cliente(client_socket);
            close(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket);
            close(tuberia[1]);
            char ip[INET_ADDRSTRLEN] = {0};
            read(tuberia[0], ip, sizeof(ip) - 1);
            close(tuberia[0]);  
            printf("Cliente %s atendido por el proceso hijo PID %d\n\n", ip, pid);
        }
    }

    close(server_socket);


    return 0;

}