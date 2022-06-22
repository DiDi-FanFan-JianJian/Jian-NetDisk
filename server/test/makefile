build: Client Server

Client: TCPServer.o TCPClient.o
	g++ -o client TCPServer.o TCPClient.o Client.cpp -lpthread

Server: TCPServer.o TCPClient.o
	g++ -o server TCPServer.o TCPClient.o Server.cpp -lpthread

TCPServer.o:
	g++ -c -o TCPServer.o TCPServer.cpp

TCPClient.o:
	g++ -c -o TCPClient.o TCPClient.cpp

clean:
	rm -f *.o server