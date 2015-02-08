/*	Pinochle Game Server
		Desktop Application
		Lucas Weisensee
		November 2014

	to START THE PROGRAM in command prompt::
	executableName PortNumber

	This program will function as a desktop game server for a network based pinochle game. It will:
	-Listen for new connections.
	-Manage incomming player connections.
		-Spinn off processes to handle new games.
		-Allow new players to connect to games in progress.
	-Keep track of games in progress.
	-Allow games to be added to database
	-Allow games to be requested from the database.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

//Error report management function
void error(char *er_msg)
{
  perror(er_msg);
  exit(1);
}

int main(int argc, char const *argv[])
{
	cout << "\nSetting up connection";

	//check argument length for port number
	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided");
		exit(1);
	}

	//initialize connection variables: 
	//socket file descriptors, port number, client address lenght and # of char read/written, respectively
	int sockfd, newsockfd, portnumber, clientlen, n;

	//initialize buffer
	char buffer[256];

	//server & client address structures
	struct sockaddr_in server_address, client_address;

	cout << "\ncreating to socket...";

	//get socket file descriptor
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	  error("Error: Socket didn't open correctly.");

	//reset buffer to zeros
	bzero((char *) &server_address, sizeof(server_address));

	//convert port number from argument 
	portnumber = atoi(argv[1]);

	//initiate server address
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;

	//initate port number with network byte order
	server_address.sin_port = htons(portnumber)

	//socket address to port number binding
	if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(serv_addr)) < 0)
	  error("ERROR on binding");

	cout << "\nsocket connected and bound";

	//listen for new connections with a total of 5 queued.
	listen(sockfd, 5);

	//Accept new connection
	clientlen = sizeof(client_address);
	newsockfd = accept(sockfd, (struct sockaddr *) &client_address, &clientlen);
	if (newsockfd < 0)
	  error("ERROR: accept not successful");

	cout << "\nconnection accepted.";

	//Reads from client(socket file)
	bzero(buffer,256);				//clear buffer
	n = read(newsockfd,buffer,255);	//read to buffer
	if (n < 0) 
		error("ERROR reading from socket");

	//prints message from client
	printf("Here is the message: %s", buffer);

	//Send answer to client
	n = write(newsockfd,"I got your message",18);
	if (n < 0)
		error("ERROR writing to socket");	//writes error if string not written successfully



	return 0;
}