/************* Client Program **************/

// Defining header files
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

//defining constants
#define MAX_LEN 512
#define PORT 8080   // port number 


int main(){
	char *ip_addr = "127.0.0.1";  // ip address initialization 
	int socket_id;   	         // for storing socket id
	struct sockaddr_in ser_addr; // structure defination in above header files
	ser_addr.sin_family = AF_INET; // Initializing family
	ser_addr.sin_port = PORT;      // Initializing port number
	ser_addr.sin_addr.s_addr = inet_addr(ip_addr); // Initializing IP_Address
	
	// Socket Creation
	socket_id = socket(AF_INET, SOCK_STREAM, 0);   // Creating socket for client, SOCK_STREAM represents TCP
	if(socket_id < 0){                              //  Verifying socket creation
		perror("Error in creating socket");       // Error message
		exit(1);
	}
	printf("Socket Creation successfull \n");    // Success message
	
	// Connecting server and client
	if(connect(socket_id,(struct sockaddr*)&ser_addr, sizeof(ser_addr))==-1){
		perror("Error in creating connection or Connection failed");       // Error message
		exit(1);
	}
	printf("Connection Established \n");     // Success message
	char  buf[MAX_LEN];
	
	while(1){                         //Looping
		
		printf("what to do? : ");
		bzero(buf,512);
		gets(buf);                  // Scanning data from user
		if(write(socket_id,buf,512)<=0){     //writing to server and error checking if any
			perror("error in writing msg\n");
			exit(1);
		}
		if(strcmp("STOP",buf)==0){   //loop termination purpose
			break;
		}
		bzero(buf,512);
		if(read(socket_id,buf,512)<=0){    //Reading from server
			perror("error in reading msg\n");
			exit(1);
		}
		printf("%s\n",buf);      //Printing read data
	}
	
	return 0;
}
