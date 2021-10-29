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

#include <algorithm>

#define BUFSIZE 256
#define CONSTNUMENCRYPT 6
using namespace std;



/*
UDP Echo Server 
 */

vector<pair<const string,int>> voting_list;
vector<string> ip_list;

int main(int argc, char const *argv[])
{
    //vector<pair<const string,int>> voting_list;
    voting_list.emplace_back("Bob Davidson",56);
    voting_list.emplace_back("Jessica Jiraffe",43);
    voting_list.emplace_back("Colino Albainiano",44);
    voting_list.emplace_back("Dewey Drew",32);



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
    if(micro_socket < 0 ){
        cout<<"socket failed";
        exit(EXIT_FAILURE);
    }

    // bing to the socket
    int micro_bind = bind(micro_socket, micro_info->ai_addr, micro_info->ai_addrlen);
    if(micro_bind < 0 ){

        cout<<"bind failed";
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
        string key = "Multiply your candidates id by 5.\r\n";

        int id;
        if(received == "getkey"){

            if(std::find(ip_list.begin(), ip_list.end(), ip_str) != ip_list.end()) {
                strcpy(buffer2, "You have already voted! If you would like to see results type (results) otherwise press enter.\n");
            } else {
                strcpy(buffer2, key.c_str());
            }
        }else if(received == "getcandidates"){
            string candidate_list = "\nThe candidates are:\n" + voting_list.at(0).first + "(1), " + voting_list.at(1).first + "(2), "+ voting_list.at(2).first + "(3), and "+ voting_list.at(3).first + "(4)\r\n";
            strcpy(buffer2, candidate_list.c_str());
        }else if(received == "results"){
            string results = "The results for the election are:\n" + voting_list.at(0).first+ ": " + to_string(voting_list.at(0).second) + "\n"+ voting_list.at(1).first+ ": " + to_string(voting_list.at(1).second) + "\n"+ voting_list.at(2).first+ ": " + to_string(voting_list.at(2).second) + "\n" + voting_list.at(3).first+ ": " + to_string(voting_list.at(3).second) + "\r\n";
            strcpy(buffer2, results.c_str());
        }else {

            sscanf(buffer,"%d",&id);
            int temp = (id/5) -1;
            voting_list.at(temp).second++;
            string voted_for = "You voted for: " + voting_list.at(temp).first + "\nGood work!\nIf you would like to see the results of the vote type (results) otherwise press enter to continue.\n";
            strcpy(buffer2, voted_for.c_str());
            ip_list.push_back(ip_str);
        }
        sendto(micro_socket, buffer2, strlen(buffer2), 0, micro_info->ai_addr, micro_info->ai_addrlen);

    }
}