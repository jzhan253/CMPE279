// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    if (argc == 1){
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int server_fd;
        int opt = 1;
        // ---------------- Setup socket Starts --------------------------- 
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
            perror("socket failed");
            exit(EXIT_FAILURE);
            }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            perror("setsockopt");
            exit(EXIT_FAILURE);
            }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        // ---------------- Setup socket Finished --------------------------- 
        printf("Forking a child process.\n");
        pid_t pid = fork();
        if(pid < 0 ){
            printf("ERROR in forking child\n");
            return -1;
        }else if(pid == 0 ){
            printf("Forked! Inside Child Process\n");
            struct passwd *pw = getpwnam("nobody");
            if (pw == NULL) {
                printf("ERROR in finding UID for name %s\n", nobodyUser);
                return -1;
            }
            int dropStatus = setuid(pw->pw_uid);

            if(dropStatus < 0){
                printf("ERROR in dropping privileges | dropStatus = %d\n", dropStatus);
                perror("Set UID Failed! Try use sudo to start the server!");
	            exit(EXIT_FAILURE);
            }
            printf("Child process dropped privileges\n" );

            char fd_arg [10];
            char port_arg [10];
            snprintf(fd_arg,10, "%d", server_fd);
            snprintf(port_arg,10,"%d",PORT);

            char *args[]={fd_arg, port_arg, NULL};
            // re-exec server child's process
            execv("./server",args);
            return 0;
        }
    }
    else{
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int new_socket, valread;
        int opt = 1;
        char buffer[1024] = {0};
        char *hello = "Woo from server";
        
        int server_fd = atoi(argv[0]);
        int port = atoi(argv[1]);

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = read(new_socket , buffer, 1024);
        printf("%s\n",buffer );
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");
    }
}
    