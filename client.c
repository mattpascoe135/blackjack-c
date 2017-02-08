#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[100], server_reply[200];
    bool connected = true;

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    //Connect to the server
    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }

    //Receive welcome message from server
    printf("Client started...\r\n\n");
    if(recv(sock, server_reply, 200, 0) < 0) {
	printf("Failed to receive data.\r\n");
	return 1;
    }
    printf("%s", server_reply);
    fflush(stdout);

    //Continue communiation with the server
    while(connected) {
        //Enter command
        printf("Enter command: ");
        scanf("%s", message);

        //Send the message
        if(send(sock, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 1;
        }

        //Receive reply and display
        if(recv(sock, server_reply, 200, 0) < 0) {
            puts("recv failed");
            break;
        }
        puts(server_reply);
    }

    close(sock);
    return 0;
}
