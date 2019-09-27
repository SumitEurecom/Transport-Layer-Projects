/*
	Initialise Winsock
*/
#include<pthread.h> //for threading , link with lpthread
#include<io.h>
#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#ifdef __unix__
# include <unistd.h>
#elif defined _WIN32
# include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#endif

void *connection_handler(void *);

int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;
	char* message;
	int *new_sock;

	printf("\nInitialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return 1;
	}
    printf("Initialised.\n");

	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d\n" , WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 52000 );

	//Bind
	if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d" , WSAGetLastError());
	}

	puts("Bind done\n");
	//Listen to incoming connections
    listen(s , 3);

    //Accept and incoming connection
	puts("Waiting for incoming connections...\n");

	c = sizeof(struct sockaddr_in);
	while ( (new_socket = accept(s , (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
    {
        puts("Connection accepted\n");

        char *client_ip = inet_ntoa(client.sin_addr);
        int client_port = ntohs(client.sin_port);

        puts(client_ip);
        printf("client_port: %d\n",client_port);

        //Reply to client
        message = "Hello Client , I have received your connection. But I have to go now, bye\n";
        send(new_socket , message , strlen(message) , 0);

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        if( pthread_create( &sniffer_thread , NULL, connection_handler, (void*)new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
        //pthread_join(sniffer_thread, NULL);

    }

	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d\n" , WSAGetLastError());
	}

//	getchar();
	closesocket(s);
    WSACleanup();

	return 0;

}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;

    int read_size;
    char *message , client_message[2000];

    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    send(sock , message , strlen(message),0);

    message = "Its my duty to communicate with you";
    send(sock , message , strlen(message),0);

    message = "Now type something and i shall repeat what you type \n";
    send(sock , message , strlen(message),0);

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        send(sock , client_message , strlen(client_message),0);
        sleep(2);
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    //Free the socket pointer
    free(socket_desc);

    return 0;
}
