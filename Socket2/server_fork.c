/*** server program using fork ****/

//Headerfiles declaration
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//constant declaration
#define PORT 4444
//structure declaration
struct bill{
	char item_name[256];
	char date[256];
	float price;
	char D_name[256];
	char prices[256];
}bill[2056];
//Date validation
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
// Main starts from here
int main(){
// variable initilization
	int socket_id;
	int option = 1;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buf[1024];
	pid_t childpid;
// socket creation
	socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_id < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Server Socket is created.\n");
	if( setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, (char *)&option,sizeof(option)) < 0 )
	{
		perror("error in setsockopt");
		exit(EXIT_FAILURE);
	}

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
//binding
	if(bind(socket_id, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
		printf("Error in binding.\n");
		exit(1);
	}
	printf("Bind to port %d\n", 4444);
//listening
	if(listen(socket_id, 5) == 0){
		printf("Listener Listening on port %d \n", PORT);
	}else{
		printf("Error in binding.\n");
	}
	char error[]="Limit";
	char error1[]="files empty or length>1800";
	char errr[]="files not sorted";
	char e1[]="Invalid file content format";
	char e2[]="Invalid file content price format";
	char e3[]="Invalid file content date format";
	char e4[]="Invalid file content date";
	int limit=5;
	int total_conn=0;
	int entry;
	char input[1024]={0};
	char file_name1[1024]={0};
	char file_name2[1024]={0};
	char file_name3[1024]={0};
	char order[1024];
	char choice[100]={0};
	char buff[1024]={0};
	
	while(1){
		newSocket = accept(socket_id, (struct sockaddr*)&newAddr, &addr_size);
		entry=0;
		if(newSocket < 0){
			exit(1);
		}
		int limit=5;
		if(newSocket>0){
			if(total_conn<limit){
				total_conn++;
				printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
				char suc[]="success";
				send(newSocket,suc,1024,0);
			}
			else{
				entry = 1;
				send(newSocket,error,1024,0);
			}
		}
		printf("total connections:%d limit=%d\n",total_conn,limit);
		//child creation
		if((childpid = fork()) == 0 && entry==0){
			close(socket_id);

			while(1){
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
				if(strcmp(input,"/exit")==0){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}
				else if(5==sscanf(input,"%s %s %s %s %s",choice,file_name1,file_name2,file_name3,order)){//merging program starts
					printf("enterd for merging\n");
					int count1,count2,lines=0;
					struct bill tp;
					int DD,MM,YY;
					struct bill bill1[1024];
					struct bill bill2[1024];
					struct bill bill3[1024];
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
					while(fgets(buf,1024,temp)){//file format error checking
						
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
						while(fgets(buf,1024,temp1)){ //file format error checking
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
					if(k==0){//sort checking
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
						if(en==1){
							write(newSocket,errr,1024);
						}
						else{//merge and return to client
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
				else if(3==sscanf(input,"%s %s %s",choice,file_name1,order)){//sort file starts here
					int count,lines=0;
					int DD,MM,YY;
					struct bill tp;
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
						while(fgets(buf,1024,temp)){//file format error checking
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
						if(k==0){//sort and return to client
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
				else{//similarity program starts here
					int count1,count2,lines=0;
					struct bill tp;
					int DD,MM,YY;
					struct bill bill1[1024];
					struct bill bill2[1024];
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
					if(count1!=0 && count2!=0 && count1<1800 && count2<1800){
					while(fgets(buf,1024,temp)){ //file format error checking
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
					if(k==0){//file format checking
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
						if(k==0){//finding similarity and printing
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
		}//child end
	}//while end
	total_conn--;

	close(newSocket);
    

	return 0;
}//main end
