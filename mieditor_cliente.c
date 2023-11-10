#include "mieditor.h"  

int main(int argc, char *argv[]) {
    /* Definimos el mensaje del cliente */
    char new_file[32];

    /* Verificamos que el número de argumentos sea correcto. */
    if (argc == 2) {
        strcpy(new_file, argv[1]);
    } else {
        printf(KRED);
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
    //server_address.sin_addr.s_addr = "192.168.0.7"; //INADDR_ANY;
    inet_pton(AF_INET, "192.168.0.7", &(server_address.sin_addr));

    /* Establecer una conexión al servidor */
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    /* En caso de que se presente algún problema con la conexión, finalizamos la ejecución del programa. */
    if (connection_status != 0) {
        printf(KRED);
        printf("Error: La conexión no pudo ser establecida con el servidor.\n");
        exit(EXIT_FAILURE);
    }

    /* Recibimos la contraseña del servidor */
    char password[256];
    recv(network_socket, &password, sizeof(password), 0);

    /* Pedimos al usuario que teclee la contraseña para poder comunicarse con el servidor */
    int attempts = 0;
    char *prompt = "Por favor proporcione la contraseña: ";
    char *user_password;
    while (1) {
        user_password = getpass(prompt);
        if (strcmp(user_password, password) == 0) {
            break;
        } else {
            attempts++;
            if (attempts == 3)
                break;
            prompt = "La contraseña no es correcta. Intente de nuevo: ";
        }
    }

    if (attempts == 3) {
        printf(KRED);
        printf("\nDemasiados intentos. Cerrando conexión con el servidor.\n");
        close(network_socket);
        exit(EXIT_FAILURE);
    }

    printf("\nConexión con el servidor exitosa. Abriendo archivo %s...\n", new_file);
    fflush(stdin);

    /* Enviamos al servidor el nombre del nuevo archivo que queremos crear. */
    send(network_socket, new_file, sizeof(new_file), 0);

    /* Comenzamos a recibir información del servidor */
    char server_response[32];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    /* Mostramos los datos nos fueron enviados por el servidor */
    if (strcmp(server_response, "FAIL") == 0) {
        printf(KRED);
        printf("\nEl archivo \"%s\" no pudo ser creado/abierto. Cerrando conexión con el servidor.\n", new_file);
        exit(EXIT_FAILURE);
    }

    /* Sección para distinguir al editor del resto del programa */
    printf(KBLU);

    /* Mostramos el editor */
    printf("______  _______    ___________________________              \n");
    printf("___   |/  /__(_)   ___  ____/_____  /__(_)_  /______________\n");
    printf("__  /|_/ /__  /    __  __/  _  __  /__  /_  __/  __ \\_  ___/\n");
    printf("_  /  / / _  /     _  /___  / /_/ / _  / / /_ / /_/ /  /    \n");
    printf("/_/  /_/  /_/      /_____/  \\__,_/  /_/  \\__/ \\____//_/     \n");

    printf("\nPara cerrar el editor, presione la tecla Enter dos veces\n");

    printf(KGRN);
    printf("\n---------------------------- %s ----------------------------\n\n", new_file);

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

    printf("--------------------------------------------------------");
    for (int i = 0; i < strlen(new_file) + 2; i++)
        putchar('-');
    printf("\n");
        
    printf(KWHT);
    printf("\nEl contenido se guardó en el archivo \"%s\". Cerrando conexión.\n", new_file);

    /* Cerramos el socket para terminar la conexión */
    close(network_socket);

    return 0;
}