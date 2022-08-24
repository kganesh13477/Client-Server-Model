/******** clinet  *******/


//Header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Defining constants
#define PORT 4444
//checking field format
int Test_order(char order[1024]){
	if(strcmp(order,"D")==0 || strcmp(order,"N")==0 || strcmp(order,"P")==0 ||strcmp(order,"date")==0 || strcmp(order,"item_name")==0 || strcmp(order,"price")==0){
		return 1;
	}
	return 0;
}
//checking file name format
int Test_file(char file_name[1024]){
	FILE *fp;
	if((fp=fopen(file_name,"r"))){
		fclose(fp);
		return 1;
	}
	return 0;
}
//checking file name format
int Test_file1(char file_name[1024]){
	int len=strlen(file_name);
	char *four=&file_name[len-4];
	if(strcmp(four,".txt")==0 && len>4){
		return 1;
	}
	return 0;
}

//main starts here
int main(){
	int socket_id;
	struct sockaddr_in serverAddr;
	char buf[1024];
	char check[1024]={0};
//socket creating
	socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_id < 0){
		printf("Error in connection\n");
		exit(1);
	}
	printf("Socket is created\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//connecting
	if(connect(socket_id, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
		printf("Error in connection\n");
		exit(1);
	}
	// Connection establishment
	bzero(check,1024);
	if(recv(socket_id,check,1024,0)<=0){
		perror("Invalid");
		exit(1);
	}
	if(strcmp("Limit",check)==0){
		close(socket_id);
		printf("Connection to server limit exceded\n");
		exit(1);
	}
	printf("Connected to server\n");
	char input[1024]={0};
	char file_name1[1024]={0};
	char file_name2[1024]={0};
	char file_name3[1024]={0};
	char order[1024];
	char choice[100]={0};
	int Res1,Res2,Res3,Res4;
//Program for dividing parsed msg and performing operation.
	while(1){
		printf("What to do:\t");
		FILE *fp;
		FILE *fp1;
		FILE *fp2;
		bzero(input,1024);
		bzero(file_name1,1024);
		bzero(file_name2,1024);
		bzero(file_name3,1024);
		bzero(order,1024);
		bzero(choice,1024);		
		printf("enter string");
		gets(input);
		if(strcmp("/exit",input)==0){
			write(socket_id,input,1024);
			exit(1);
		}
		else if(5==sscanf(input,"/%s %s %s %s %s",choice,file_name1,file_name2,file_name3,order)){
			printf("%s %s %s %s %s\n",choice,file_name1,file_name2,file_name3,order);
			Res1=Test_file(file_name1);
			Res2=Test_file(file_name2);
			Res3=Test_file1(file_name3);
			Res4=Test_order(order);
			if(Res1==1 && Res2==1 && Res3==1 && Res4==1 && strcmp(choice,"merge")==0){
				printf("enter validity program\n");
				bzero(input,1024);
				sprintf(input,"%s %s %s %s %s",choice,file_name1,file_name2,file_name3,order);
				send(socket_id, input, strlen(input), 0);
				int count1=0,count2=0;
				fp=fopen(file_name1,"r");
				if(fp==NULL){
					perror("Error in reading data from file");
					exit(1);
				}
				while(fgets(buf,1024,fp)){
					count1++;
				}
				printf("file1_length:%d\n",count1);
				write(socket_id,&count1,sizeof(int));
				rewind(fp);
				while(fgets(buf,1024,fp)){
					write(socket_id,buf,1024);
				}
				fp1=fopen(file_name2,"r");
				if(fp1==NULL){
					perror("Error in reading data from file");
					exit(1);
				}
				while(fgets(buf,1024,fp1)){
					count2++;
				}
				printf("file2_length:%d\n",count2);
				write(socket_id,&count2,sizeof(int));
				rewind(fp1);
				while(fgets(buf,1024,fp1)){
					write(socket_id,buf,1024);
				}
				fclose(fp1);
				fclose(fp);
				bzero(input,1024);
				read(socket_id,input,1024);
				if(strcmp(input,"No error in file format")!=0){
					printf("%s\n",input);
				}
				else{
					printf("successfull\n");
					int lines=0;
					int count;
					fp2=fopen(file_name3,"w");
					printf("%s\n",input);
					bzero(buf,1024);
					read(socket_id,&count,sizeof(int));
					while(count!=lines){
						read(socket_id,buf,1024);
						//printf("%s\n",buf);
						fputs(buf,fp2);
						bzero(buf,1024);
						lines++;
					}
					fclose(fp2);
					
				}
				
			}
			else{
				printf("Invalid syntax\n");
			}
		
		}
		else if(3==sscanf(input,"/%s %s %s",choice,file_name1,order)){
			printf("%s %s %s\n",choice,file_name1,order);
			Res1=Test_order(order);
			Res2=Test_file(order);
			Res3=Test_file(file_name1);
			if((strcmp(choice,"sort")==0) && Res3==1 && Res1==1){
				printf("Enter sort file\n");
				int count=0;
				sprintf(input,"%s %s %s",choice,file_name1,order);
				send(socket_id, input, strlen(input), 0);
				fp=fopen(file_name1,"r");
				if(fp==NULL){
					perror("Error in reading data from file");
					exit(1);
				}
				while(fgets(buf,1024,fp)){
					count++;
				}
				printf("file_length:%d\n",count);
				write(socket_id,&count,sizeof(int));
				rewind(fp);
				while(fgets(buf,1024,fp)){
					write(socket_id,buf,1024);
					bzero(buf,1024);
				}
				bzero(input,1024);
				read(socket_id,input,1024);
				if(strcmp(input,"No error in file format")!=0){
					printf("%s\n",input);
				}
				else{
					fclose(fp);
					printf("successfull\n");
					fp=fopen(file_name1,"w");
					int lines=0;
					bzero(buf,1024);
					read(socket_id,&count,sizeof(int));
					while((count)!=lines){
						read(socket_id,buf,1024);
						fputs(buf,fp);
						bzero(buf,1024);
						lines++;
					}
					printf("%s\n",input);
				}
				rewind(fp);
			}
			else if(strcmp(choice,"similarity")==0 && Res2==1 && Res3==1){
				printf("Entered similarity\n");
				sprintf(input,"%s %s",file_name1,order);
				send(socket_id, input, strlen(input), 0);
				
				int count1=0,count2=0;
				fp=fopen(file_name1,"r");
				if(fp==NULL){
					perror("Error in reading data from file");
					exit(1);
				}
				while(fgets(buf,1024,fp)){
					count1++;
				}
				printf("file_length:%d\n",count1);
				write(socket_id,&count1,sizeof(int));
				rewind(fp);
				while(fgets(buf,1024,fp)){
					write(socket_id,buf,1024);
				}
				fp1=fopen(order,"r");
				if(fp1==NULL){
					perror("Error in reading data from file");
					exit(1);
				}
				while(fgets(buf,1024,fp1)){
					count2++;
				}
				printf("file_length:%d\n",count2);
				write(socket_id,&count2,sizeof(int));
				rewind(fp1);
				while(fgets(buf,1024,fp1)){
					write(socket_id,buf,1024);
				}
				fclose(fp1);
				fclose(fp);
				printf("files sent\n");
				bzero(input,1024);
				read(socket_id,input,1024);
				if(strcmp(input,"No error in file format")!=0){
					printf("%s\n",input);
				}
				else{
					int max,lines=0;
					printf("%s\n",input);
					read(socket_id,&max,sizeof(int));
					bzero(buf,1024);
					while(lines!=max){
						read(socket_id,buf,1024);
						printf("%s",buf);
						bzero(buf,1024);
						lines++;
					}
					if(max==0){
						printf("no similarities\n");
					}
				}
			}
			else{
				printf("Invalid syntax\n");
			}
		}
	
		else{
			printf("Invalid syntax\n");
		}
	}

	return 0;
}
