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
