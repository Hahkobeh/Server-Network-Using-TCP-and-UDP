to run micro servers

g++ translator.cpp -o translator
./translator <port 1>

g++ voting.cpp -o voting
./voting <port 2>

g++ currency_converter.cpp -o converter
./converter <port 3>

then on indirection server

g++ indirection_server.cpp -o indirser
./indirser <micro server ip> <tcp port> <port 1> <port 2> <port 3>

then telnet to connect
telnet <indirection server port> <tcp port>

so for example

on 136.159.5.27
g++ translator.cpp -o translator
./translator 9991

g++ voting.cpp -o voting
./voting 9992

g++ currency_converter.cpp -o converter
./converter 9993

on 136.159.5.25
g++ indirection_server.cpp -o indirser
./indirser 136.159.5.27 9990 9991 9992 9993

then on some other
telnet 136.159.5.25 9990

inputs are displayed in parentheses, input just the text, so for (exit) type:
exit

3 second timers for udp servers to respond

I was unable to test using both servers so it works for sure using only one ip but should work for both.
  
#assignment definition
Assignment 2: Transport Protocols (40 marks)
Due: Friday, October 22, 2021 (11:59pm)
Learning Objectives
The purpose of this assignment is to learn about client-server network applications, TCP and UDP protocols, and data representation. In particular, you will use a combination of TCP and UDP data transfer services to implement client-server programs (in C/C++) that provide different services to users.

Background Scenario
TCP (Transmission Control Protocol) and UDP (User Datagram Protocol) are transport-layer protocols used for sending data over the Internet. They both build on top of the Internet Protocol (IP). TCP is a connection-oriented protocol and UDP is a connection-less protocol. TCP establishes a connection between a sender and receiver before data can be sent. UDP does not establish a connection before sending data. TCP is a reliable data transfer protocol. TCP and UDP are not the only protocols that work on top of IP. However, they are the most widely used.

The modern Internet offers many different online services for users, such as language translation, currency conversion, mortgage calculators, time zone conversion, and many more. In fact, the current trend is toward service chaining of virtualized network functions, which we will see much later in the semester when we discuss software-defined networking.

There is an old saying in Computer Science that "any problem can be solved with another level of indirection". In this assignment, we will apply this principle by implementing some simple "micro-services" accessible from an "Indirection Server".

Your Task
Your primary task in this assignment is to build a network-based application that offers several different data transformation services to users. Each of these micro-services will run as its own micro-server, and you will use an "Indirection Server" to coordinate each session.

Client. There is only one Client program in this assignment. The client makes a TCP connection with the Indirection Server. The Client's interaction with the Indirection Server will involve connecting to the server (by getting Indirection Server's IP and Port number via terminal), and then entering a loop for interaction with the server (you shortly will see the list of interactions). Within the loop, the client can specify which service is desired.

Indirection Server. The Indirection Server uses TCP as its transport-layer protocol, for reliable data transfer with the client. The Indirection Server receives a command from the client and then communicates with the appropriate micro-service via UDP to perform the user's request, prior to returning the final result data back to the client via TCP. The client is able to request several services in a session. When the client is finished with all requests, the session with the Indirection Server ends.

Micro Servers. The micro-services will be offered via UDP-based communication, which is simple and fast, but unreliable. There will be three different micro-services running, each on a different port. The Indirection Server needs to know which services are running where (i.e., IP address and port), and send the data to the correct place for each data transformation request.

The specific data transformation micro-services that you need to implement are:

Translator Service: This micro-server translates one word from English to French. For example, the message "Hello" would become "Bonjour". Your micro-service should support at least 5 English words. For simplicity, you can ignore the French accent marks. You should choose the English words for your micro-service.

> Enter an English word: <user input>
> French translation: <user input translated to French>

Currency Converter Service: This micro-service receives three inputs from the user: 1) amount of money (an integer number), 2) the source currency, and 3) the destination currency. For example, it receives "$10 CAD US" from the user, and it converts 10 Canadian dollars to $7.90 US dollars. This micro-service supports converting the Canadian Dollar to US Dollar, Euro, British Pound, and Bitcoin currencies. You can use pre-defined reasonable values as an approximation of currency exchange rates (e.g., 0.79 for CAD to US dollars).

Voting: This micro-service performs three tasks.

Show Candidates. Your micro-server returns a list of names of the (fictional) candidates, along with their assigned ID number, upon the client's request. You can define at least four candidates, and each has a unique Integer ID number.

Secure voting. This service receives a user's vote and updates the voting result. Here is the voting scenario: first, a user requests to vote. Then the Indirection Server asks the voting micro-server for an encryption key. This encryption key is only one digit like '4'. The micro-service sends the key as a response to the Indirection Server, and the Indirection Server delivers that key to the client. Then the client machine encrypts the candidate's ID with the encryption key. We implement a simple encryption function. It multiplies the candidate's ID with the encryption key. Your system can encode each vote by [<vote> * <key>], and decode it by [<encoded vote> / <key>].
For example, if the encryption key is '4', and the candidate's ID is '3', the encrypted message is 12. Use the same key to decode the vote.
Then the Client sends the encrypted vote to the Indirection Server. The Indirection Server forwards it to the voting micro-server, and the micro-service decrypts the vote and updates the voting result.

Voting summary. It returns the voting results.

Notes.
- A Client can request results only after he/she votes.
- You should create an imaginary list of candidates with a pre-assigned number of votes.

Your Indirection Server should also do something reasonable if no response is received from one of the UDP-based micro-services within a certain time limit.

Testing
First, run all three micro-servers. Then run your Indirection Server. The Indirection Server should have the IP address and port numbers of the micro-servers. Run the Client program by using Indirection Server IP address and port number through the command line. Your Client program should make a TCP connection to the Indirection Server. Then your Client program shows a list of available services to the user. After receiving services, the Client program should be able to close the connection.
Grading Scheme
The grading scheme for the assignment is as follows:

10 marks for the design and implementation of the main TCP-based client-server solution to this problem. Your implementation should include proper use of TCP and UDP socket programming in C or C++, and reasonably commented code.
4 marks for a proper implementation of Translation Service.
4 marks for a proper implementation of Currency Converter Service.
8 marks for a proper implementation of the Voting Service.
4 marks for a clear and concise user manual (at most 1 page) that describes how to compile, configure, and use your solution. Also, describe where and how your testing was done (e.g., home, university, office), what works, and what does not. Be honest!
10 marks for a suitable demonstration of your Indirection Server to your TA in your tutorial section, or to your professor at a mutually convenient time. All demos will be done during the week of November 1st.
When you are finished, please submit your assignment solution via D2L. Your submission should include your source code and user manual documentation.

Bonus (optional): Up to 4 bonus marks are available if your voting system works based on universal time. For example, users are able to vote only before a specific time, and see the results only after that given time.

Tips
On a single machine, you can do your initial testing using the loopback interface.
For the micro-services, you can start by creating a simple echo micro-server. Once it is working, you can make the small changes required to create the other services. Test them individually.
Once you are testing on a real network interface, you may find Wireshark particularly useful to look at the network packets being sent and received by your application (i.e., ports, size, data content).
Please be cognizant of the data types that you are working with. For example, the messages that you send back and forth are likely to be ASCII strings, but some of the micro-services involve manipulating numerical values. Make sure you know how to convert between different data types when needed.

