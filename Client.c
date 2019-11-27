//****************************************************************************//
//AUTHOR: Clayton Glenn
//OUID: 113375641
//DATE: February 14, 2019
//HOMEWORK: Client / Server Socket Interaction
//Program for CS4473 Introduction to Parallel Programming
//																																						//
//This Program creates a Client that interacts with a Server by reading a text
//file and sending it to the Server through a socket with TCP / IP protocol.
//****************************************************************************//

#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define MAX 1024
#define MAX_FILE 128
#define PORT 5000
#define IPV4 AF_INET
#define DATA_STREAM SOCK_STREAM
#define PROTOCOL 0
#define FAILURE -1
#define IP_ADDRESS inet_addr("127.0.0.1")
#define PORT_ADDRESS htons(PORT)

//////////////////////////////////////////////////////////////////////////////
// Declare the FILE OF FILES for Reading all Files
//////////////////////////////////////////////////////////////////////////////

char *file_of_files = "input.txt";

//****************************************************************************//
//Main Function that opens a socket and connects
//to a server that is specified for file transmission
//****************************************************************************//
int main(int argc, char **argv) {

	//Change File of Files if Arg Specifies
	if(argc > 1) {
		file_of_files = malloc(sizeof(argv[1]) * sizeof(char *));
		file_of_files = argv[1];
	}

  //////////////////////////////////////////////////////////////////////////////
  // Declare Essential Variables for Client Program
  //////////////////////////////////////////////////////////////////////////////
	int socket_fd;					//Socket File Descriptor
	struct sockaddr_in server_address;		//Server Socket Address

	//Create Server Address for Transfer
  memset(&server_address, '0', sizeof(server_address));
  server_address.sin_family = IPV4;
  server_address.sin_addr.s_addr = IP_ADDRESS;
  server_address.sin_port = PORT_ADDRESS;

	char buffer[MAX];

  //////////////////////////////////////////////////////////////////////////////
  // Create Socket File Descriptor for Connection
  //////////////////////////////////////////////////////////////////////////////
	if ((socket_fd = socket(IPV4, DATA_STREAM, PROTOCOL)) == FAILURE) {
		fprintf(stderr, "FAILED TO CREATE SOCKET!\n");
		exit(FAILURE);
	}
	fprintf(stdout, "SOCKET CREATED\n");

  //////////////////////////////////////////////////////////////////////////////
  // Connect to Server Socket with the Client Socket
  //////////////////////////////////////////////////////////////////////////////
	if (connect(socket_fd, (struct sockaddr*)&server_address,
				sizeof(server_address)) == FAILURE) {
		fprintf(stderr, "FAIELD TO CONNECT TO SERVER!\n");
		exit(FAILURE);
	}
	fprintf(stdout, "CONNECTED TO SERVER!\n");

  //////////////////////////////////////////////////////////////////////////////
  // Communicate with the Server
  //////////////////////////////////////////////////////////////////////////////
	FILE *fof_pointer;
	if (!(fof_pointer = fopen(file_of_files, "r"))){
		fprintf(stderr, "COULD NOT OPEN %s FOR READING\n", file_of_files);
		return(FAILURE);
	}
	bzero(&buffer, sizeof(buffer));

	// reads text until newline
	while (fgets(buffer, sizeof(buffer), fof_pointer)) {

	  	////////////////////////////////////////////////////////////////////////////
	  	// If File of Files has random new line, which mostlikely at Bottom of File
	  	////////////////////////////////////////////////////////////////////////////
		if(buffer[0] == '\n'){
			break;
		}

		////////////////////////////////////////////////////////////////////////////
		// Scratch new line off buffer
		////////////////////////////////////////////////////////////////////////////
		int i = 0;
		while(buffer[i] != 0){
			i++;
		}
		i--;
		buffer[i] = 0;
    		printf("%s\n", buffer);

		////////////////////////////////////////////////////////////////////////////
	  	// Read File Specified by File of Files
	  	////////////////////////////////////////////////////////////////////////////
		FILE *f_pointer;
		if (!(f_pointer = fopen(buffer, "r"))){
			fprintf(stderr, "COULD NOT OPEN %s FOR READING\n", buffer);
		} else {

			//Send the filename to the server
			write(socket_fd, buffer, sizeof(buffer));
			////////////////////////////////////////////////////////////////////////////
			// Ensure Acknowledgement
			////////////////////////////////////////////////////////////////////////////
			bzero(&buffer, sizeof(buffer));
			read(socket_fd, buffer, sizeof(buffer));
			if(!strncmp("0", buffer, 1)){
				fprintf(stderr, "SERVER ACKNOWLEDGEMENT TIMED OUT\n");
				return(FAILURE);
			}

			////////////////////////////////////////////////////////////////////////////
			// Read File Line by Line
			////////////////////////////////////////////////////////////////////////////
			bzero(&buffer, sizeof(buffer));
			while (fgets(buffer, sizeof(buffer), f_pointer)) {
				fprintf(stdout, "%s", buffer);

				//////////////////////////////////////////////////////////////////////////
				// Send File Contents to the Server
				//////////////////////////////////////////////////////////////////////////
				if(buffer[0] != '\n'){
					write(socket_fd, buffer, sizeof(buffer));
					bzero(&buffer, sizeof(buffer));
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////
		// End File Contents for Server Acknowledgement
		////////////////////////////////////////////////////////////////////////////
		bzero(&buffer, sizeof(buffer));
		strncpy(buffer, "DONE!", 5);
		write(socket_fd, buffer, 5);
		printf("%s\n", buffer);

		////////////////////////////////////////////////////////////////////////////
		// Check Server Acknowledgement
		////////////////////////////////////////////////////////////////////////////
		read(socket_fd, buffer, sizeof(buffer));
		if(strncmp(buffer, "THANK YOU! FILE CLOSED!", 23)){
			fprintf(stderr, "FAILED TO CLOSE FILE!\n");
		}

		////////////////////////////////////////////////////////////////////////////
		// Close File and Zero Out Buffer
		////////////////////////////////////////////////////////////////////////////
		if(f_pointer) {
			fclose(f_pointer);
		}
		bzero(&buffer, sizeof(buffer));
  }

	//////////////////////////////////////////////////////////////////////////////
	// Close File of Files
	//////////////////////////////////////////////////////////////////////////////
	fclose(fof_pointer);

	//////////////////////////////////////////////////////////////////////////////
  	// Finish Up
  	//////////////////////////////////////////////////////////////////////////////
	bzero(&buffer, sizeof(buffer));
	strncpy(buffer, "FINISHED!", 9);
	write(socket_fd, buffer, sizeof(buffer));
	fprintf(stdout, "%s\n", buffer);

	//////////////////////////////////////////////////////////////////////////////
	// Acknowledge Server Closure and Close Client Socket
	//////////////////////////////////////////////////////////////////////////////
	bzero(&buffer, sizeof(buffer));
	read(socket_fd, buffer, sizeof(buffer));
	if(!strncmp(buffer, "THANK YOU CLOSE CONNECTION!", 27)){
		fprintf(stdout, "%s\n", buffer);
		close(socket_fd);
		return(0);
	} else {
		fprintf(stderr, "FAILED TO CLOSE CONNECTION!\n");
		return(FAILURE);
	}
}
