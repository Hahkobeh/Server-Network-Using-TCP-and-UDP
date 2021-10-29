#include <iostream>     // For cerr and cout
#include <cstdlib>      // For atoi()
#include <sys/types.h>  // For data types
#include <sys/socket.h> // For socket(), connect(), send(), and recv()
#include <netdb.h>      // For gethostbyname()
#include <arpa/inet.h>  // For inet_addr()
#include <unistd.h>     // For close()
#include <netinet/in.h> // For sockaddr_in
#include <string.h>     // strcmp
#include <vector>
#include <string>
#include <sstream>


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

        int i = 0;
        char* char_ptr = buffer;
        string number_string;
        string convert_from;
        string convert_to;
        while(*char_ptr != '\0'){
            if(*char_ptr == '\r'){
                i++;
                char_ptr += 2;
                continue;
            }

            if(i==0){
                number_string.push_back(*char_ptr);
            }
            if(i==1){
                convert_from.push_back(*char_ptr);
            }
            if(i==2){
                convert_to.push_back(*char_ptr);
            }
            char_ptr++;
        }

        string symbol;
        double convert_ratio = 0;
        if(convert_from == "CAD"){
            if(convert_to == "USD"){
                convert_ratio = 0.79;
                symbol = "$";
            }
            if(convert_to == "EURO"){
                convert_ratio = 0.70;
                symbol = "€";
            }
            if(convert_to == "GBP"){
                convert_ratio = 0.59;
                symbol = "£";
            }
            if(convert_to == "BTC"){
                convert_ratio = 0.000013;
                symbol = "₿";
            }
            if(convert_to == "CAD"){
                convert_ratio = 1;
                symbol = '$';
            }
        }
        if(convert_from == "USD"){
            if(convert_to == "CAD"){
                convert_ratio = 1.23;
                symbol = "$";
            }
            if(convert_to == "GBP"){
                convert_ratio = 0.72;
                symbol = "£";
            }
            if(convert_to == "EURO"){
                convert_ratio = 0.86;
                symbol = "€";
            }
            if(convert_to == "BTC"){
                convert_ratio = 0.000016;
                symbol = "₿";
            }
            if(convert_to == "USD"){
                convert_ratio = 1;
                symbol = '$';
            }
        }

        stringstream ss(number_string);
        int number = 0;
        ss >> number;

        double final_number = static_cast<double>(number) * convert_ratio;
        string final_number_string = to_string(final_number);
        for(int i = 0; i < final_number_string.length(); i++){
            if(final_number_string.at(i) == '.'){
                final_number_string = final_number_string.substr(0, i + 3);
            }
        }




        string response = symbol + final_number_string + " " + convert_to+"\r\n";
        strcpy(buffer2,response.c_str());
        cout<<buffer2;


        sendto(micro_socket, buffer2, strlen(buffer2), 0, micro_info->ai_addr, micro_info->ai_addrlen);
    }
}
