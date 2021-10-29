#include <iostream>     // For cerr and cout
#include <cstdlib>      // For atoi()
#include <sys/types.h>  // For data types
#include <sys/socket.h> // For socket(), connect(), send(), and recv()
#include <netdb.h>      // For gethostbyname()
#include <arpa/inet.h>  // For inet_addr()
#include <unistd.h>     // For close()
#include <netinet/in.h> // For sockaddr_in
#include <string.h>     // strcmp

#define BUFSIZE 256
using namespace std;



/*
UDP Echo Server 
 */

int main(int argc, char const *argv[])
{

    addrinfo hints, *micro_info;

    char buffer[BUFSIZE];
    char buffer2[BUFSIZE];

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // for wildcard IP address
    hints.ai_protocol = 0;       // any protocol

    getaddrinfo(NULL, argv[1], &hints, &micro_info);

    // create a socket
    int micro_socket = socket(micro_info->ai_family, micro_info->ai_socktype, micro_info->ai_protocol);

    // bing to the socket
    int micro_bind = bind(micro_socket, micro_info->ai_addr, micro_info->ai_addrlen);
    if(micro_bind < 0 ){
        exit(EXIT_FAILURE);
    }
    for (;;)
    {
        // get clients IP address
        sockaddr_in *address = (sockaddr_in *)micro_info->ai_addr;
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(micro_info->ai_family, &(address->sin_addr), ip_str, INET_ADDRSTRLEN);
        cout << "Client IP Address: " << ip_str << endl;

        // receive from main server
        memset(buffer, 0, sizeof(buffer));
        memset(buffer2, 0, sizeof(buffer2));
        size_t recv_size = recvfrom(micro_socket, buffer, sizeof(buffer), 0, micro_info->ai_addr, &micro_info->ai_addrlen);
        if(recv_size < 0 ){
            exit(EXIT_FAILURE);
        }
        //print_buffer("Received", buffer, recv_size);
        std::cout << buffer;
        string received(buffer);
        if(received == "hello\r\n"){
            strcpy(buffer2,"bonjour\n");
        }else if(received.compare("goodbye\r\n") == 0){
            strcpy(buffer2,"au revoir\r\n");
        }else if(received.compare("eat\r\n") == 0){
            strcpy(buffer2,"manger\r\n");
        }else if(received.compare("walk\r\n") == 0){
            strcpy(buffer2,"marcher\r\n");
        }else if(received.compare("talk\r\n") == 0){
            strcpy(buffer2,"parler\r\n");
        }else if(received.compare("goodbye\r\n") == 0){
            strcpy(buffer2,"au revoir\r\n");
        }else if(received.compare("done\r\n") == 0){
            strcpy(buffer2,"closing\r\n");
        }else{
            strcpy(buffer2, "invalid input\n");
        }
        cout <<"sending: "<< buffer2;
        sendto(micro_socket, buffer2, strlen(buffer2), 0, micro_info->ai_addr, micro_info->ai_addrlen);
    }
}