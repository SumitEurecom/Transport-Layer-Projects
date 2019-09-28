/*
	Initialise Winsock
*/

#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFFLEN 2000


int main(int argc , char *argv[])
{
	WSADATA wsa;
    SOCKET s;
    char *hostname = "www.timesofindia.indiatimes.com";
    char ip[100];
	struct hostent *he;
	struct in_addr **addr_list;
	int i;
    struct sockaddr_in server;
	char *message , server_reply[2000];
	int recv_size;
	int ret;

	printf("\nInitialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	if ( (he = gethostbyname( hostname ) ) == NULL)
	{
		//gethostbyname failed
		printf("gethostbyname failed : %d\n" , WSAGetLastError());
		return 1;
	}

	//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
	addr_list = (struct in_addr **) he->h_addr_list;

	for(i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(ip , inet_ntoa(*addr_list[i]) );
	}

	printf("%s resolved to : %s\n" , hostname , ip);

	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d\n" , WSAGetLastError());
	}

	printf("Socket created.\n");


	//server.sin_addr.s_addr = inet_addr("172.217.18.174"); // for www.google.com
	//server.sin_addr.s_addr = inet_addr("10.187.80.88"); // for another system
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); // for local host
    server.sin_family = AF_INET;
	//server.sin_port = htons( 80 ); // for any webpage
    server.sin_port = htons( 53000 ); // for localhost
    		//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return 1;
	}

	puts("Connected");

    //keep communicating with server
	while(1)
    {
        //Send some data
        //message = "GET / HTTP/1.1\r\n\r\n"; // for any webpage
        message = "Hello Server\n"; // for localhost
        //memset(message,0,BUFFLEN);
        if( send(s , message , strlen(message) , 0) < 0)
        {
            puts("Send failed\n");
            return 1;
        }
        puts("Data Sent\n");

        //Receive a reply from the server
		memset(server_reply,0,BUFFLEN);
        if( ret = recv(s , server_reply , BUFFLEN , 0) < 0)
        {
            puts("recv failed");
            break;
        }

        printf("%s", server_reply);

    }



	closesocket(s);
    WSACleanup();

	return 0;
}
