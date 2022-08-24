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

// constant defining
#define PORT 4444
#define max_clients 5

// structure defining
struct bill{
	char item_name[256];
	char date[256];
	float price;
	char D_name[256];
	char prices[256];
}bill[2056];

//Date validation function
int validate_date(int dd, int mm, int yy) {
    if (mm < 1 || mm > 12) {
        return 0;
    }
    if (dd < 1) {
        return 0;
    }

    int days = 31;
    if (mm == 2) {
        days = 28;
        if (yy % 400 == 0 || (yy % 4 == 0 && yy % 100 != 0)) {
            days = 29;
        }
    } else if (mm == 4 || mm == 6 || mm == 9 || mm == 11) {
        days = 30;
    }

    if (dd > days) {
        return 0;
    }
    return 1;
}

// Main starts from here:
int main(int argc , char *argv[])
{
	// All variables used in program are defining
	
	int socket_id , add_len , new_socket , client_socket[5];
	int active, i , valread , sd;
	int max_sd,newSocket,m;
	struct sockaddr_in add;
	int option = 1;
	int limit=5;
	int total_conn=0;
	int entry;
	char error[]="Limit";
	char error1[]="files empty or length>1800";
	char errr[]="files not sorted";
	char e1[]="Invalid file content format";
	char e2[]="Invalid file content price format";
	char e3[]="Invalid file content date format";
	char e4[]="Invalid file content date";
	char input[1024]={0};
	char file_name1[1024]={0};
	char file_name2[1024]={0};
	char file_name3[1024]={0};
	char order[1024];
	char choice[100]={0};
	char buff[1024]={0};
	char buf[1024];
		
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
			if ((new_socket = accept(socket_id,(struct sockaddr *)&add, (socklen_t*)&add))<0)
			{
				perror("accept command error");
				exit(EXIT_FAILURE);
			}
			// checking total connections to server
			if(total_conn<max_clients){
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
					
				FILE *temp=tmpfile();
				FILE *temp1=tmpfile();
				FILE *fp;
				bzero(buf,sizeof(buf));
				bzero(input,sizeof(input));
				bzero(file_name1,1024);
				bzero(file_name2,1024);
				bzero(file_name3,1024);
				bzero(order,1024);
				bzero(choice,1024);
				read(newSocket, input, 1024);
				if(strcmp(input,"/exit") == 0)
				{
					getpeername(newSocket , (struct sockaddr*)&add , (socklen_t*)&add_len);
					printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(add.sin_addr) , ntohs(add.sin_port));
					total_conn--;
					close( newSocket );
					client_socket[m] = 0;
					break;
				}
					
				else if(5==sscanf(input,"%s %s %s %s %s",choice,file_name1,file_name2,file_name3,order)){ // merging process starts here
					printf("Enterd for merging\n");
					int count1,count2,lines=0;
					struct bill tp;
					int DD,MM,YY;
					struct bill bill1[1024];
					struct bill bill2[1024];
					struct bill bill3[1024];
					//Reading files and store them in temporary files
					read(newSocket,&count1,sizeof(int));
					while(count1!=lines){
						read(newSocket,buf,1024);
						fputs(buf,temp);
						lines++;
					}
					lines=0;
					read(newSocket,&count2,sizeof(int));
					while(count2!=lines){
						read(newSocket,buf,1024);
						fputs(buf,temp1);
						lines++;
					}
					if(count1!=0 && count2!=0 && count1<1800 && count2<1800){
					int size1,size2;
					rewind(temp);
					rewind(temp1);
					int i=0,k=0,j;
					int x=0,y=0,z=0;
					char tempo[512];
					float t1;
					char t2[512];
					//check for file format
					while(fgets(buf,1024,temp)){
						
						if(3!=sscanf(buf,"%s\t%[^\t]\t%s",bill1[i].date,bill1[i].item_name,tempo)){
							k=1;
							write(newSocket,e1,1024);
							break;
						}
						else if(2==sscanf(tempo,"%f%s",&t1,t2)){
							k=1;
							write(newSocket,e2,1024);
							break;
						}
						else if(3!=sscanf(bill1[i].date,"%d.%d.%d",&DD,&MM,&YY)){
							k=1;
							write(newSocket,e3,1024);
							break;
						}
						else if(!validate_date(DD,MM,YY)){
							k=1;
							write(newSocket,e4,1024);
							break;
						}
						else{
							sscanf(tempo,"%f",&bill1[i].price);
						}
						i++;
					}
					size1=i;
					i=0;
					if(k==0){
						while(fgets(buf,1024,temp1)){
							if(3!=sscanf(buf,"%s\t%[^\t]\t%s",bill2[i].date,bill2[i].item_name,tempo)){
								k=1;
								write(newSocket,e1,1024);
								break;
							}
							else if(2==sscanf(tempo,"%f%s",&t1,t2)){
								k=1;
								write(newSocket,e2,1024);
								break;
							}
							else if(3!=sscanf(bill2[i].date,"%d.%d.%d",&DD,&MM,&YY)){
								k=1;
								write(newSocket,e3,1024);
								break;
							}
							else if(!validate_date(DD,MM,YY)){
								k=1;
								write(newSocket,e4,1024);
								break;
							}
							else{
								sscanf(tempo,"%f",&bill2[i].price);
							}
							i++;
						}
					}
					size2=i;
					int en=0;
					
					//Check for files are in sorted mode
					if(k==0){
						if(strcmp(order,"N")==0 || strcmp(order,"item_name")==0){
							
							for(i=1;i<size1;++i){
								for(j=0;j<size1-1;j++){
									if(strcmp(bill1[j+1].item_name,bill1[j].item_name)<0){
										en=1;
										break;
									}
								}
							}
							for(i=1;i<size2;++i){
								for(j=0;j<size2-1;j++){
									if(strcmp(bill2[j+1].item_name,bill2[j].item_name)<0){
										en=1;
										break;
									}
								}
							}	
						}
						else if(strcmp(order,"P")==0 || strcmp(order,"price")==0){
							
							for(i=1;i<size1;++i){
								for(j=0;j<size1-1;j++){
									if(bill1[j+1].price<bill1[j].price){
										en=1;
										break;
									}
								}
							}
							for(i=1;i<size2;++i){
								for(j=0;j<size2-1;j++){
									if(bill2[j+1].price<bill2[j].price){
										en=1;
										
										break;
									}
								}
							}	
						}
						else{
							
							for(i=0;i<size1;i++){
								sscanf(bill1[i].date,"%d.%d.%d",&DD,&MM,&YY);
								sprintf(bill1[i].D_name,"%d%d%d",YY,MM,DD);
							}
							for(i=0;i<size2;i++){
								sscanf(bill2[i].date,"%d.%d.%d",&DD,&MM,&YY);
								sprintf(bill2[i].D_name,"%d%d%d",YY,MM,DD);
							}
							
							for(i=1;i<size1;++i){
								for(j=0;j<size1-1;j++){
									if(strcmp(bill1[j+1].D_name,bill1[j].D_name)<0){
										en=1;
										break;
									}
								}
							}
							for(i=1;i<size2;++i){
								for(j=0;j<size2-1;j++){
									if(strcmp(bill2[j+1].D_name,bill2[j].D_name)<0){
										en=1;
										break;
									}
								}
							}
						}
						//merge based on given field name
						if(en==1){
							write(newSocket,errr,1024);
						}
						else{
							char succ[]="No error in file format";
							write(newSocket,succ,1024);
							
							if(strcmp(order,"N")==0 || strcmp(order,"item_name")==0){
//								printf("hell\n");
								while(x<size1 && y<size2){
									if(strcmp(bill1[x].item_name,bill2[y].item_name)<0){
										bill3[z++]=bill1[x++];
//										printf("%s\n",bill1[x-1].item_name);
									}
									else{
										bill3[z++]=bill2[y++];
//										printf("%s\n",bill2[y-1].item_name);
									}
								}
								while(x<size1){
									bill3[z++]=bill1[x++];
//									printf("%s\n",bill1[x-1].item_name);
								}
								while(y<size2){
									bill3[z++]=bill2[y++];
//									printf("%s\n",bill2[y-1].item_name);
								}
							}
							else if(strcmp(order,"P")==0 || strcmp(order,"price")==0){
								while(x<size1 && y<size2){
									if(bill1[x].price<bill2[y].price){
										bill3[z++]=bill1[x++];
									}
									else{
										bill3[z++]=bill2[y++];
									}
								}	
								while(x<size1){
									bill3[z++]=bill1[x++];
								}
								while(y<size2){
									bill3[z++]=bill2[y++];
								}
							}
							else{
								while(x<size1 && y<size2){
									if(strcmp(bill1[x].D_name,bill2[y].D_name)<0){
										bill3[z++]=bill1[x++];
									}
									else{
										bill3[z++]=bill2[y++];
									}
								}
								while(x<size1){
									bill3[z++]=bill1[x++];
								}
								while(y<size2){
									bill3[z++]=bill2[y++];
								}
							}
							int total=size1+size2;
							write(newSocket,&total,sizeof(int));
							bzero(buf,1024);
							printf("total lines=%d\n",total);
							for(i=0;i<total;i++){
//								printf("%s\t%s\t%f\n",bill3[i].date,bill3[i].item_name,bill3[i].price);
								if(i==total-1){
									sprintf(buf,"%s\t%s\t%0.2f",bill3[i].date,bill3[i].item_name,bill3[i].price);
								}
								else{
									sprintf(buf,"%s\t%s\t%0.2f\n",bill3[i].date,bill3[i].item_name,bill3[i].price);
								}
								//printf("%s\n",buf);
								write(newSocket,buf,1024);
								bzero(buf,1024);
							}
							printf("merge finished\n");
							
						}
						
					}
				}
				else{
					
					write(newSocket,error1,1024);
					printf("files empty or over length\n");
				}
			}
				else if(3==sscanf(input,"%s %s %s",choice,file_name1,order)){ // sorting program based on field
					int count,lines=0;
					int DD,MM,YY;
					struct bill tp;
					//Reading file data into temporary file
					read(newSocket,&count,sizeof(int));
					while(count!=lines){
						bzero(buf,1024);
						read(newSocket,buf,1024);
						fputs(buf,temp);
						lines++;
					}
					if(count==0 || count>1800){
						printf("files empty or over length\n");
						
						if(write(newSocket,error1,1024)<=0){
							perror("In sending error");
							exit(1);
						}
					}
					else{
						lines=0;
						printf("entered for sorting\n");
						int size;
						rewind(temp);
						int i=0,k=0,j;
						char tempo[512];
						float t1;
						char t2[512];
						// check for file format
						while(fgets(buf,1024,temp)){
							if(strlen(buf)>1){
								if(3!=sscanf(buf,"%s\t%[^\t]\t%s",bill[i].date,bill[i].item_name,tempo)){
									k=1;
									write(newSocket,e1,1024);
									break;
								}
								else if(2==sscanf(tempo,"%f%s",&t1,t2)){
									k=1;
									write(newSocket,e2,1024);
									break;
								}
								else if(3!=sscanf(bill[i].date,"%d.%d.%d",&DD,&MM,&YY)){
									k=1;
									write(newSocket,e3,1024);
									break;
								}	
								else if(!validate_date(DD,MM,YY)){
									k=1;
									write(newSocket,e4,1024);
									break;
								}
								else{
									sscanf(tempo,"%f",&bill[i].price);
								}
								i++;
							}
						}
						rewind(temp);
						// sort based on file
						if(k==0){
							char succ[]="No error in file format";
							write(newSocket,succ,1024);
							size=i;
							if(strcmp(order,"N")==0 || strcmp(order,"item_name")==0){
							
								for(i=1;i<size;++i){
									for(j=0;j<size-1;j++){
										if(strcmp(bill[j+1].item_name,bill[j].item_name)<0){
											tp=bill[j];
											bill[j]=bill[j+1];
											bill[j+1]=tp;
										}
									}
								}
								bzero(buf,1024);
								count=size;
								write(newSocket,&count,sizeof(int));

								for(i=0;i<size;i++){
//									printf("%s\t%s\t%f\n",bill[i].date,bill[i].item_name,bill[i].price);
									if(i==size-1){
										sprintf(buf,"%s\t%s\t%0.2f",bill[i].date,bill[i].item_name,bill[i].price);
									}
									else{
										sprintf(buf,"%s\t%s\t%0.2f\n",bill[i].date,bill[i].item_name,bill[i].price);
									}
									write(newSocket,buf,1024);
									bzero(buf,1024);
								}
							}
							else if(strcmp(order,"P")==0 || strcmp(order,"price")==0){
								for(i=1;i<size;++i){
									for(j=0;j<size-1;j++){
										if(bill[j+1].price<bill[j].price){
											tp=bill[j];
											bill[j]=bill[j+1];
											bill[j+1]=tp;
										}
									}
								}
								bzero(buf,1024);
								count=size;
								write(newSocket,&count,sizeof(int));
								for(i=0;i<size;i++){
//									printf("%s\t%s\t%0.2f\n",bill[i].date,bill[i].item_name,bill[i].price);
									if(i==size-1){
										sprintf(buf,"%s\t%s\t%0.2f",bill[i].date,bill[i].item_name,bill[i].price);
									}
									else{
										sprintf(buf,"%s\t%s\t%0.2f\n",bill[i].date,bill[i].item_name,bill[i].price);
									}
								
									write(newSocket,buf,1024);
									bzero(buf,1024);
								}
							}
							else{
								int DD,MM,YY;
								for(i=0;i<size;i++){
									sscanf(bill[i].date,"%d.%d.%d",&DD,&MM,&YY);
									sprintf(bill[i].D_name,"%d%d%d",YY,MM,DD);
								}
								for(i=1;i<size;++i){
									for(j=0;j<size-1;j++){
										if(strcmp(bill[j+1].D_name,bill[j].D_name)<0){
											tp=bill[j];
											bill[j]=bill[j+1];
											bill[j+1]=tp;
										}
									}
								}
								bzero(buf,1024);
								count=size;
								write(newSocket,&count,sizeof(int));

								for(i=0;i<size;i++){
//									printf("%s\t%s\t%0.2f\n",bill[i].date,bill[i].item_name,bill[i].price);
									if(i==size-1){
										sprintf(buf,"%s\t%s\t%0.2f",bill[i].date,bill[i].item_name,bill[i].price);
									}
									else{
										sprintf(buf,"%s\t%s\t%0.2f\n",bill[i].date,bill[i].item_name,bill[i].price);
									}
									write(newSocket,buf,1024);
									bzero(buf,1024);
								}
							}
							printf("sort finished\n");
						}
						printf("\n");
						fclose(temp);
					}
					
				}
				else{ // code for similarity
					int count1,count2,lines=0;
					struct bill tp;
					int DD,MM,YY;
					struct bill bill1[1024];
					struct bill bill2[1024];
					// Reading files data into temporary files
					read(newSocket,&count1,sizeof(int));
					while(count1!=lines){
						read(newSocket,buf,1024);
						fputs(buf,temp);
						lines++;
					}
					lines=0;
					read(newSocket,&count2,sizeof(int));
					while(count2!=lines){
						read(newSocket,buf,1024);
						fputs(buf,temp1);
						lines++;
					}
					
					
					int size1,size2;
					rewind(temp);
					rewind(temp1);
					int i=0,k=0,j;
					int x=0,y=0,z=0;
					char tempo[512];
					char store[2056][1024];
					float t1;
					char t2[512];
					//check for file format
					if(count1!=0 && count2!=0 && count1<1800 & count2<1800){
					while(fgets(buf,1024,temp)){
						if(3!=sscanf(buf,"%s\t%[^\t]\t%s",bill1[i].date,bill1[i].item_name,tempo)){
							k=1;
							write(newSocket,e1,1024);
							break;
						}
						else if(2==sscanf(tempo,"%f%s",&t1,t2)){
							k=1;
							write(newSocket,e2,1024);
							break;
						}
						else if(3!=sscanf(bill1[i].date,"%d.%d.%d",&DD,&MM,&YY)){
							k=1;
							write(newSocket,e3,1024);
							break;
						}
						else if(!validate_date(DD,MM,YY)){
							k=1;
							write(newSocket,e4,1024);
							break;
						}
						else{
							sscanf(tempo,"%s",bill1[i].prices);
						}
						i++;
					}
					size1=i;
					i=0;
					if(k==0){
						while(fgets(buf,1024,temp1)){
							if(3!=sscanf(buf,"%s\t%[^\t]\t%s",bill2[i].date,bill2[i].item_name,tempo)){
								k=1;
								write(newSocket,e1,1024);
								break;
							}
							else if(2==sscanf(tempo,"%f%s",&t1,t2)){
								k=1;
								write(newSocket,e2,1024);
								break;
							}
							else if(3!=sscanf(bill2[i].date,"%d.%d.%d",&DD,&MM,&YY)){
								k=1;
								write(newSocket,e3,1024);
								break;
							}
							else if(!validate_date(DD,MM,YY)){
								k=1;
								write(newSocket,e4,1024);
								break;
							}
							else{
								sscanf(tempo,"%s",bill2[i].prices);
							}
							i++;
						}
						size2=i;
						int z=0;
						//Get similarity records and print to client
						if(k==0){
							char succ[]="No error in file format";
							write(newSocket,succ,1024);
							for(i=0;i<size1;i++){
									sscanf(bill1[i].date,"%d.%d.%d",&DD,&MM,&YY);
									sprintf(bill1[i].D_name,"%d%d%d",YY,MM,DD);
								}
							for(i=0;i<size2;i++){
									sscanf(bill2[i].date,"%d.%d.%d",&DD,&MM,&YY);
									sprintf(bill2[i].D_name,"%d%d%d",YY,MM,DD);
							}
							for(i=0;i<size1;i++){
								for(j=0;j<size2;j++){
									if(strcmp(bill1[i].item_name,bill2[j].item_name)==0 || strcmp(bill1[i].D_name,bill2[j].D_name)==0 || strcmp(bill1[i].prices,bill2[j].prices)==0){
										//printf("%s %s %s\t%s %s %s\n",bill1[i].item_name,bill1[i].prices,bill1[i].date,bill2[j].item_name,bill2[j].prices,bill2[j].date);
										sprintf(store[z++],"%s\t%s\t%s\t|\t%s\t%s\t%s\n",bill1[i].date,bill1[i].item_name,bill1[i].prices,bill2[j].date,bill2[j].item_name,bill2[j].prices);
									}
								}
							}
							int max=z;
							write(newSocket,&max,sizeof(int));
							for(i=0;i<max;i++){
								write(newSocket,store[i],1024);
							}
							printf("similarity finding finished\n");
						}
					}
				}
				else{
					printf("files empty or over length\n");
					write(newSocket,error1,1024);
				}
			}
		}
		}
	}
		
	return 0;
}

