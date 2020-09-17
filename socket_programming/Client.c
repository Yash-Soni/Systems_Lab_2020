#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

void handleError(int, char*);

int main(int argc, char *argv[]){

    int client_sock, server_port, n;
    char message[512];
    struct sockaddr_in server_addr;
    struct hostent *server;

    //validate input, check if the number of input arguments matches exactly as that of required
    if (argc < 3) {
        printf("Invalid Input");
        return 0;
    }

    printf("\nCLIENT\n");

    server_port = atoi(argv[2]);        //converts the manually enterred port number from string datatype to integer datatype

    //creates the client socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    handleError(client_sock, "Unable to create socket");

    //populate configuration for server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(server_port);

    //connect to the server
    n = connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    handleError(n, "Unable to connect to the server");

    printf("Connection Established... \n");
    bzero(message, sizeof(message));
    sprintf(message, "UDP Port Request\n");         //Client requests for the server's UDP port

    printf("Message Type: 1, Length: %d, Message: %s", strlen(message), message);

    //send this message to the server
    n = write(client_sock, message, strlen(message));
    handleError(n, "Unable send request to the server");

    bzero(message, sizeof(message));        //clears the message buffer

    //read server response
    n = read(client_sock, message, sizeof(message));
    handleError(n, "Unable to receive server message");

    //prints message sent from the server
    printf("Server response\n");
    printf("Message Type: 2, Length: %d, Message: %s\n", strlen(message), message);

    close(client_sock);     //closes TCP socket

    //udp connection setup
    int client_udp_sock;
    int udp_sock_port = atoi(message), size;        //assigns the udp port received from the server
    socklen_t len;

    //Creating Socket for UDP
    client_udp_sock = socket(AF_INET, SOCK_DGRAM, 0);

    //populate server address configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(udp_sock_port);

    printf("\nPlease enter message:");
    bzero(message, sizeof(message));
    fgets(message, sizeof(message), stdin);         //reading message input from the command line to send it to the server

    printf("Message Type: 3, Length: %d, Message: %s\n", strlen(message), message);

    len = sizeof(server_addr);

    //Send message to server using UDP
    size = sendto(client_udp_sock, message, sizeof(message), 0, (struct sockaddr*) &server_addr, len);
    handleError(size, "Failed to send message");

    bzero(message, sizeof(message));//clear message content

    //Receive response from server
    size = recvfrom(client_udp_sock, message, sizeof(message), 0, (struct sockaddr*) &server_addr, &len);
    handleError(size, "Failed to receive message");

    printf("Server response\n");
    printf("Message Type: 4 Length: %d, Message: %s\n", strlen(message), message);

    printf("************Communication end with server!************\n");
    close(client_udp_sock);     //closes the clients UDP socket
    return 0;
}

//function handleError used to display the error message on the terminal
void handleError(int flag, char* msg){
    if (flag < 0) {
        printf("%s\n", msg);
        exit(0);
    }
}