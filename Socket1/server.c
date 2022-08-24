/************* Server Program **************/

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
	int acc;
	struct sockaddr_in ser_addr,chan_addr; // structure defination in above header files
	ser_addr.sin_family = AF_INET; // Initializing family
	ser_addr.sin_port = PORT;      // Initializing port number
	ser_addr.sin_addr.s_addr = inet_addr(ip_addr); // Initializing IP_Address
	
	// Socket Creation
	socket_id = socket(AF_INET, SOCK_STREAM, 0);   // Creating socket for server, SOCK_STREAM represents TCP
	if(socket_id < 0){                              //  Verifying socket creation
		perror("Error in creating socket");       // Error message
		exit(1);
	}
	printf("Socket Creation successfull \n");    // Success message
	int opt = 1;
	if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
	// Binding 
	if(bind(socket_id,(struct sockaddr*)&ser_addr, sizeof(ser_addr))<0){
		perror("Error in binding socket");       // Error message
		exit(1);
	}
	printf("Binding Successfull \n");
	
	// Listening
	if(listen(socket_id,7)!=0){
		perror("Error in listening");       // Error message
		exit(1);	
	}
	printf("Listening..\n");
	socklen_t len = sizeof(chan_addr);
	// Accepting
	acc = accept(socket_id,(struct sockaddr*)&chan_addr,&len);
	printf("Accepted successfully\n");
	
	int lines = 0;
	char buf[MAX_LEN];
	char buff[MAX_LEN];
	char sto[MAX_LEN];
	char chr[MAX_LEN];
	int i,j,k = 0;
	int entry,ver;
	int read_line;
	int count = 0;
	char erro[] = "Invalid syntax not in format of NLINEX or READX<k> or INSERTX<k><data>";
	char error[] = "Invalid syntax not_in format of READX<k>";
	char error1[] = "Invalid syntax not_in format of INSERTX<k><data>"; 
	char error_out_of_range[] = "Out of Range";
	char success[] = "Successfully Inserted";
	
	FILE *fp;     //File pointer initialization
	
	while(1){
		fp = fopen("server_file.txt","r");   //File open for reading
		lines=0;
		while(fgets(sto,sizeof(sto),fp)){    //Count number of lines
			lines=lines+1;
		}
		rewind(fp);                          // Re-initializing file pointer to start
		bzero(buf,512);
		if(read(acc,buf,512)<=0){           //Read from client
			perror("error in reading\n");
			exit(1);
		}
		if(strcmp("STOP",buf)==0){          //Loop termination
			break;
		}
		//Nlinex-->Number of lines
		if(!strcmp("NLINEX",buf)){
			sprintf(buf,"%d",lines);
			if(write(acc,buf,512)<=0){      //writing number of lines in a file
				perror("Error in writing\n");
				exit(1); 
			}	
		}
		else if(strlen(buf)>=5 && strncmp("READX",buf,5)==0){    //READX-->for reading particular line
			if(buf[strlen(buf)-1]!='>' && buf[5]=='<'){
				if(write(acc,error,512)<=0){      //writing error msg
					perror("Error in writing\n");
					exit(1);
				}
			}
			else{
				bzero(chr,512);
				count=0;
				read_line=0;
				k=0;
				entry=0;
				// Finding k value present in READX<k>
				for(i=0;buf[i]!='\0';i++){
					if(buf[i]=='<'){
						if(buf[i+1]=='-'){
							chr[k++]=buf[i+1];
							i++;
						}
						for(j=i+1;buf[j]!='\0';j++){
							if(buf[j]=='>'){
								//break;
							}	
							else if((buf[j]>='0' && buf[j]<='9')){
								chr[k++]=buf[j];
							}
							else{
								entry=1;
								if(write(acc,error,512)<=0){
									perror("Error in writing\n");
									exit(1);
								}
								break;
							}
						}
						read_line=atoi(chr);
						//break;
					}
				}
				
				if(read_line<0 && read_line>=(0-lines) && entry!=1){
					read_line=lines+read_line;
				}
				else if((read_line>=lines && entry!=1) || (read_line<0 && read_line<(0-lines) && entry!=1)){
					entry=1;
					if(write(acc,error_out_of_range,512)<=0){  //writing error msg for out of range
						perror("Error in writing\n");
						exit(1);
					}
				}
				rewind(fp);
				bzero(sto,512);
				//finding line present in kth index
				while(fgets(sto,sizeof(sto),fp) && entry!=1){
					
		        	if(count==read_line){
		        		bzero(buf,512);
		        		sprintf(buf,"%s",sto);
			        	if(write(acc,sto,512)<=0){
			        		perror("Error in writing\n");
			        		exit(1);
						}
		        	}
		        	count=count+1;
	        	}
	        	fclose(fp);
			}
			
		}
		else if(strlen(buf)>=5 && strncmp("INSERTX",buf,7)==0){   //Insertx-->for Inserting a new line
			FILE* temp=tmpfile(); //Temporary file creation
			bzero(chr,512);
			k=0;
			count=0;
			entry=0;
			read_line=lines;
			if((buf[7]!='<' && strlen(buf)>7) || buf[strlen(buf)-1]!='>'){
				if(write(acc,error1,512)<=0){  //writing error msg
					perror("Error in writing\n");
					exit(1);
				}	
			}
			else{
				for(i=0;buf[i]!='\0';i++){
					if(buf[i]=='<'){
						if(buf[i+1]=='-'){
							chr[k++]=buf[i+1];
							i++;
						}
						for(j=i+1;buf[j]!='\0';j++){
							if(buf[j]=='>'){
								break;
							}
							else if((buf[j]>='0' && buf[j]<='9')){
								chr[k++]=buf[j];
							}
							else{
								entry =1;
								if(write(acc,error1,sizeof(error1))<=0){   //writing error msg
									perror("Error in writing\n");
									exit(1);
								}
								break;
							}
						}
						if(strcmp(chr,"")){
							read_line=atoi(chr);
						}
						break;
					}
				}
				
				if(read_line<0 && read_line>=(0-lines) && entry!=1){
					read_line=lines+read_line;
				}
				else if((read_line>lines && entry!=1) || (read_line<0 && read_line<(0-lines) && entry!=1)){
					entry=1;
					if(write(acc,error_out_of_range,512)<=0){   //writing error msg for out of range
						perror("Error in writing\n");
						exit(1);
					}
				}
				bzero(buff,512);
				k=0;
				ver=0;
				if(entry!=1)
				{
					for(i=0;buf[i]!='\0';i++){
						if(buf[i]=='>' && buf[i+1]=='<' && buf[i-1]!='>'){
							ver=1;
							for(j=i+2;buf[j]!='\0';j++){
								if(buf[j]!='>'){
									buff[k++]=buf[j];
								}
							}
							break;
						}
					}
					
					if(ver==0){
						entry=1;
						if(write(acc,error1,sizeof(error1))<=0){
							perror("Error in writing\n");
							exit(1);
						}
					}
					//Inserting new line to file
					if(read_line==lines && entry!=1){
						while(fgets(sto,sizeof(sto),fp)){
							fputs(sto,temp);
						}
						fprintf(temp,"\n");
						fputs(buff,temp);
						fclose(fp);
						fp=fopen("server_file.txt","w");
						rewind(temp);
						while(fgets(sto,sizeof(sto),temp)){
							fputs(sto,fp);
						}
						fclose(fp);
						if(write(acc,success,sizeof(success))<=0){
							perror("Error in writing\n");
							exit(1);
						}
						
					}
					else if(entry!=1){
						count=0;
						while(fgets(sto,sizeof(sto),fp)){
		        			if(count!=read_line){
		    					fputs(sto,temp);
							}
							else{
								fputs(buff,temp);
								fprintf(temp,"\n");
								fputs(sto,temp);
							}
							count++;
						}
						fclose(fp);
						rewind(temp);
						fp=fopen("server_file.txt","w");
						while(fgets(sto,sizeof(sto),temp)){
							fputs(sto,fp);
						}
						fclose(fp);
						fclose(temp);
						if(write(acc,success,sizeof(success))<=0){
							perror("Error in writing\n");
							exit(1);
						}
					}
					
				}
				
				
			}
			
		}
		else{
			if(write(acc,erro,512)<=0){
				perror("Error in writing\n");
				exit(1);
			}
		}
	}
	
	return 0;
}
