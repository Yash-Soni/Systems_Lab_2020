#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <unistd.h>

void handleError(int, char*);

//function for communicating with the client over udp
void communicateWithClientOverUDP(int udp_sock_port){

    char message[512];
    struct sockaddr_in client_addr;           //stores the client ip address
    socklen_t len = sizeof(client_addr);      //stores length of client address of type socklen_t
    pid_t child_pid;
    if((child_pid = fork()) == 0){            //if a new client requests the server

        int server_udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
        handleError(server_udp_sock, "Unable to create UDP socket!");

        //Info about server getting filled
        struct sockaddr_in udp_server_addr;
        udp_server_addr.sin_family = AF_INET;
        udp_server_addr.sin_port = htons(udp_sock_port);    //binding the server to the randomly generated udp port
        udp_server_addr.sin_addr.s_addr = INADDR_ANY;

        //binding the server for Udp connection
        int b = bind(server_udp_sock, (struct sockaddr *) &udp_server_addr, sizeof(udp_server_addr));
        handleError(b, "Unable to bind udp socket!");

        //receive the client message
        int size = recvfrom(server_udp_sock, message, sizeof(message), 0, (struct sockaddr*) &client_addr, &len);

        bzero(message, sizeof(message));        //clears the message buffer

        sprintf(message, "Recieved your message");

        //sending the server's response to the client
        sendto(server_udp_sock, message, sizeof(message), 0, (struct sockaddr*) &client_addr, len);

        printf("Communication end with one client!!\n");
        close(server_udp_sock);     //closes the server created for this client once the communication gets completed
        exit(0);
    }

}

int main(int argc, char *argv[]){

    int server_sock, client_sock, port;
    char message[512];
    struct sockaddr_in server_addr, client_addr;
    int n;

    //validate input, check if the number of input arguments matches exactly as that of required
    if (argc < 2) {
        printf("Invalid Input");
        return 0;
    }
    printf("\nSERVER\n");
    printf("Waiting for clients to connect\n");

    server_sock = socket(AF_INET, SOCK_STREAM, 0);          //creates the server socket
    handleError(server_sock, "Unable to create Server socket!");

    bzero((char *) &server_addr, sizeof(server_addr));

    port = atoi(argv[1]);       //converts the manually enterred port number from string datatype to integer datatype

    //Filling the information about the server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);     //binds the server to the port enterred at the command line input

    //binding the TCP server
    n = bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    handleError(n, "Unable to bind server address!");

    //listen for clients through the socket
    listen(server_sock, SOMAXCONN);
    socklen_t len;

    while(1){

        len = sizeof(client_addr);      //stores the length of the client address of type socklen_t

        //accept client request
        client_sock = accept(server_sock, (struct sockaddr *) &client_addr, (socklen_t*)&len);

        handleError(client_sock, "Unable to accept TCP connection from client!");

        bzero(message, sizeof(message));        //clear previous content in the message buffer

        //read message sent by the client and store in message variable
        n = read(client_sock, message, sizeof(message));

        printf("\n<---------------------Connection Established with new Client--------------------->\n%s", message);

        handleError(n, "Unable to read data from client!");

        //generate random UDP port
        srand(time(0));
        int udp_sock_port = (rand() % 16000) + 1024;

        bzero(message, sizeof(message));

        sprintf(message, "%d", udp_sock_port);      //message that is to be sent to the client UDP port

        //send generated port to client
        n = write(client_sock, message, sizeof(message));
        handleError(n, "Unable to send generated PORT to client!");

        printf("UDP port number generated: %d\n", udp_sock_port);

        //calling helper function to work on the UDP communication
        communicateWithClientOverUDP(udp_sock_port);
    }

    //Closed TCP connection
    close(server_sock);

    return 0;
}

//function handleError used to display the error message on the terminal
void handleError(int flag, char* msg){
    if (flag < 0) {
        printf("%s\n", msg);
        exit(0);
    }
}