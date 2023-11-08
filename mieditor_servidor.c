#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>

int main() {
    /* Los datos que vamos a enviar a los clientes */
    char server_response[256] = "Conexión con el servidor exitosa";
    char actual_password[32] = "passwordhehehe";

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
    listen(server_socket, 5);

    /* Guardamos al socket del cliente, aceptando su conexión al servidor */
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    /* Enviamos la contraseña al socket client */
    send(client_socket, actual_password, sizeof(actual_password), 0);

    char new_file_name[256];
    read(client_socket, new_file_name, sizeof(new_file_name));
    printf("%s\n", new_file_name);

    /* Intentamos crear el nuevo archivo o abrirlo si ya existe */
    FILE *new_file;
    new_file = fopen(new_file_name, "a");

    char SUCC[] = "SUCC";
    char FAIL[] = "FAIL";

    if (new_file == NULL) {
        printf("Error: el archivo %s no pudo ser creado/abierto.", new_file_name);
        //send(client_socket, server_response, sizeof(server_response), 0);
        exit(EXIT_FAILURE);
    }

    /* Enviamos el mensaje al socket client */
    send(client_socket, server_response, sizeof(server_response), 0);

    char user_input_line[400];
    while (1) {
        read(client_socket, user_input_line, sizeof(user_input_line));
        if (strcmp(user_input_line, "EOF") == 0)
            break;
        printf("%s", user_input_line);
        fprintf(new_file, "%s", user_input_line);
    }

    /* Cerramos el archivo al que estamos escribiendo */
    fclose(new_file);

    /* Cerramos la conexión */
    close(server_socket);

    return 0;
}