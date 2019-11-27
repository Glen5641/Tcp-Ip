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
#define PORT 5000
#define IPV4 AF_INET
#define DATA_STREAM SOCK_STREAM
#define PROTOCOL 0
#define FAILURE -1
#define IP_ADDRESS htonl(INADDR_ANY)
#define PORT_ADDRESS htons(PORT)

////////////////////////////////////////////////////////////////////////////////
//                    Clayton Glenn OUID: 113375641                           //
//        Program for CS4473 Introduction to Parallel Programming             //
//       This Program creates a Server that interacts with a Client           //
//      by recieving a text file from the Client and writing it to a          //
//           file through a socket with TCP/IP protocol.                      //
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {

  //////////////////////////////////////////////////////////////////////////////
  // Declare Essential Variables for Server Program
  //////////////////////////////////////////////////////////////////////////////
  int socket_fd, connection_fd;
  unsigned int client_size;
	struct sockaddr_in server_address, client;
  memset(&server_address, '0', sizeof(server_address));
	server_address.sin_family = IPV4;
	server_address.sin_addr.s_addr = IP_ADDRESS;
	server_address.sin_port = PORT_ADDRESS;

  //////////////////////////////////////////////////////////////////////////////
  // Create Socket File Descriptor for Connection
  //////////////////////////////////////////////////////////////////////////////
	if ((socket_fd = socket(IPV4, SOCK_STREAM, PROTOCOL)) == FAILURE) {
		perror("FAILED TO CREATE SOCKET!\n");
		exit(0);
	}
	printf("SOCKET CREATED!\n");

  //////////////////////////////////////////////////////////////////////////////
  // Bind Socket File Descriptor to the Server
  //////////////////////////////////////////////////////////////////////////////
	if ((bind(socket_fd, (struct sockaddr*)&server_address,
                      sizeof(server_address))) == FAILURE) {
		perror("FAILED TO BIND SOCKET!\n");
		exit(0);
	}
	printf("SOCKET BOUND!\n");

  //////////////////////////////////////////////////////////////////////////////
  // Listen for Client Sockets Attempting Connection
  //////////////////////////////////////////////////////////////////////////////
	if ((listen(socket_fd, 5)) == FAILURE) {
		perror("FAILED TO LISTEN!\n");
		exit(0);
	}
	printf("LISTENING!\n");

  //////////////////////////////////////////////////////////////////////////////
  // Accept the Client and Allow Data Transmittion
  //////////////////////////////////////////////////////////////////////////////
  client_size = sizeof(client);
	if ((connection_fd = accept(socket_fd, (struct sockaddr*)&client,
                                        &client_size)) == FAILURE) {
		perror("FAILED TO ACCEPT CLIENT!\n");
		exit(0);
	}
	printf("CLIENT ACCEPTED!\n");

  //////////////////////////////////////////////////////////////////////////////
  // Loop and Interact with Client
  //////////////////////////////////////////////////////////////////////////////
  char buffer[MAX];
  FILE *f_pointer = 0;
  // infinite loop for chat
  for (;;) {

    //////////////////////////////////////////////////////////////////////////////
    // Read Client Messages
    //////////////////////////////////////////////////////////////////////////////

    //Clear buffer to ensure clean pass and read argument
    bzero(buffer, sizeof(buffer));
    read(connection_fd, buffer, sizeof(buffer));

    //////////////////////////////////////////////////////////////////////////////
    // Finish Communication
    //////////////////////////////////////////////////////////////////////////////
    if (!strncmp("FINISHED!", buffer, 9)) { //If line is FINISHED
      //Clear Buffer for output and sent to client
      bzero(buffer, sizeof(buffer));
      strcpy(buffer, "THANK YOU CLOSE CONNECTION!");
      printf("%s\n", buffer);
      write(connection_fd, buffer, sizeof(buffer));

      //Close socket and finish program
      close(socket_fd);
      break;

      //////////////////////////////////////////////////////////////////////////////
      // Finish Current File
      //////////////////////////////////////////////////////////////////////////////
    } else if(!strncmp("DONE!", buffer, 5)){
      bzero(buffer, sizeof(buffer));
      strcpy(buffer, "THANK YOU! FILE CLOSED!");
      printf("%s\n", buffer);
      write(connection_fd, buffer, sizeof(buffer));
      if(f_pointer) {
        fclose(f_pointer);
        f_pointer = 0;
      }

      //////////////////////////////////////////////////////////////////////////////
      // Open or Write to File
      //////////////////////////////////////////////////////////////////////////////
    } else if(buffer[0] != 0){

////////////////////////////////////////////////////////////////////////////////////
//Added For Demo
////////////////////////////////////////////////////////////////////////////////////
      if(!f_pointer) {
        for(int i = MAX - 1; i >= 0; i--){
          buffer[i+1]=buffer[i];
        }
        buffer[0]='f';
		    if (!(f_pointer = fopen(buffer, "w"))) {
			    fprintf(stderr, "COULD NOT OPEN %s FOR READING\n", buffer);
          return(FAILURE);
		    }
        fprintf(stdout, "%s\n", buffer); // Print File for Debugging

////////////////////////////////////////////////////////////////////////////////////
      // Open File by buffer
      /*if(!f_pointer) {
		    if (!(f_pointer = fopen(buffer, "w"))) {
			    fprintf(stderr, "COULD NOT OPEN %s FOR READING\n", buffer);
			    return(FAILURE);
		    }
        fprintf(stdout, "%s\n", buffer); // Print File for Debugging*/

        //Clear Buffer for Acceptance, and Write
        bzero(buffer, sizeof(buffer));
        strcpy(buffer, "1");
        write(connection_fd, buffer, sizeof(buffer));

      // Write to File Character by Character
      } else {
        fprintf(stdout, "%s", buffer);
        int index = 0;
        while(buffer[index] != 0){
          fprintf(f_pointer, "%c", buffer[index++]);
        }
      }
    } else {
      // Do Nothing Because Buffer is Empty
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Close the Server Socket
  //////////////////////////////////////////////////////////////////////////////
  return(0);
}
