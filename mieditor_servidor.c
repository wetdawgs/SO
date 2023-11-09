#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define KBLU  "\x1B[34m"
#define KRED  "\x1B[31m"

int main() {
    /* Los datos que vamos a enviar a los clientes */
    char server_response[256] = "Conexión con el servidor exitosa";
    char actual_password[32] = "passwordhehehe";

    printf(KBLU);

    /* Creamos el socket para la conexión */
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* Especificamos la dirección para el socket */
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(43210);
    server_address.sin_addr.s_addr = INADDR_ANY;

    /* Unimos al socket a la IP especificada */
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    /* Escuchamos por conexiones al servidor */
    /* log */ printf("[ log ] Servidor inicializado. Escuchando por clientes...\n");
    listen(server_socket, 5);

    /* Guardamos al socket del cliente, aceptando su conexión al servidor */
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);
    /* log */ printf("[ log ] Conexión establecida con cliente.\n");

    /* Enviamos la contraseña al socket client */
    send(client_socket, actual_password, sizeof(actual_password), 0);
    /* log */ printf("[ log ] Contraseña enviada a cliente.\n");

    char new_file_name[32];
    recv(client_socket, &new_file_name, sizeof(new_file_name), 0);
    /* log */ printf("[ log ] Nombre del nuevo archivo recibido con éxito: \"%s\".\n", new_file_name);

    /* Intentamos crear el nuevo archivo o abrirlo si ya existe */
    FILE *new_file;
    new_file = fopen(new_file_name, "a");

    char SUCC[32] = "SUCC";
    char FAIL[32] = "FAIL";

    if (new_file == NULL) {
        printf(KRED);
        printf("[Error] El archivo %s no pudo ser creado/abierto.\n", new_file_name);
        send(client_socket, FAIL, sizeof(FAIL), 0);
        exit(EXIT_FAILURE);
    }

    /* Enviamos el mensaje al socket client */
    send(client_socket, SUCC, sizeof(SUCC), 0);

    char user_input_line[400];
    while (1) {
        read(client_socket, user_input_line, sizeof(user_input_line));
        if (strcmp(user_input_line, "EOF") == 0)
            break;
        fprintf(new_file, "%s", user_input_line);
    }

    /* Cerramos el archivo al que estamos escribiendo */
    fclose(new_file);

    /* Cerramos la conexión */
    /* log */ printf("[ log ] El contenido se guardó en el archivo \"%s\".\n", new_file_name);
    /* log */ printf("[ log ] Fin de la comunicación.\n");
    close(server_socket);

    return 0;
}