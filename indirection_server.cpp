#include <algorithm>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <vector>
#include <csignal>

#define PORT "9990"
#define BUFFERSIZE 1024

using namespace std;

void close_sockets(int);
string handle_client(string,bool);
int connect_to_micro_translator(const char **,addrinfo&,addrinfo*);

string communicate_with_translator(string, addrinfo*);
string communicate_with_converter(string, addrinfo*);
string communicate_with_voting_service(string, addrinfo*);




int server_socket, client_socket, converter_socket, translator_socket, voting_socket;




int main(int argc, const char *argv[]) {
    struct timeval tv;
    struct timeval tv1;
    struct timeval tv2;
    struct addrinfo server, *server_info, hints1, *server_info1, hints2, *server_info2, hints3, *server_info3;
    if (argc != 6) {
        fprintf(stderr,
                "usage: <UDP Server IP> <TCP Server Port> <UDP Translator Server Port> <UDP Converter Server Port> <UDP Voting Server Port>\n");
        exit(1);
    }
    /*const char *translator_info[] = {argv[1], argv[2]};
    if (connect_to_micro_translator(translator_info, hints1, server_info1) == -1) {
        fprintf(stderr, "translator socket creation failed!\n");
        exit(1);
    }*/
    //initialize translator +++++++++++++++++++++++++++++++++++++++++++++
    memset(&hints1, 0, sizeof hints1);
    hints1.ai_family = AF_INET; // set to AF_INET6 to use IPv6
    hints1.ai_socktype = SOCK_DGRAM;
    if ((getaddrinfo(argv[1], argv[3], &hints1, &server_info1)) != 0)
    {
        fprintf(stderr, "getaddrinfo() failed.\n");
        return -1;
    }

    // loop through all the results and make a socket
    translator_socket = socket(server_info1->ai_family, server_info1->ai_socktype, server_info1->ai_protocol);
    if (translator_socket < 0) {
        perror("socket creation failed translator");
        exit(EXIT_FAILURE);
    }
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    if(setsockopt(translator_socket,SOL_SOCKET,SO_RCVTIMEO, &tv,sizeof(tv)) < 0){
        perror("set sock opt failed translator");
        exit(EXIT_FAILURE);
    }

    //initialize currency converter ++++++++++++++++++++++++++++++++++++++++++++++++++++++
    memset(&hints2, 0, sizeof hints2);
    hints2.ai_family = AF_INET; // set to AF_INET6 to use IPv6
    hints2.ai_socktype = SOCK_DGRAM;
    if ((getaddrinfo(argv[1], argv[4], &hints2, &server_info2)) != 0)
    {
        fprintf(stderr, "getaddrinfo() failed.\n");
        return -1;
    }

    // loop through all the results and make a socket
    converter_socket = socket(server_info2->ai_family, server_info2->ai_socktype, server_info2->ai_protocol);
    if (converter_socket < 0) {
        perror("socket creation failed translator");
        exit(EXIT_FAILURE);
    }
    tv1.tv_sec = 3;
    tv1.tv_usec = 0;
    if(setsockopt(converter_socket,SOL_SOCKET,SO_RCVTIMEO, &tv1,sizeof(tv1)) < 0){
        perror("set sock opt failed translator");
        exit(EXIT_FAILURE);
    }

    //initialize voting service ++++++++++++++++++++++++++++++++++++++++++++++++++++++
    memset(&hints3, 0, sizeof hints3);
    hints3.ai_family = AF_INET; // set to AF_INET6 to use IPv6
    hints3.ai_socktype = SOCK_DGRAM;
    if ((getaddrinfo(argv[1], argv[5], &hints3, &server_info3)) != 0)
    {
        fprintf(stderr, "getaddrinfo() failed.\n");
        return -1;
    }

    // loop through all the results and make a socket
    voting_socket = socket(server_info3->ai_family, server_info3->ai_socktype, server_info3->ai_protocol);
    if (voting_socket < 0) {
        perror("socket creation failed translator");
        exit(EXIT_FAILURE);
    }
    tv2.tv_sec = 3;
    tv2.tv_usec = 0;
    if(setsockopt(voting_socket,SOL_SOCKET,SO_RCVTIMEO, &tv2,sizeof(tv2)) < 0){
        perror("set sock opt failed translator");
        exit(EXIT_FAILURE);
    }

    //initialize main server +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    int opt = 1;
    char buffer[BUFFERSIZE] = {0};
    char currency_buffer[BUFFERSIZE] = "test test curr buffer\0";
    //memset(&currency_buffer, 0, sizeof(currency_buffer));

    signal(SIGINT, close_sockets);
    signal(SIGTERM, close_sockets);


    memset(&server, 0, sizeof(addrinfo));

    server.ai_family = AF_INET;
    server.ai_socktype = SOCK_STREAM;
    server.ai_flags = AI_PASSIVE;
    int addrinfo = getaddrinfo(NULL, argv[2], &server, &server_info);
    if (addrinfo < 0) {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }


    server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (server_socket < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    int setsock_server = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (setsock_server < 0) {
        perror("setsock failed");
        exit(EXIT_FAILURE);
    }


    int bind_server = bind(server_socket, server_info->ai_addr, server_info->ai_addrlen);
    if (bind_server < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


    int listen_server = listen(server_socket, 5);
    if (listen_server < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    //currency converter
    //int opt = 1;

    signal(SIGINT, close_sockets);
    signal(SIGTERM, close_sockets);



    while (true) {


        client_socket = accept(server_socket, server_info->ai_addr, &server_info->ai_addrlen);
        if (client_socket < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        cout<<"connection established!"<<endl;
        while (true) {

            bool voted = false;
            string received = handle_client(
                    "you have connected to the indirection server :)\nPlease enter a number for whichever service you would like to access:\nTranslator(1), currency converter(2), or voting service(3). Or type (close) or (quit) to exit\n",false);

            cout <<"i got:"<< received<<endl;
            if (!received.compare("quit\r\n") || !received.compare("close\r\n")  ) {
                handle_client("exiting!\r\n", true);
                break;
            }
            if (received.compare("1\r\n") == 0) {
                while(true) {
                    received = handle_client(
                            "You have selected the translator.\nPlease enter a word to translate. Your options are: (hello), (goodbye), (eat), (walk), and (talk).\nType (done) to leave translator.\n\n",false);
                    if(received == "done\r\n"){
                        handle_client("Closing the currency converter! Press enter to continue.\r\n\n",true);
                        break;
                    }
                    received = communicate_with_translator(received, server_info1);

                    if(received == "closing\r\n") {
                        handle_client("Closing the translator!\r\n\n",true);
                        break;
                    }else{
                        handle_client(received,true);
                    }
                }


            } else if (received.compare("2\r\n") == 0) {
                while(true) {
                    received = handle_client(
                            "You have selected the currency converter.\nPlease enter an integer dollar amount or type (done) to leave the currency converter:\n",false);
                    if(received == "done\r\n"){
                        handle_client("Closing the currency converter! Press enter to continue.\r\n\n",true);
                        break;
                    }


                    received += handle_client(
                            "Please enter a currency to exchange from: (CAD) or (USD)\n",false);
                    received += handle_client(
                            "Please enter a currency to exchange to: (USD), (EURO), (GBP), (BTC), or (CAD)\n",false);

                    string exchanged_to = "The currency converter has converted your input into: " + communicate_with_converter(received, server_info2) + "\n\n";

                    handle_client(exchanged_to.c_str(),true);

                }


            } else if (received.compare("3\r\n") == 0) {
                handle_client("You have selected the voting service.\nIn this election there are 4 candidates!\n",true);
                while(true){
                    received = handle_client(
                            "For a list of candidates type (candidates).\nIf you would like to continue and vote type (vote), otherwise to exit the voting service type (done).\n",false);
                    if(received == "done\r\n"){
                        handle_client("Closing the voting service!\r\n\n",true);
                        break;
                    }else if(received == "candidates\r\n"){
                        received = communicate_with_voting_service("getcandidates",server_info3);
                        handle_client(received,true);
                        continue;
                    }else if(received == "vote\r\n"){
                        received = communicate_with_voting_service("getkey",server_info3);
                        if(received == "You have already voted! If you would like to see results type (results) otherwise press enter.\n"){
                            received = handle_client(received,false);

                        }else {
                            received = handle_client(received, false);
                            received = communicate_with_voting_service(received, server_info3);
                            received = handle_client(received, false);
                        }
                        if(received == "results\r\n"){
                            received = communicate_with_voting_service("results",server_info3);
                            handle_client(received,true);

                        }


                    }

                }


            } else {
                handle_client("Please enter either 1, 2 or 3\n", false);

            }


        }
        close(client_socket);


    }
}



string handle_client(string message_to_client,bool send_only) {

    char buffer[BUFFERSIZE];
    char buffer2[BUFFERSIZE];
    memset(buffer, 0, sizeof(buffer));
    memset(buffer2, 0, sizeof(buffer2));
    strcpy(buffer, message_to_client.c_str());
    if (!strcmp(buffer, "0"))
        return "\0";
    //char welcome_message[] = "you have connected to the indirection server :)\nPlease enter a number for whichever service you would like to access:\ncurrency converter(1), translator(2), or voting service(3)\n";
    send(client_socket,buffer,sizeof(buffer),0);
    if(!send_only) {
        recv(client_socket, buffer2, sizeof(buffer2), 0);
    }

    string return_val(buffer2);
    return return_val;

}




string communicate_with_translator(string tosend,addrinfo* server_info1){
    size_t numbytes;
    sockaddr_in *address = (sockaddr_in *)server_info1->ai_addr;
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(server_info1->ai_family, &(address->sin_addr), ip_str, INET_ADDRSTRLEN);
    cout << "Client IP Address: " << ip_str << endl;


    char buffer3[BUFFERSIZE];
    memset(buffer3, 0, sizeof(buffer3));
    char buffer4[BUFFERSIZE];
    memset(buffer4, 0, sizeof(buffer4));
    strcpy(buffer3, tosend.c_str());
    if (!strcmp(buffer3, "0"))
        return "\0";
    cout<<"sending "<<buffer3<<endl;


    if ((numbytes = sendto(translator_socket, buffer3, strlen(buffer3), 0, server_info1->ai_addr, server_info1->ai_addrlen)) == -1)
    {
        perror("sendto() failed");
        exit(1);
    }

    //printf("sent %d bytes to %s\n", numbytes, ip_str);
    int n = recvfrom(translator_socket,buffer4,sizeof(buffer4),0,server_info1->ai_addr,&server_info1->ai_addrlen);
    cout << buffer4;
    string return_val(buffer4);
    return return_val;



}

string communicate_with_converter(string tosend,addrinfo* server_info2){
    size_t numbytes;
    sockaddr_in *address = (sockaddr_in *)server_info2->ai_addr;
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(server_info2->ai_family, &(address->sin_addr), ip_str, INET_ADDRSTRLEN);
    cout << "Client IP Address: " << ip_str << endl;


    char buffer3[BUFFERSIZE];
    memset(buffer3, 0, sizeof(buffer3));
    char buffer4[BUFFERSIZE];
    memset(buffer4, 0, sizeof(buffer4));
    strcpy(buffer3, tosend.c_str());
    if (!strcmp(buffer3, "0"))
        return "\0";
    cout<<"sending "<<buffer3<<endl;


    if ((numbytes = sendto(translator_socket, buffer3, strlen(buffer3), 0, server_info2->ai_addr, server_info2->ai_addrlen)) == -1)
    {
        perror("sendto() failed");
        exit(1);
    }

    //printf("sent %d bytes to %s\n", numbytes, ip_str);
    int n = recvfrom(translator_socket,buffer4,sizeof(buffer4),0,server_info2->ai_addr,&server_info2->ai_addrlen);
    cout << buffer4;
    string return_val(buffer4);
    return return_val;



}
string communicate_with_voting_service(string tosend,addrinfo* server_info3){
    size_t numbytes;
    sockaddr_in *address = (sockaddr_in *)server_info3->ai_addr;
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(server_info3->ai_family, &(address->sin_addr), ip_str, INET_ADDRSTRLEN);
    cout << "Client IP Address: " << ip_str << endl;


    char buffer3[BUFFERSIZE];
    memset(buffer3, 0, sizeof(buffer3));
    char buffer4[BUFFERSIZE];
    memset(buffer4, 0, sizeof(buffer4));
    strcpy(buffer3, tosend.c_str());
    if (!strcmp(buffer3, "0"))
        return "\0";
    cout<<"sending "<<buffer3<<endl;


    if ((numbytes = sendto(translator_socket, buffer3, strlen(buffer3), 0, server_info3->ai_addr, server_info3->ai_addrlen)) == -1)
    {
        perror("sendto() failed");
        exit(1);
    }

    //printf("sent %d bytes to %s\n", numbytes, ip_str);
    int n = recvfrom(translator_socket,buffer4,sizeof(buffer4),0,server_info3->ai_addr,&server_info3->ai_addrlen);
    cout << buffer4;
    string return_val(buffer4);
    return return_val;



}



void close_sockets(int signal){
    cout << "CLOSED\n";
    close(server_socket);
    close(client_socket);
    close(translator_socket);
    close(converter_socket);
    close(voting_socket);


    exit(signal);
}