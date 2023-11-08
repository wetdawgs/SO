#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    /* Definimos el mensaje del cliente */
    char *new_file;

    /* Verificamos que el número de argumentos sea correcto. */
    if (argc == 2) {
        new_file = argv[1];
    } else {
        printf("Error: el programa debe recibir exactamente un parámetro.\n");
        exit(EXIT_FAILURE);
    }

    /* Crea uh nuevo socket */
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* Especifica una dirección para el socket */
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(43210);
    server_address.sin_addr.s_addr = INADDR_ANY;

    /* Establecer una conexión al servidor */
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    /* En caso de que se presente algún problema con la conexión, finalizamos la ejecución del programa. */
    if (connection_status != 0) {
        printf("La conexión no pudo ser establecida con el servidor.");
        exit(EXIT_FAILURE);
    }

    /* Recibimos la contraseña del servidor */
    char password[256];
    recv(network_socket, &password, sizeof(password), 0);

    /* Pedimos al usuario que teclee la contraseña para poder comunicarse con el servidor */
    printf("Por favor proporcione la contraseña: ");
    int attempts = 0;
    char user_password[32];
    while (1) {
        scanf("%s", user_password);
        // printf("user pass: %s.\n", user_password);

        if (strcmp(user_password, password) == 0) {
            break;
        } else {
            attempts++;
            if (attempts == 3)
                break;
            printf("La contraseña no es correcta. Intente de nuevo: ");
        }
    }

    if (attempts == 3) {
        printf("Demasiados intentos. Cerrando conexión.\n");
        close(network_socket);
        exit(EXIT_FAILURE);
    }

    printf("Conexión con el servidor exitosa.\n");

    fflush(stdin);

    /* Enviamos al servidor el nombre del nuevo archivo que queremos crear. */
    send(network_socket, new_file, sizeof(new_file), 0);

    /* Comenzamos a recibir información del servidor */
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    /* Mostramos los datos nos fueron enviados por el servidor */
    printf("El servidor envió los datos: %s\n", server_response);

    //char* user_input_line;
    char user_input_line[256];
    char* eof = "EOF";
    while (1) {
        fgets(user_input_line, sizeof(user_input_line), stdin);
        if (strcmp(user_input_line, "\n") == 0) {
            send(network_socket, eof, sizeof(eof), 0);
            break;
        }
        send(network_socket, user_input_line, sizeof(user_input_line), 0);
    }

    /* Cerramos el socket para terminar la conexión */
    close(network_socket);

    return 0;
}