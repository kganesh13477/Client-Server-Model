#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444

int Test_filename(char file_name[1024]){
	FILE *fp;
	if((fp=fopen(file_name,"r"))){
		fclose(fp);
		return 1;
	}
	return 0;
}
int Test_file1(char file_name[1024]){
	int len=strlen(file_name);
	char *four=&file_name[len-4];
	if(strcmp(four,".txt")==0 && len>4){
		return 1;
	}
	return 0;
}

int permission(char test[1024]){
	if(strcmp(test,"V")==0 || strcmp(test,"E")==0){
		return 1;
	}
	return 0;
}

int main(){

	int socket_id, ret,x,y,i;
	struct sockaddr_in serverAddr;
	int fdmax;
	
	fd_set readfds1;
	char buffer[1024];
	char choice[1024];
	char file[1024];
	char c[1024];
	char d[1024];
	char p[1024];
	int c1;
	int count;
	int cl,el;
	FILE *fp;

	socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_id < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(socket_id, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	
	pid_t p1;
	char inp[1024];
	char buf[1024];
	bzero(inp,1024);
	read(socket_id,inp,1024);
	if(strcmp("Limit",inp)==0){
		close(socket_id);
		printf("Connection to server limit exceded\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");
	puts(inp);
	bzero(inp,1024);
	
	while(1){
		int newS=socket_id;
		FD_ZERO(&readfds1);
		FD_SET(newS, &readfds1); /* add sockfd to connset */
		FD_SET(0, &readfds1);			 /* add STDIN to connset */
		fdmax = newS;
		if (select(fdmax + 1, &readfds1, NULL, NULL, NULL) < 0)
		{
			perror("select error\n");
			exit(0);
		}	
		if (FD_ISSET(newS, &readfds1)){
			bzero(buf,1024);
			read(newS,buf,1024);
			printf("\nRequest for accepting permission as %s\n",buf);
			bzero(buf,1024);
			printf("Enter yes or no\n");
			gets(buf);
			while(strcmp(buf,"yes")!=0 && strcmp(buf,"no")!=0){
				printf("Enter yes or no only\n");
				bzero(buf,1024);
				gets(buf);
			}
			write(newS,buf,1024);
			if(strcmp(buf,"yes")==0){
				printf("Successfully accepted\n");
			}
			else if(strcmp(buf,"no")==0){
				printf("Successfully rejected\n");
			}
//			close(newS);
			fflush(stdout);
		}
		if(FD_ISSET(0, &readfds1)){
			bzero(buf,1024);
			bzero(choice,1024);
			bzero(file,1024);
			bzero(c,1024);
			bzero(d,1024);
			bzero(p,1024);
			
			gets(buf);
			if(strcmp(buf,"/exit")==0){
				write(socket_id,buf,1024);
				printf("Successfully Disconnected\n");
				break;
			}
			else if(strcmp(buf,"/users")==0){
				write(socket_id,buf,1024);
				int count,id,id1;
				printf("printing active clients\n");
				read(socket_id,&count,sizeof(int));
				if(count==0){
					printf("No active clients\n");
				}
				else{
					for(i=0;i<count;i++){
						read(socket_id,&id,sizeof(int));
						read(socket_id,&id1,sizeof(int));
						printf("socket[%d]_id = %d, client[%d]_id = %d\n",i,id1,i,id);
					}
				}
			}
			else if(strcmp(buf,"/files")==0){
				write(socket_id,buf,1024);
				printf("print files and its client id_s\n");
				bzero(buf,1024);
				read(socket_id,&count,sizeof(int));
				for(i=0;i<count;i++){
					read(socket_id,buf,1024);
					puts(buf);
					bzero(buf,1024);
					read(socket_id,&cl,sizeof(int));
					read(socket_id,&el,sizeof(int));
					for(x=0;x<cl;x++){
						read(socket_id,buf,1024);
						puts(buf);
						bzero(buf,1024);
					}
					for(x=0;x<el;x++){
						read(socket_id,buf,1024);
						puts(buf);
						bzero(buf,1024);
					}
				}
			}
			else if(4==sscanf(buf,"/%s %s %s %s",choice,file,c,d)){
				c1=Test_file1(file);
				if(c1==1){
					if(strcmp(choice,"invite")==0){
						int p;
						if(4==sscanf(buf,"/%s %s %d %s",choice,file,&p,d)){
							c1=permission(d);
							if(c1==1){
								printf("%s %s %d %s",choice,file,p,d);
								write(socket_id,buf,1024);
								printf("\nInvitation sent\n");
								bzero(buf,1024);
								read(socket_id,buf,1024);
								if(strcmp(buf,"No error")==0){
									bzero(buf,1024);
									read(socket_id,buf,1024);
									if(strcmp(buf,"yes")==0){
										printf("Invitation accepted\n");
									}
									else{
										printf("Invitation rejected\n");
									}
								}
								else{
									puts(buf);
								}
							}
							else{
								printf("Permission format error or Invalid syntax\n");
							}
						}
						else{
							printf("Permission format error or client_id error\n");
						}
					}
					else if(strcmp(choice,"read")==0){
						if(4==sscanf(buf,"/%s %s %d %d",choice,file,&x,&y) && 5!=sscanf(buf,"/%s %s %d %d%s",choice,file,&x,&y,p)){
							write(socket_id,buf,1024);
							printf("read data at particular data\n");
							bzero(buf,1024);
							read(socket_id,buf,1024);
							if(strcmp(buf,"No error")==0){
								read(socket_id,&count,sizeof(int));
								bzero(buf,1024);
								for(i=0;i<count;i++){
									read(socket_id,buf,1024);
									puts(buf);
									bzero(buf,1024);
								}
							}
							else{
								puts(buf);
							}	
						}
						else{
							printf("Invalid syntax in start and end positions\n");
						}
					}
					else if(strcmp(choice,"insert")==0){
						puts(buf);
						bzero(d,1024);
						if(4==sscanf(buf,"%s %s %d %[^\n]",choice,file,&x,c)){
							if(c[0]=='"' && c[strlen(c)-1]=='"' && strlen(c)>=2){
								write(socket_id,buf,1024);
								printf("inserting\n");
								bzero(buf,1024);
								read(socket_id,buf,1024);
								if(strcmp(buf,"No error")==0){
									read(socket_id,&count,1024);
									fp=fopen(file,"w");
									bzero(buf,1024);
									for(i=0;i<count;i++){
										read(socket_id,buf,1024);
										fputs(buf,fp);
										puts(buf);
										bzero(buf,1024);
										
									}
									fclose(fp);
								}
								else{
									puts(buf);
								}	
							}
							else{
								printf("No comments or Invalid syntax\n");
							}
						}
						else if(3==sscanf(buf,"%s %s %[^\n]",choice,file,c)){
							if(c[0]=='"' && c[strlen(c)-1]=='"' && strlen(c)>=2){
								write(socket_id,buf,1024);
								printf("inserting\n");
								bzero(buf,1024);
								read(socket_id,buf,1024);
								if(strcmp(buf,"No error")==0){
									read(socket_id,&count,1024);
									fp=fopen(file,"w");
									bzero(buf,1024);
									for(i=0;i<count;i++){
										read(socket_id,buf,1024);
										fputs(buf,fp);
										puts(buf);
										bzero(buf,1024);
										
									}
									fclose(fp);
								}
								else{
									puts(buf);
								}	
							}
							else{
								printf("No comments or Invalid syntax\n");
							}
						}
					}
					else if(strcmp(choice,"delete")==0){
						if(4==sscanf(buf,"/%s %s %d %d",choice,file,&x,&y) && 5!=sscanf(buf,"/%s %s %d %d%s",choice,file,&x,&y,p)){
							write(socket_id,buf,1024);
							printf("delete particular line\n");
							bzero(buf,1024);
							read(socket_id,buf,1024);
							if(strcmp(buf,"No error")==0){
								read(socket_id,&count,sizeof(int));
								bzero(buf,1024);
								fp=fopen(file,"w");
								for(i=0;i<count;i++){
									read(socket_id,buf,1024);
									fputs(buf,fp);
									puts(buf);
									bzero(buf,1024);
								}
								fclose(fp);
							}
							else{
								puts(buf);
							}	
							
						}
						else{
							printf("Invalid syntax in start and end positions\n");
						}
					}
					else{
						printf("Invalid syntax\n");
					}
				}
				else{
					printf("Invalid syntax or Invalid file_name\n");
				}
			}
			else if(3==sscanf(buf,"/%s %s %s",choice,file,c)){
				c1=Test_file1(file);
				if(c1==1){
					if(strcmp(choice,"read")==0){
						if(3==sscanf(buf,"/%s %s %d",choice,file,&x) && 4!=sscanf(buf,"/%s %s %d%s",choice,file,&x,p)){
							write(socket_id,buf,1024);
							printf("read data at particular data\n");
							bzero(buf,1024);
							read(socket_id,buf,1024);
							if(strcmp(buf,"No error")==0){
								bzero(buf,1024);
								read(socket_id,buf,1024);
								puts(buf);
							}
							else{
								puts(buf);
							}	
						}
						else{
							printf("Invalid syntax in start and end positions\n");
						}
					}
					else if(strcmp(choice,"insert")==0){
						if(c[0]=='"' && c[strlen(c)-1]=='"' && strlen(c)>2){
							write(socket_id,buf,1024);
							printf("inserting\n");
							bzero(buf,1024);
							read(socket_id,buf,1024);
							if(strcmp(buf,"No error")==0){
								read(socket_id,&count,1024);
								fp=fopen(file,"w");
								bzero(buf,1024);
								for(i=0;i<count;i++){
									read(socket_id,buf,1024);
									fputs(buf,fp);
									puts(buf);
									bzero(buf,1024);
										
								}
								fclose(fp);
							}
							else{
								puts(buf);
							}	
						}
						else{
							printf("No comments or Invalid syntax or Invalid in content\n");
						}
					}
					else if(strcmp(choice,"delete")==0){
						if(3==sscanf(buf,"/%s %s %d",choice,file,&x) && 4!=sscanf(buf,"/%s %s %d%s",choice,file,&x,p)){
							write(socket_id,buf,1024);
							printf("delete particular content\n");
							bzero(buf,1024);
							read(socket_id,buf,1024);
							if(strcmp(buf,"No error")==0){
								read(socket_id,&count,sizeof(int));
								fp=fopen(file,"w");
								bzero(buf,1024);
								for(i=0;i<count;i++){
									read(socket_id,buf,1024);
									fputs(buf,fp);
									puts(buf);
									bzero(buf,1024);
								}
								fclose(fp);
							}
							else{
								puts(buf);
							}	
						}
						else{
							printf("Invalid syntax in start and end positions\n");
						}
					}
					else{
						printf("Invalid syntax\n");
					}
				}
			}
			else if(2==sscanf(buf,"/%s %s",choice,file)){
				c1=Test_file1(file);
				if(c1==1){
					if(strcmp(choice,"read")==0){
						write(socket_id,buf,1024);
						printf("Read entire file\n");
						bzero(buf,1024);
						read(socket_id,buf,1024);
						if(strcmp(buf,"No error")==0){
							bzero(buf,1024);
							read(socket_id,&count,sizeof(int));
							for(i=0;i<count;i++){
								read(socket_id,buf,1024);
								puts(buf);
								bzero(buf,1024);
							}
						}
						else{
							puts(buf);
						}
					}
					else if(strcmp(choice,"delete")==0){
						write(socket_id,buf,1024);
						printf("delete entire file\n");
						bzero(buf,1024);
						read(socket_id,buf,1024);
						if(strcmp(buf,"No error")==0){
							read(socket_id,buf,1024);
							fp=fopen(buf,"w");
							fclose(fp);
							printf("whole data removed from file\n");
						}
						else{
							puts(buf);
						}
					}
					else if(strcmp(choice,"upload")==0){
						c1=Test_filename(file);
						if(c1==1){
							write(socket_id,buf,1024);
							printf("uploading file name\n");
							bzero(buf,1024);
							read(socket_id,buf,1024);
							if(strcmp(buf,"No error")==0){
								fp=fopen(file,"r");
								bzero(buf,1024);
								count=0;
								while(fgets(buf,1024,fp)){
									count++;
								}
								write(socket_id,&count,sizeof(int));
								rewind(fp);
								bzero(buf,1024);
								while(fgets(buf,1024,fp)){
									write(socket_id,buf,1024);
								bzero(buf,1024);
								}
								fclose(fp);
								bzero(buf,1024);
								read(socket_id,buf,1024);
								puts(buf);
								bzero(buf,1024);
							}
							else{
								puts(buf);
							}	
						}
						else{
							printf("Invalid file name\n");
						}
					}
					else if(strcmp(choice,"download")==0){
						write(socket_id,buf,1024);
						printf("download file\n");
						bzero(buf,1024);
						if(read(socket_id,buf,1024)<0){
							perror("read error");
							exit(1);
						}
						if(strcmp(buf,"No error")==0){
							bzero(buf,1024);
							read(socket_id,buf,1024);
							puts(buf);
						}
						else{
							puts(buf);
						}	
					}
					else{
						printf("Invalid syntax\n");
					}
				}
				else{
					printf("Invalid syntax or Invalid file name\n");
				}
			}
			else{
				printf("Invalid syntax\n");
			}
//			printf("what to do?\t:");
			fflush(stdout);
		}
	}
	return 0;
}
