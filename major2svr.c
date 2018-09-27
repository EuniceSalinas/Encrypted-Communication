/*
 MAJOR 2
 MAJOR GROUP 21
 COURSE: CSCE 3600 001
 DATE: May 03 2018
 TEAM MEMBERS:  1. Eunice Salinas
                2. Dheepthi Somangili Babulal
                3. Megdelawit Woldeselassie
 
 DESCRIPTION:   This is the server side code for major 2. It accepts two client
                connection.It receives two integers from the client and performs
                encryption by implementing the RSA algorithm. and generates private and
                public key. It sends the private key to the client it received p and q
                from. It sends the public key to the other client. In the case that any
                client disconnects then the server will accept another client. In the
                case where more that two clients are trying to connect then it will not
                allow that connection and display an error.
 
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define MAX 1025

int CheckPrime(int temp);//checks if number is prime
int gcd(int a, int b);//gdc for RSA

int main(int argc, char *argv[])
{
	//usage error
	if(argc != 2)
	{
		printf("usage:	./server <portaddr>\n");
		exit(EXIT_FAILURE);
	}

	int listenfd = 0, client = 0, client2 = 0, client3 = 0;//fds
	int cli_size, port; //client size and port 
	int client_count = 0;//will count the number of clients connected
	int temp, temp2;//used as temporary variables 
	int nread, nready;//used to check for errors
	int p, q, n, phi, e, d, i; //to be used to compute RSA
	struct sockaddr_in serv_addr, cli_addr; //for sockets
	int maxfd;//used for select, maximum fds in set
	int cli_id = 0;//flag to know wheter client 1 or client 2 is connected if only one client in server
	fd_set fds;//fd set

	char buf[MAX];//used for messages

	//set up listen socket
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket error\n");
		exit(EXIT_FAILURE);
	}

	//zero
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(buf, '0', sizeof(buf));

	//get port from command line
	port = atoi(argv[1]);

	//set up serv_addr
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;    
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serv_addr.sin_port = htons(port);

	//prevents bind errors
	int on = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
	//bind 
	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("Bind Failed");
		exit(EXIT_FAILURE);	
	}

	//listen for connection
	if (listen(listenfd, 100 ) == -1)
	{
		perror("Listen Failed");
		exit(EXIT_FAILURE);
	}
	
	printf("Waiting for Connections...\n");
	while(1)//infinite loop
	{
		//set up fd set
		cli_size = sizeof(cli_addr);
		FD_ZERO(&fds);
                FD_SET(listenfd, &fds);
                if(client_count == 1)
                {
			if(cli_id == 2)//if client 2 is in the server alone
			{
				FD_SET(client2, &fds);
			}else//client 1 is in server
			{
                        	FD_SET(client, &fds);
			}

                }else if(client_count == 2)//the two clients are in the server
                {
                        FD_SET(client, &fds);
                        FD_SET(client2, &fds);
                }

		//set up maxfd for select
                if(client_count == 0)//no clients in the server
                {
                        maxfd = listenfd + 1;
                }
                else if(client_count == 1)
                {
			if(cli_id == 2)//only client 2 is in server
			{
				maxfd = (listenfd > client2 ? listenfd : client2) + 1;
			}else//only client 1
			{
                        	maxfd = (listenfd > client ? listenfd : client) + 1;
                	}
		}else//both clients in server
                {
			if(listenfd > client)
                        {
                                if(listenfd > client2)
                                {
                                        maxfd = listenfd + 1;
                                }else
                                {
                                        maxfd = client2 +1;
                                }
                        }else
                        {
                                if(client > client2)
                                {
                                        maxfd = client + 1;
                                }else
                                {
                                        maxfd = client2 +1;
                                }
                        }

                }

		//wait for input
		nready = select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

		//if a client is trying to connect
		if(FD_ISSET(listenfd, &fds))
		{
			if(client_count == 0)//no clients in server
			{
				//accept client 1
				if((client = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_size)) == -1)
				{
					perror("Accept failed");
					close(client);
				}else
				{
					printf("Accepted Client Connection\n");
					bzero(buf, MAX);
					strcpy(buf, "You are now connected to the Server\n");
					write(client, buf, strlen(buf));
					client_count++;
					cli_id = 1;
					bzero(buf, MAX);
					strcpy(buf, "Enter CLIENT 1 Data (p and q prime, where p* q > 128): ");
					write(client, buf, strlen(buf));
					
				}

			}else if(client_count == 1 && cli_id == 1) //client 1 is connected
			{
				//accept client 2
				if((client2 = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_size)) == -1)
                                {
                                      	 perror("Accept failed");
                                         close(client2);
                                }else
                                {
                                       	printf("Accepted Client 2 Connection\n");
                                       	bzero(buf, MAX);
                                       	strcpy(buf, "You are now connected to the Server\n");
                                       	write(client2, buf, strlen(buf));
                                       	client_count++;
					bzero(buf, MAX);
                                        strcpy(buf, "Enter CLIENT 2 Data (p and q prime, where p * q > 128): ");
                                        write(client2, buf, strlen(buf));
                                }

			}else if(client_count == 1 && cli_id == 2)//client 2 is connected client 1 disconnected and its trying to connect again
			{
				//accept client 1
				if((client = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_size)) == -1)
                                {
                                        perror("Accept failed");
                                        close(client);
                                }else
                                {
                                        printf("Accepted Client Connection\n");
                                        bzero(buf, MAX);
                                        strcpy(buf, "You are now connected to the Server\n");
                                        write(client, buf, strlen(buf));
                                        client_count++;
                                        cli_id = 1;
                                        bzero(buf, MAX);
                                        strcpy(buf, "Enter CLIENT 1 Data (p and q prime, where p* q > 128): ");
                                        write(client, buf, strlen(buf));

                                }
	
			}else//another client trying to connect
			{
				//accept client 3 and then drop after error message
				if((client3 = accept(listenfd, (struct sockaddr*)&cli_addr, &cli_size)) == -1)
                                {
                                        perror("Accept failed");
                                        close(client3);
                                }

				printf("Attempt Connection by third Client\n");
                                bzero(buf, MAX);
                                strcpy(buf, "Too many clients cannot connect to server. Dropping....\n");
                                write(client3, buf, strlen(buf));

                                close(client3);

			}
		}
	
		//message from client 1
		if(FD_ISSET(client, &fds))
		{
			//read first intiger
			temp = 0;
			nread = recv(client, &temp, sizeof(temp), 0);
			
			//if zero send back a 0 and disconnect
			if(nread < 1 || temp == 0){
				printf("CLIENT 1 Disconnected\n");
				if(temp == 0)
				{
					bzero(buf, MAX);
					strcpy(buf, "0");
					write(client, buf, sizeof(buf));
					bzero(buf, MAX);
					strcpy(buf, "CLIENT 1 Disconnecting...\n");
					write(client, buf, sizeof(buf));
					
				}
				close(client);
				FD_CLR(client, &fds);
				client_count--;
				cli_id = 2;//only client 2 in server

			
			}else if(temp == 9999){
				//used to ask for data
				bzero(buf, MAX);
				strcpy(buf, "Enter CLIENT 1 Data (p and q prime, where p * q > 128: ");
				write(client, buf, strlen(buf));
			}else{
				//read second intiger
				temp2 = 0;
				nread = recv(client, &temp2, sizeof(temp2), 0);
				
				//check if prime or valid
				if((CheckPrime(temp) == 1 || CheckPrime(temp2) == 1) || (temp * temp2) < 128)
				{
					bzero(buf, MAX);
					strcpy(buf, "INVALID");
					write(client, buf, sizeof(buf));
				}else
				{
					//perform RSA
					printf("p = %d, q = %d ", temp, temp2);
					p = temp;
					q = temp2;
					n = p*q;
					phi = (p-1)*(q-1);
					e = 2;
					while(e < phi){
						if(gcd(e, phi) == 1){
							break;
						}else{
							e++;
						}
					}
				
					i = i;
					while(1){
						temp = (phi * i) + 1;
						if((temp % e) == 0){
							d = temp / e;
							break;
						}
						i++;
					}
					printf("n = %d, e = %d, phi = %d, d = %d\n", n, e, phi, d);
					
					//error case only one client in server
					if(client_count == 1){
						printf("ERROR only one client connected to server\n");
						bzero(buf, MAX);
						strcpy(buf, "0");
						write(client, buf, sizeof(buf));
						bzero(buf, MAX);
						strcpy(buf, "No other client, CLIENT 1 Disconnecting...\n");
						write(client, buf, sizeof(buf));
						close(client);
                                		FD_CLR(client, &fds);
                		                client_count--;
		                                cli_id = 2;
					}else{
						//send keys
						printf("Sending CLIENT 1 Public Key to Client 2\n");						
						//to client 2
						bzero(buf, MAX);
						strcpy(buf, "KEY ");
						write(client2, buf, sizeof(buf));
						write(client2, &d, sizeof(d));
						write(client2, &n, sizeof(n));
						
						//to client 1
						bzero(buf, MAX);
						strcpy(buf, "PRK ");
						write(client, buf, sizeof(buf));
						write(client, &e, sizeof(e));
						write(client, &n, sizeof(n));	
					}
				}
			}
		}

		//message from client 2
		if(FD_ISSET(client2, &fds))
		{
			//read first intiger
			temp = 0;
                        nread = recv(client2, &temp, sizeof(temp), 0);
			if(nread < 1 || temp == 0) //if zero disconnect
                        {
                                printf("CLIENT 2 Disconnected\n");
				if(temp == 0)
				{
					bzero(buf, MAX);
                                        strcpy(buf, "0");
                                        write(client2, buf, sizeof(buf));
                                        bzero(buf, MAX);
                                        strcpy(buf, "CLIENT 2 Disconnecting...\n");
                                        write(client2, buf, sizeof(buf));
                                }
				close(client2);
                                FD_CLR(client2, &fds);
                                client_count--;
                                cli_id = 1;
                        }else if(temp == 9999){
				//send prompt
                                bzero(buf, MAX);
                                strcpy(buf, "Enter CLIENT 2 Data (p and q prime, where p * q > 128: ");
                                write(client2, buf, strlen(buf));
			}else{
				//read second intiger
				temp2 = 0;
                                nread = recv(client2, &temp2, sizeof(temp2), 0);
				
				//check if prime or valid
                                if((CheckPrime(temp) == 1 || CheckPrime(temp2) == 1) || (temp * temp2) < 128)
                                {
                                        bzero(buf, MAX);
                                        strcpy(buf, "INVALID");
                                        write(client2, buf, sizeof(buf));
                                }else
                                {
					//preform RSA
                                        printf("p = %d, q = %d ", temp, temp2);
                                        p = temp;
                                        q = temp2;
                                        n = p*q;
                                        phi = (p-1)*(q-1);
                                        e = 2;
                                        while(e < phi){
                                                if(gcd(e, phi) == 1){
                                                        break;
                                                }else{
                                                        e++;
                                                }
                                        }

                                        i = i;
                                        while(1){
                                                temp = (phi * i) + 1;
                                                if((temp % e) == 0){
                                                        d = temp / e;
                                                        break;
                                                }
                                                i++;
                                        }
                                        printf("n = %d, e = %d, phi = %d d = %d\n", n, e, phi, d);

					//error case only one client in server
                                        if(client_count == 1){
                                                printf("ERROR only one client connected to server\n");
						bzero(buf, MAX);
						strcpy(buf, "0");
                                                write(client2, buf, sizeof(buf));
						bzero(buf, MAX);
                                                strcpy(buf, "No other client, CLIENT 2 Disconnecting...\n");
                                                write(client2, buf, sizeof(buf));
						close(client);
                                                FD_CLR(client, &fds);
                                                client_count--;
                                                cli_id = 2;
                                        }else{
						//send keys
                                                printf("Sending Client 2 Public Key to Client 1\n");

                                                //to client 1
                                                bzero(buf, MAX);
                                                strcpy(buf, "KEY ");
                                                write(client, buf, sizeof(buf));
                                                write(client, &d, sizeof(d));
                                                write(client, &n, sizeof(n));

                                                //to client 2
						bzero(buf, MAX);
						strcpy(buf, "PRK ");
						write(client2, buf, sizeof(buf));
                                                write(client2, &e, sizeof(e));
                                                write(client2, &n, sizeof(n));
                                        }
                                }
                        }
	
		}	
	}
	
	//close listen socket
	close(listenfd);
	return 0;
}


//checks if number is prime
int CheckPrime(int temp)
{
	int i, a;
	a = 0;

	for(i = 1; i <= temp; i++)
	{
		if(temp%i == 0)
		{
			a++;
		}
	}

	if(a == 2)
	{
		return 0;
	}else
	{
		return 1;
	}
}

//preforms gcd calculations
int gcd(int a, int b)
{
	int tmp;
	while(1){
		tmp = a % b;
		if(tmp == 0){
			return b;
		}
		a = b;
		b = tmp;
	}
	return 1;
}
