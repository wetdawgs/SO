#include "mieditor.h"

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

    while (1) {
        fflush(stdin);
        printf(KBLU);
        listen(server_socket, 5);

        /* Guardamos al socket del cliente, aceptando su conexión al servidor */
        int client_socket;
        client_socket = accept(server_socket, NULL, NULL);
        /* log */ printf("\n[ log ] Conexión establecida con cliente %d.\n", client_socket);

        /* Enviamos la contraseña al socket client */
        send(client_socket, actual_password, sizeof(actual_password), 0);
        /* log */ printf("[ log ] Contraseña enviada a cliente.\n");

        char new_file_name[32];
        recv(client_socket, &new_file_name, sizeof(new_file_name), 0);
        if (strcmp(new_file_name, "") != 0) {
        /* log */ printf("[ log ] Nombre del nuevo archivo recibido con éxito: \"%s\".\n", new_file_name);
        } else {
            printf(KRED);
            /* log */ printf("[Error] El cliente no pudo establecer una conexión.\n");
            /* log */ printf("[Error] Cerrando conexión con el cliente %d.\n", client_socket);
            close(client_socket);
            //exit(EXIT_FAILURE);
            continue;
        }

        /* Intentamos crear el nuevo archivo o abrirlo si ya existe */
        FILE *new_file;
        new_file = fopen(new_file_name, "a");

        char SUCC[32] = "SUCC";
        char FAIL[32] = "FAIL";

        if (new_file == NULL) {
            printf(KRED);
            /* log */ printf("[Error] El archivo %s no pudo ser creado/abierto.\n", new_file_name);
            send(client_socket, FAIL, sizeof(FAIL), 0);
            close(client_socket);
            //exit(EXIT_FAILURE);
            /* log */ printf("[Error] Cerrando conexión con el cliente %d.\n", client_socket);
            continue;
        }

        /* Enviamos el mensaje de éxito al socket client */
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

        /* Cerramos el archivo */
        /* log */ printf("[ log ] El contenido se guardó en el archivo \"%s\".\n", new_file_name);

        /* Cerramos la conexión */
        /* log */ printf("[ log ] Fin de la comunicación con el cliente %d.\n", client_socket);
        close(client_socket);
    }

    /* Cerramos la conexión */
    /* log */ printf("[ log ] Fin de la comunicación.\n");
    close(server_socket);

    return 0;
}