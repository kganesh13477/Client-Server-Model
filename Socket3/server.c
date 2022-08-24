/********************** server program using select************/

// Header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> 
#include <time.h>

// constant defining
#define PORT 4444
#define max_clients 5

int store_rand[5]={0};
int c_id[5]={0};
int s_id[5]={0};

// structure defining
struct data{
	int owner_id;
	int editor_id[4];
	int collab_id[4];
	char f_name[1024];
	int leng;
	
}data[1024];

struct mes{
	
	char cha[1024];
	
}mes[1024];

struct arra{
	int a[10];
	int lengt;
};

int verify(int a[10],int m,int n){
	for(int i=0;i<n;i++){
		if(a[i]==m){
			return 1;
		}
	}
	return 0;
}

int verify_rand(int x){
	for(int i=0;i<5;i++){
		if(store_rand[i]==x){
			return 1;
		}
	}
	return 0;
}

int parsing_msg(char msg[1024]){
	int count=0,j=0;
	
	for(int i=0;msg[i]!='\0';i++){
		if(msg[i]=='\\' && msg[i+1]=='n'){
			count++;
			j=0;
			i++;
		}
		else{
			mes[count].cha[j++]=msg[i];	
		}	
	}
	return count+1;
}

struct arra remove_arr(int a1[10],int m,int n){
	int k=0;
	struct arra pi;
	for(int j=0;j<n;j++){
		if(m!=a1[j]){
			pi.a[k++]=a1[j];
		}
	}
	pi.lengt=k;
	return pi;
}

// Main starts from here:
int main(int argc , char *argv[])
{
	// All variables used in program are defining
	struct data data1[1024];
	int socket_id , add_len , new_socket , client_socket[5];
	srand(time(NULL));
	int active, i , valread , sd;
	int inc=0;
	int max_sd,newSocket,m,q=0;
	struct sockaddr_in add;
	int option = 1;
	int limit=5;
	int total_conn=0;
	int entry;
	char error[]="Limit";
	char er3[]="Already invited";
	char error1[]="files empty";
	char errr[]="No such file exit";
	char er1[]="Out of range";
	char er2[]="Start index cross end index so no data from start to end is printed";
	char er4[]="Same file name in the server list  enter different file name";
	char succ[]="Uploaded successfully";
	char e1[]="No file name with this client";
	char e2[]="client_id not active or inviting this client not possible";
	char e3[]="No error";
	int entry1;
	char e4[]="No permission to download this file";
	char input[1024]={0};
	char file[1024];
	char c[1024];
	char d[1024];
	int check1,check2;
	char choice[100]={0};
	char buff[1024]={0};
	char buf[1024];
	int rand_num;
	char val[2046];
	int count,ei[1024]={0},ci[1024]={0},x,y,line,start,end;
	char buffer[1025];
	fd_set r_fds;
	char *message = "Welcome message from server\n";
	
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
		
	//create a master socket as socket_id
	if( (socket_id = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	if( setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, (char *)&option,sizeof(option)) < 0 )
	{
		perror("error in setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//type of socket created
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons( PORT );
		
	//bind the socket to localhost port 8888
	if (bind(socket_id, (struct sockaddr *)&add, sizeof(add))<0)
	{
		perror("binding failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener Listening on port %d \n", PORT);
		
	//try to specify maximum queue as 5
	if (listen(socket_id, 5) < 0)
	{
		perror("error in listen");
		exit(EXIT_FAILURE);
	}
		
	//accept the incoming connection
	add_len = sizeof(add);
	puts("Waiting for connections establishment");
		
	while(1)
	{
		//clear the socket set
		FD_ZERO(&r_fds);
		FILE *fp;
		FILE *tmp=tmpfile();
	
		//add master socket to set
		FD_SET(socket_id, &r_fds);
		max_sd = socket_id;
			
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &r_fds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
		active = select( max_sd + 1 , &r_fds , NULL , NULL , NULL);
	
		if ((active < 0) && (errno!=EINTR))
		{
			printf("select usage error");
		}
		if (FD_ISSET(socket_id, &r_fds))
		{
			// accept any new connection occurs
			if ((new_socket = accept(socket_id,(struct sockaddr *)&add, (socklen_t*)&add_len))<0)
			{
				perror("accept command error");
				exit(EXIT_FAILURE);
			}
			// checking total connections to server
			if(total_conn<max_clients){
				rand_num=rand()%100000;
				while(rand_num%10000==rand_num || verify_rand(rand_num)){
					rand_num=rand()%100000;
				}
				store_rand[q++]=rand_num;
				printf("Client Id:%d\n",rand_num);
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(add.sin_addr) , ntohs(add.sin_port));
			total_conn++;
			
			//send new connection greeting message
			if( send(new_socket, message, strlen(message), 0) != strlen(message) )
			{
				perror("send");
			}
				
			puts("Welcome message sent successfully");
				
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					c_id[i]=rand_num;
					s_id[i]=new_socket;
					client_socket[i] = new_socket;
					printf("Added to list of sockets at index: %d\n" , i);
					break;
				}
			}
		}
		else{
			write(new_socket,error,1024);
		}
	}
	// Operation starts here
		for (m = 0; m < max_clients; m++)
		{
			newSocket = client_socket[m];
				
			if (FD_ISSET( newSocket , &r_fds))
			{
				bzero(input,1024);
				bzero(choice,1024);
				bzero(file,1024);
				bzero(c,1024);
				bzero(d,1024);
				read(newSocket, input, 1024);
				if(strcmp(input,"/exit") == 0)
				{
					int pi=0;
					int cid[10];
					int eid[10];
					for(i=0;i<inc;i++){
						if(data[i].owner_id!=c_id[m]){
							data1[pi]=data[i];
							cid[pi]=ci[i];
							eid[pi++]=ei[i];
						}
						bzero(data[i].f_name,1024);
						ci[i]=0;
						ei[i]=0;
					}
					inc=pi;
					for(i=0;i<inc;i++){
						struct arra temp=remove_arr(data1[i].collab_id,c_id[m],cid[i]);
						cid[i]=temp.lengt;
						struct arra temp1=remove_arr(data1[i].editor_id,c_id[m],eid[i]);
						eid[i]=temp1.lengt;
						for(int j=0;j<cid[i];j++){
							data1[i].collab_id[j]=temp.a[j];
						}
						for(int j=0;j<eid[i];j++){
							data1[i].editor_id[j]=temp1.a[j];
						}
					}
					for(i=0;i<inc;i++){
						data[i]=data1[i];
						ci[i]=cid[i];
						ei[i]=eid[i];
					}
					getpeername(newSocket , (struct sockaddr*)&add , (socklen_t*)&add_len);
					printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(add.sin_addr) , ntohs(add.sin_port));
					total_conn--;
					close(newSocket);
					client_socket[m] = 0;
					c_id[m]=0;
					s_id[m]=0;
					break;
				}
				else if(strcmp(input,"/users")==0){
					printf("print clients\n");
					int count=0;
					for(i=0;i<max_clients;i++){
						if(client_socket[i]!=0){
							count++;
						}
					}
					write(newSocket,&count,sizeof(int));
					if(count>0){
						for(i=0;i<max_clients;i++){
							if(client_socket[i]!=0){
								write(newSocket,&c_id[i],sizeof(int));
								write(newSocket,&s_id[i],sizeof(int));
							}
						}
					}
					
				}
				else if(strcmp(input,"/files")==0){
					printf("printed files data\n");
					write(newSocket,&inc,sizeof(int));
					for(i=0;i<inc;i++){
						bzero(val,1024);
						sprintf(val,"file_name = %s and its owner is %d and length of file is %d",data[i].f_name,data[i].owner_id,data[i].leng);
						write(newSocket,val,1024);
						bzero(val,1024);
						int edl=ei[i];
						int cdl=ci[i];
						write(newSocket,&cdl,sizeof(int));
						write(newSocket,&edl,sizeof(int));
						for(x=0;x<ci[i];x++){
							sprintf(val,"viewer_%d=%d",x,data[i].collab_id[x]);
							write(newSocket,val,1024);
							bzero(val,1024);
						}
						bzero(val,1024);
						for(y=0;y<ei[i];y++){
							sprintf(val,"editor_%d=%d",y,data[i].editor_id[y]);
							write(newSocket,val,1024);
							bzero(val,1024);
						}
					}
				}
				else if(4==sscanf(input,"/%s %s %s %s",choice,file,c,d)){
					if(strcmp(choice,"invite")==0){
						printf("invite client\n");
						int p,t1,entry=0,entry1=0;
						sscanf(input,"/%s %s %d %s",choice,file,&p,d);
						for(i=0;i<max_clients;i++){
							if(c_id[i]==p && c_id[m]!=c_id[i]){
								printf("%d\n",c_id[i]);
								entry=1;
								t1=client_socket[i];
								break;
							}
						}
						if(entry==1){
							for(i=0;i<inc;i++){
								if(strcmp(file,data[i].f_name)==0 && c_id[m]==data[i].owner_id){
									entry=0;
									if(strcmp(d,"E")==0){
										for(int j=0;j<ei[i];j++){
											if(data[i].editor_id[j]==p){
												entry1=1;
											}
										}
										if(entry1==0){
											write(newSocket,e3,1024);
											bzero(val,1024);
											sprintf(val,"editor of file_name:%s,from client_id:%d",data[i].f_name,data[i].owner_id);
											write(t1,val,1024);
											bzero(buf,1024);
											if(read(t1,buf,1024)<0){
												perror("reading error");
												exit(1);
											}
											puts(buf);
											if(strcmp(buf,"yes")==0){
												data[i].editor_id[ei[i]++]=p;
											}
											write(newSocket,buf,1024);
											bzero(buf,1024);
											break;
										}
										else{
											write(newSocket,er3,1024);
										}
									}
									else if(strcmp(d,"V")==0){
										for(int j=0;j<ci[i];j++){
											if(data[i].collab_id[j]==p){
												entry1=1;
											}
										}
										if(entry1==0){
											write(newSocket,e3,1024);
											bzero(val,1024);
											sprintf(val,"viewer of file_name:%s,from client_id:%d",data[i].f_name,data[i].owner_id);
											write(t1,val,1024);
											bzero(buf,1024);
											read(t1,buf,1024);
											puts(buf);
											if(strcmp(buf,"yes")==0){
												data[i].collab_id[ci[i]++]=p;
											}
											write(newSocket,buf,1024);
											bzero(buf,1024);
											break;
										}
										else{
											write(newSocket,er3,1024);
										}
									}
								}
							}
							if(entry==1){
								write(newSocket,e1,1024);
							}
						}
						else{
							write(newSocket,e2,1024);
						}
						
					}
					else if(strcmp(choice,"read")==0){
						printf("read data\n");
						entry=0;
						entry1=0;
						count=0;
						sscanf(input,"/%s %s %d %d",choice,file,&start,&end);
						for(i=0;i<inc;i++){
							if(strcmp(data[i].f_name,file)==0){
								entry=1;
								check1=verify(data[i].editor_id,c_id[m],ei[i]);
								check2=verify(data[i].collab_id,c_id[m],ci[i]);
								if(data[i].owner_id==c_id[m] || check1==1 || check2==1){
									if(start>=data[i].leng || (start<0 && start<(0-data[i].leng)) || end>=data[i].leng || (end<0 && end<(0-data[i].leng))){
										entry1=1;
										write(newSocket,er1,1024);
									}
									if(start<0 && entry1==0){
										start=data[i].leng+start;
									}
									if(end<0 && entry1==0){
										end=data[i].leng+end;
									}
									if(entry1==0 && end>=start){
										write(newSocket,e3,1024);
										fp=fopen(file,"r");
										bzero(buf,1024);
										line=end-start+1;
										write(newSocket,&line,sizeof(int));
										while(fgets(buf,1024,fp)){
											if(count>=start && count<=end){
												write(newSocket,buf,1024);
											}
											count++;
										}
										fclose(fp);
									}
									else if(entry1==0){
										write(newSocket,er2,1024);
									}	
								}
								else{
									write(newSocket,e4,1024);
								}
								break;
							}
						}
						if(entry==0){
							write(newSocket,errr,1024);
						}
						
					}
					else if(strcmp(choice,"insert")==0){
						puts(input);
						int n_n=0,en=0;
						entry=0;
						entry1=0;
						int ki=0;
						count=0;
						printf("enter inserting\n");
						if(4==sscanf(input,"/%s %s %d %[^\n]",choice,file,&line,d)){
							for(i=0;i<inc;i++){
								if(strcmp(data[i].f_name,file)==0){
									entry=1;
									check1=verify(data[i].editor_id,c_id[m],ei[i]);
									if(data[i].owner_id==c_id[m] || check1==1){
									
										if(line>=data[i].leng || (line<0 && line<(0-data[i].leng))){
											entry1=1;
											write(newSocket,er1,1024);
										}
										if(line<0 && entry1==0){
											line=data[i].leng+line;
										}
										bzero(c,1024);
									
										if(entry1==0){
											write(newSocket,e3,1024);
											if(strlen(d)>2){
												en=1;
											}
											for(int j=1;j<strlen(d)-1;j++){
												if(d[j]=='\\' && d[j+1]=='n'){
													n_n=0;
													j++;
												}
												else{
													en=0;
												}
											}
											for(int j=1;j<strlen(d)-1;j++){
												c[j-1]=d[j];
											}
											if(strcmp(c,"\n")==0){
												strcpy(mes[0].cha,"");
												en=1;
											}
											for(int j=1;j<strlen(d)-1;j++){
												if(d[j]=='\\' && d[j+1]=='n'){
													mes[n_n].cha[ki]='\n';
													n_n++;
													ki=0;
													j++;
												}
												else{
													mes[n_n].cha[ki++]=d[j];
												}
											}
											if(n_n==0){
												fp=fopen(file,"r");
												while(fgets(buf,1024,fp)){
													if(count==line){
														c[strlen(c)]='\n';
														fputs(c,tmp);
													}
													fputs(buf,tmp);
												count++;
												}
												fclose(fp);
												rewind(tmp);
												
												line=0;
												while(fgets(buf,1024,tmp)){
													line++;
												}
												rewind(tmp);
												fp=fopen(file,"w");
												write(newSocket,&line,sizeof(int));
												bzero(buf,1024);
												while(fgets(buf,1024,tmp)){
													fputs(buf,fp);
													write(newSocket,buf,1024);
													bzero(buf,1024);
												}
												data[i].leng=line;
												fclose(fp);
												fclose(tmp);
											}
											else{
												fp=fopen(file,"r");
												while(fgets(buf,1024,fp)){
													if(count==line){
														if(en==1){
															for(int j=0;j<n_n;j++){
																fputs(mes[j].cha,tmp);
																bzero(mes[j].cha,1024);
															}
														}
														else{
															for(int j=0;j<=n_n;j++){
																fputs(mes[j].cha,tmp);
																bzero(mes[j].cha,1024);
															}
														}
													}
													fputs(buf,tmp);
													count++;
												}
												fclose(fp);
												rewind(tmp);
												line=0;
												while(fgets(buf,1024,tmp)){
													line++;
												}
												rewind(tmp);
												fp=fopen(file,"w");
												write(newSocket,&line,sizeof(int));
												bzero(buf,1024);
												while(fgets(buf,1024,tmp)){
													fputs(buf,fp);
													write(newSocket,buf,1024);
													bzero(buf,1024);
												}
												data[i].leng=line;
												fclose(fp);
												fclose(tmp);
											}
										}	
									}
									else{
										write(newSocket,e4,1024);
									}
									break;
								}
							}
							if(entry==0){
								write(newSocket,errr,1024);
							}
						}
						else if(3==sscanf(input,"/%s %s %[^\n]",choice,file,d)){
							for(i=0;i<inc;i++){
								if(strcmp(data[i].f_name,file)==0){
									entry=1;
									check1=verify(data[i].editor_id,c_id[m],ei[i]);
									if(data[i].owner_id==c_id[m] || check1==1){
										bzero(c,1024);
										if(entry1==0){
											write(newSocket,e3,1024);
											if(strlen(d)>2){
												en=1;
											}
											for(int j=1;j<strlen(d)-1;j++){
												if(d[j]=='\\' && d[j+1]=='n'){
													n_n=0;
													j++;
												}
												else{
													en=0;
												}
											}
											for(int j=1;j<strlen(d)-1;j++){
												c[j-1]=d[j];
											}
											if(strcmp(c,"\n")==0){
												strcpy(mes[0].cha,"");
												en=1;
											}
											for(int j=1;j<strlen(d)-1;j++){
												if(d[j]=='\\' && d[j+1]=='n'){
													mes[n_n].cha[ki]='\n';
													n_n++;
													ki=0;
													j++;
												}
												else{
													mes[n_n].cha[ki++]=d[j];
												}
											}
											if(n_n==0){
												fp=fopen(file,"r");
												puts("enter");
												while(fgets(buf,1024,fp)){
													fputs(buf,tmp);
												}
												fputs("\n",tmp);
												fputs(c,tmp);
												fclose(fp);
												rewind(tmp);
												line=0;
												while(fgets(buf,1024,tmp)){
													line++;
												}
												rewind(tmp);
												fp=fopen(file,"w");
												write(newSocket,&line,sizeof(int));
												bzero(buf,1024);
												while(fgets(buf,1024,tmp)){
													fputs(buf,fp);
													write(newSocket,buf,1024);
													bzero(buf,1024);
												}
												data[i].leng=line;
												fclose(fp);
												fclose(tmp);
											}
											else{
												fp=fopen(file,"r");
												while(fgets(buf,1024,fp)){
													fputs(buf,tmp);
												}
												fputs("\n",tmp);
												if(en==1){
													for(int j=0;j<n_n;j++){
														fputs(mes[j].cha,tmp);
														bzero(mes[j].cha,1024);
													}
												}
												else{
													for(int j=0;j<=n_n;j++){
														fputs(mes[j].cha,tmp);
														bzero(mes[j].cha,1024);
													}
												}
												fclose(fp);
												rewind(tmp);
												line=0;
												while(fgets(buf,1024,tmp)){
													line++;
												}
												rewind(tmp);
												fp=fopen(file,"w");
												write(newSocket,&line,sizeof(int));
												bzero(buf,1024);
												while(fgets(buf,1024,tmp)){
													fputs(buf,fp);
													write(newSocket,buf,1024);
													bzero(buf,1024);
												}
												data[i].leng=line;
												fclose(fp);
												fclose(tmp);
											}
										}	
									}
									else{
										write(newSocket,e4,1024);
									}
									break;
								}
							}
							if(entry==0){
								write(newSocket,errr,1024);
							}
						}
					}
					else if(strcmp(choice,"delete")==0){
						printf("delete data\n");
						entry=0;
						entry1=0;
						count=0;
						sscanf(input,"/%s %s %d %d",choice,file,&start,&end);
						for(i=0;i<inc;i++){
							if(strcmp(data[i].f_name,file)==0){
								entry=1;
								check1=verify(data[i].editor_id,c_id[m],ei[i]);
								if(data[i].owner_id==c_id[m] || check1==1){
									if(start>=data[i].leng || (start<0 && start<(0-data[i].leng)) || end>=data[i].leng || (end<0 && end<(0-data[i].leng))){
										entry1=1;
										write(newSocket,er1,1024);
									}
									if(start<0 && entry1==0){
										start=data[i].leng+start;
									}
									if(end<0 && entry1==0){
										end=data[i].leng+end;
									}
									if(entry1==0 && end>=start){
										write(newSocket,e3,1024);
										fp=fopen(file,"r");
										bzero(buf,1024);
//										line=end-start+1;
//										write(newSocket,&line,sizeof(int));
										while(fgets(buf,1024,fp)){
											if(count<start || count>end){
												fputs(buf,tmp);
											}
											count++;
										}
										fclose(fp);
										fp=fopen(file,"w");
										rewind(tmp);
										line=data[i].leng-(end-start+1);
										bzero(buf,1024);
										write(newSocket,&line,sizeof(int));
										while(fgets(buf,1024,tmp)){
											
											write(newSocket,buf,1024);
											fputs(buf,fp);
											bzero(buf,1024);	
										}
										data[i].leng=line;
										fclose(fp);
										fclose(tmp);
									}
									else if(entry1==0){
										write(newSocket,er2,1024);
									}	
								}
								else{
									write(newSocket,e4,1024);
								}
								break;
							}
						}
						if(entry==0){
							write(newSocket,errr,1024);
						}
					}
				}
				else if(3==sscanf(input,"/%s %s %s",choice,file,c)){
					if(strcmp(choice,"read")==0){
						printf("read data\n");
						entry=0;
						entry1=0;
						count=0;
						sscanf(input,"/%s %s %d",choice,file,&line);
						for(i=0;i<inc;i++){
							if(strcmp(data[i].f_name,file)==0){
								entry=1;
								check1=verify(data[i].editor_id,c_id[m],ei[i]);
								check2=verify(data[i].collab_id,c_id[m],ci[i]);
								if(data[i].owner_id==c_id[m] || check1==1 || check2==1){
									if(line>=data[i].leng || (line<0 && line<(0-data[i].leng))){
										entry1=1;
										write(newSocket,er1,1024);
									}
									if(line<0 && entry1==0){
										line=data[i].leng+line;
									}
									if(entry1==0){
										write(newSocket,e3,1024);
										fp=fopen(file,"r");
										bzero(buf,1024);
										while(fgets(buf,1024,fp)){
											if(line==count){
												write(newSocket,buf,1024);
												break;
											}
											count++;
										}
										fclose(fp);
									}	
								}
								else{
									write(newSocket,e4,1024);
								}
								break;
							}
						}
						if(entry==0){
							write(newSocket,errr,1024);
						}
					}
					else if(strcmp(choice,"insert")==0){
						printf("inserting data\n");
						entry=0;
						entry1=0;
						count=0;
						int n_n=0,en=0;
						int ki=0;
						sscanf(input,"/%s %s %s",choice,file,d);
						for(i=0;i<inc;i++){
							if(strcmp(data[i].f_name,file)==0){
								entry=1;
								check1=verify(data[i].editor_id,c_id[m],ei[i]);
								if(data[i].owner_id==c_id[m] || check1==1){
									bzero(c,1024);
									write(newSocket,e3,1024);
									if(strlen(d)>2){
										en=1;
									}
//									if(strlen(c)==2){
//										strcpy(c,"");
//									}
									for(int j=1;j<strlen(d)-1;j++){
										if(d[j]=='\\' && d[j+1]=='n'){
											n_n=0;
											j++;
										}
										else{
											en=0;
										}
									}
									for(int j=1;j<strlen(d)-1;j++){
										c[j-1]=d[j];
									}
									if(strcmp(c,"\n")==0){
										strcpy(mes[0].cha,"");
										en=1;
									}
									for(int j=1;j<strlen(d)-1;j++){
										if(d[j]=='\\' && d[j+1]=='n'){
											mes[n_n].cha[ki]='\n';
											n_n++;
											ki=0;
											j++;
										}
										else{
											mes[n_n].cha[ki++]=d[j];
										}
									}
									if(n_n==0){
										fp=fopen(file,"r");
										while(fgets(buf,1024,fp)){
											fputs(buf,tmp);
											count++;
										}
										fputs("\n",tmp);
										fputs(c,tmp);
//										fputs("\n",tmp);
										fclose(fp);
										rewind(tmp);
										line=0;
										while(fgets(buf,1024,tmp)){
											line++;
										}
										rewind(tmp);
										fp=fopen(file,"w");
										write(newSocket,&line,sizeof(int));
										bzero(buf,1024);
										while(fgets(buf,1024,tmp)){
											fputs(buf,fp);
											write(newSocket,buf,1024);
											bzero(buf,1024);
										}
										data[i].leng=line;
										fclose(fp);
										fclose(tmp);
									}
									else{
										fp=fopen(file,"r");
										while(fgets(buf,1024,fp)){
											fputs(buf,tmp);
										}
										fputs("\n",tmp);
										if(en==1){
											for(int j=0;j<n_n;j++){
												fputs(mes[j].cha,tmp);		
												bzero(mes[j].cha,1024);
											}
										}
										else{
											for(int j=0;j<=n_n;j++){
												fputs(mes[j].cha,tmp);
												bzero(mes[j].cha,1024);
											}
										}
										fclose(fp);
										rewind(tmp);
										line=0;
										while(fgets(buf,1024,tmp)){
											line++;
										}
										rewind(tmp);
										fp=fopen(file,"w");
										write(newSocket,&line,sizeof(int));
										bzero(buf,1024);
										while(fgets(buf,1024,tmp)){
											fputs(buf,fp);
											write(newSocket,buf,1024);
											bzero(buf,1024);
										}
										data[i].leng=line;
										fclose(fp);
										fclose(tmp);
									}	
								}
								else{
									write(newSocket,e4,1024);
								}
								break;
							}
						}
					
						if(entry==0){
							write(newSocket,errr,1024);
						}
					}
					else if(strcmp(choice,"delete")==0){
						printf("delete data\n");
						entry=0;
						entry1=0;
						count=0;
						sscanf(input,"/%s %s %d",choice,file,&line);
						for(i=0;i<inc;i++){
							if(strcmp(data[i].f_name,file)==0){
								entry=1;
								check1=verify(data[i].editor_id,c_id[m],ei[i]);
								if(data[i].owner_id==c_id[m] || check1==1){
									if(line>=data[i].leng || (line<0 && line<(0-data[i].leng))){
										entry1=1;
										write(newSocket,er1,1024);
									}
									if(line<0 && entry1==0){
										line=data[i].leng+line;
									}
									if(entry1==0){
										write(newSocket,e3,1024);
										fp=fopen(file,"r");
										bzero(buf,1024);
										while(fgets(buf,1024,fp)){
											if(line!=count){
												fputs(buf,tmp);
											}
											count++;
										}
										fclose(fp);
										rewind(tmp);
										fp=fopen(file,"w");
										bzero(buf,1024);
										line=(data[i].leng)-1;
										write(newSocket,&line,sizeof(int));
										while(fgets(buf,1024,tmp)){
											fputs(buf,fp);
											write(newSocket,buf,1024);
											bzero(buf,1024);
										}
										fclose(fp);
										fclose(tmp);
										data[i].leng=line;
									}	
								}
								else{
									write(newSocket,e4,1024);
								}
								break;
							}
						}
						if(entry==0){
							write(newSocket,errr,1024);
						}
					}
				}
				else if(2==sscanf(input,"/%s %s",choice,file)){
					entry=0;
					if(strcmp(choice,"upload")==0){
						printf("upload data\n");
						for(i=0;i<inc;i++){
							if(strcmp(file,data[i].f_name)==0){
								entry=1;
							}
						}
						if(entry==0){
							write(newSocket,e3,1024);
							fp=fopen(file,"w");
							bzero(buf,1024);
							read(newSocket,&count,sizeof(int));
							for(i=0;i<count;i++){
								read(newSocket,buf,1024);
								fputs(buf,fp);
								bzero(buf,1024);
							}
							fclose(fp);
							strcpy(data[inc].f_name,file);
							data[inc].owner_id=c_id[m];
//							data[inc].editor_id[ei[inc]++]=0;
//							data[inc].collab_id[ci[inc]++]=0;
							data[inc].leng=count;
							inc++;
							write(newSocket,succ,1024);
						}
						else{
							write(newSocket,er4,1024);
						}
					}
					else if(strcmp(choice,"download")==0){
						printf("download data\n");
						entry=0;
						for(i=0;i<inc;i++){
							if(strcmp(data[i].f_name,file)==0){
								entry=1;
								check1=verify(data[i].editor_id,c_id[m],ei[i]);
								check2=verify(data[i].collab_id,c_id[m],ci[i]);
								if(data[i].owner_id==c_id[m] || check1==1 || check2==1){
									
									write(newSocket,e3,1024);
									bzero(buf,1024);
									sprintf(buf,"%s",file);
									write(newSocket,buf,1024);
								}
								else{
									write(newSocket,e4,1024);
								}
								break;
							}
						}
						if(entry==0){
							write(newSocket,errr,1024);
						}
					}
					else if(strcmp(choice,"read")==0){
						printf("read\n");
						entry=0;
						for(i=0;i<inc;i++){
							if(strcmp(data[i].f_name,file)==0){
								entry=1;
								check1=verify(data[i].editor_id,c_id[m],ei[i]);
								check2=verify(data[i].collab_id,c_id[m],ci[i]);
								if(data[i].owner_id==c_id[m] || check1==1 || check2==1){
									write(newSocket,e3,1024);
									bzero(buf,1024);
									write(newSocket,&data[i].leng,sizeof(int));
									fp=fopen(file,"r");
									while(fgets(buf,1024,fp)){
										write(newSocket,buf,1024);
										bzero(buf,1024);
									}
									fclose(fp);
								}
								else{
									write(newSocket,e4,1024);
								}
								break;
							}
						}
						if(entry==0){
							write(newSocket,errr,1024);
						}
					}
					else if(strcmp(choice,"delete")==0){
						printf("delete\n");
						entry=0;
						for(i=0;i<inc;i++){
							if(strcmp(data[i].f_name,file)==0){
								entry=1;
								check1=verify(data[i].editor_id,c_id[m],ei[i]);
								if(data[i].owner_id==c_id[m] || check1==1){
									write(newSocket,e3,1024);
									bzero(buf,1024);
									fp=fopen(file,"w");
									data[i].leng=0;
									fclose(fp);
									write(newSocket,file,1024);
								}
								else{
									write(newSocket,e4,1024);
								}
								break;
							}
						}
						if(entry==0){
							write(newSocket,errr,1024);
						}
						
					}
				}
			}
		}
	}
		
	return 0;
}

