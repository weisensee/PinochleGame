/*	Testing Client -- Pinochle Game Server
		Desktop Application
		Lucas Weisensee
		November 2014

	This program will function as a testing client for the pinochle game server. 
	It will allow a developer to run a series of testing functions locally.
*/

using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>	//necessary?

#pragma comment(lib, "Ws2_32.lib")

//Error reporting management function
void error(const char *er_msg)
{
    perror(er_msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	cout << "\nSetting up variables..";

	//initialize connection variables
    int sockfd, portnumber, n;
    struct sockaddr_in server_address;
    struct hostent *server;			//server profile

    char buffer[256];

    //Check for proper command line usage
    if (argc < 3) {
       fprintf(stderr,"\n%s usage %s hostname portnumber\n", argv[0], argv[0]);
       exit(0);
    }

    //convert portnumber from big endian
    portno = atoi(argv[2]);

	cout << "\nSetting up server connection...";

    //setup socket structure
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) //check for successful socket setup
        error("ERROR: socket didn't open correctly");

    //setup server structure
    server = gethostbyname(argv[1]);	//querry network for server data
    if (server == NULL) {			//check for successful server setup
        fprintf(stderr,"ERROR: no such host\n");
        exit(0);
    }

    //Setup server information
    bzero((char *) &server_address, sizeof(server_address)); //clear server address
    server_address.sin_family = AF_INET;	//address type
    bcopy((char *)server->h_addr,			//copy address
         (char *)&server_address.sin_addr.s_addr,
         server->h_length);
    server_address.sin_port = htons(portno);//conver/copy port number

    //connect to server
    if (connect(sockfd,(struct sockaddr *) &server_address,sizeof(server_address)) < 0) 
        error("ERROR connecting");

    //user interaction
    printf("Successfully setup, please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);	//gets message from command prompt
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);	//get response from server
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);

    cout << "\ntest complete, closing connection..."

    close(sockfd);


    return 0;
}