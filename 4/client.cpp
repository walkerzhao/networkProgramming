/*
  *client.cpp
  *g++ client.cpp raw.c -o client
  *./client server_ip server_port username
  *time:2015-10-27
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <iostream>
#include <time.h>
#include <string>
#include <map>

using namespace std;


#include "raw.h"
#include "duckchat.h"

#define HOSTNAME_MAX 100
#define STDIN 0  			/* file descriptor for standard input */
#define MAX_MESSAGE_LEN 65536


int s; //socket
struct sockaddr_in server; //server address
int cont; //continue to loop
char active_channel[CHANNEL_MAX]; //active channel
map<string,string> subscribed_channels;
int send_login_message(char* username);
int send_join_message(char* channel);
int send_say_message(char* channel, char* text);
void handle_user_input();
void handle_server_input();
int send_leave_message(char* channel);
int send_logout_message();
int send_list_message();
int send_who_message(char* channel);
int send_keep_alive_message();



int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Usage: ./client server_socket server_port username\n");
		exit(1);
	}

	char hostname[HOSTNAME_MAX];
	int port;
	char username[USERNAME_MAX];

	strcpy(hostname, argv[1]);
	port = atoi(argv[2]);
	strcpy(username, argv[3]);

	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		perror ("socket() failed\n");
		exit(1);
	}
	else
	{
		//printf("socket created\n");
	}

	struct hostent *he;

	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if ((he = gethostbyname(hostname)) == NULL) {
		puts("error resolving hostname..");
		exit(1);
	}
	memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);

	//send login message
	int err = send_login_message(username);
	if (err)
	{
		printf("Message failed\n");
	}
	else
	{
		//printf("Message succeeded\n");
	}

	//send join message
	strcpy(active_channel,"Common");
	err = send_join_message(active_channel);
	if (err)
	{
		printf("Message failed\n");
	}
	else
	{
		//printf("Message succeeded\n");
	}

	printf(">"); //the prompt
	raw_mode(); //character mode
	fflush(stdout);

	cont = 1;

	while (cont)
	{
		//do this while continue and call the appropriate function

		fflush(stdout);
		int rc;
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(STDIN, &fds);
		FD_SET(s, &fds);

		rc = select(s+1, &fds, NULL, NULL, NULL);

		if (rc < 0)
		{
			printf("error in select\n");
		}
		else
		{
			if (FD_ISSET(STDIN,&fds))
			{
				handle_user_input();
			}

			if (FD_ISSET(s,&fds))
			{
				handle_server_input();
			}
		}

	}

	cooked_mode(); //line mode
	return 0;
}


int send_login_message(char* username)
{
	ssize_t bytes;
	void *data;
	size_t len;

	struct request_login login_msg;
	login_msg.req_type = REQ_LOGIN;
	strcpy(login_msg.req_username,username);
	data = &login_msg;

	len = sizeof login_msg;

	bytes = sendto(s, data, len, 0, (struct sockaddr*)&server, sizeof server);

	if (bytes < 0)
	{
		return 1; //error
	}
	else
	{
		return 0;
	}

}

int send_join_message(char* channel)
{
	ssize_t bytes;
	void *data;
	size_t len;

	struct request_join join_msg;
	join_msg.req_type = REQ_JOIN;
	strcpy(join_msg.req_channel,channel);
	data = &join_msg;

	len = sizeof join_msg;

	bytes = sendto(s, data, len, 0, (struct sockaddr*)&server, sizeof server);

	if (bytes < 0)
	{
		return 1; //error
	}
	else
	{
		strcpy(active_channel,channel);

		string joined_channel = string(channel);
		subscribed_channels[joined_channel] = joined_channel; //add to the list of subscribed channels

		return 0;

	}


}

int send_leave_message(char* channel)
{
	ssize_t bytes;
	void *data;
	size_t len;

	struct request_leave msg;
	msg.req_type = REQ_LEAVE;

	strcpy(msg.req_channel,channel);
	data = &msg;

	len = sizeof msg;

	bytes = sendto(s, data, len, 0, (struct sockaddr*)&server, sizeof server);

	if (bytes < 0)
	{
		return 1; //error
	}
	else
	{
		if (strcmp(channel,active_channel) == 0)
		{
			active_channel[0] = '\0';;
		}

		string leaving_channel = string(channel);
		map<string,string>::iterator channel_iter;
		channel_iter = subscribed_channels.find(leaving_channel);
		if (channel_iter != subscribed_channels.end())
		{
			subscribed_channels.erase(channel_iter);
		}
		return 0;
	}


}

int send_logout_message()
{
	ssize_t bytes;
	void *data;
	size_t len;

	struct request_logout msg;
	msg.req_type = REQ_LOGOUT;

	data = &msg;

	len = sizeof msg;

	bytes = sendto(s, data, len, 0, (struct sockaddr*)&server, sizeof server);

	if (bytes < 0)
	{
		return 1; //error
	}
	else
	{
		return 0;
	}

}

int send_list_message()
{
	ssize_t bytes;
	void *data;
	size_t len;

	struct request_logout msg;
	msg.req_type = REQ_LIST;

	data = &msg;

	len = sizeof msg;

	bytes = sendto(s, data, len, 0, (struct sockaddr*)&server, sizeof server);

	if (bytes < 0)
	{
		return 1; //error
	}
	else
	{
		return 0;
	}
}


int send_who_message(char* channel)
{
	ssize_t bytes;
	void *data;
	size_t len;

	struct request_leave msg;
	msg.req_type = REQ_WHO;
	strcpy(msg.req_channel,channel);
	data = &msg;

	len = sizeof msg;

	bytes = sendto(s, data, len, 0, (struct sockaddr*)&server, sizeof server);

	if (bytes < 0)
	{
		return 1; //error
	}
	else
	{
		return 0;
	}


}

int send_say_message(char* text)
{
	ssize_t bytes;
	void *data;
	size_t len;

	struct request_say say_msg;
	say_msg.req_type = REQ_SAY;
	strcpy(say_msg.req_channel,active_channel);
	strcpy(say_msg.req_text,text);
	data = &say_msg;

	len = sizeof say_msg;

	bytes = sendto(s, data, len, 0, (struct sockaddr*)&server, sizeof server);

	if (bytes < 0)
	{
		return 1; //error
	}
	else
	{
		return 0;
	}

}


void handle_user_input()
{
	int count = 0;
	char userinput[SAY_MAX];
	char c = '\0';

	while (c != '\n')
	{
		c = getchar();
		if ((c != '\n') && (count < SAY_MAX-1))
		{
			userinput[count] = c;
			putchar(c);
			count++;
		}

	}

	userinput[count] = '\0';
	printf("\n");

	if (userinput[0] == '/')
	{
		//special command
		char* remainder = strchr(userinput, '/');
		remainder = &remainder[1];
		
		if (strcmp(remainder, "exit") == 0)
		{
			int err = send_logout_message();
			if (err)
			{
				printf("Message failed\n");
			}
			cont = 0;
		}
		else if (strcmp(remainder, "list") == 0)
		{
			int err = send_list_message();
			if (err)
			{
				printf("Message failed\n");
			}
		}
		else
		{
			char* parameter = strchr(remainder, ' ');

			if (parameter != NULL)
			{
				if (strlen(parameter) > 1)
				{
					parameter = &parameter[1];

					if (strncmp(remainder, "join ", 5) == 0)
					{
						int err = send_join_message(parameter);
						if (err)
						{
							printf("Message failed\n");
						}
						else
						{
							strcpy(active_channel,parameter);
						}


					}
					else if (strncmp(remainder, "leave ", 6) == 0)
					{
						int err = send_leave_message(parameter);
						if (err)
						{
							printf("Message failed\n");
						}

					}
					else if (strncmp(remainder, "who ", 4) == 0)
					{
						int err = send_who_message(parameter);
						if (err)
						{
							printf("Message failed\n");
						}


					}
					else if (strncmp(remainder, "switch ", 7) == 0)
					{
						string switching_channel = string(parameter);
						map<string,string>::iterator channel_iter;
						channel_iter = subscribed_channels.find(switching_channel);
						if (channel_iter == subscribed_channels.end())
						{
							cout << "You have not subscribed to channel " << switching_channel << endl;
						}
						else
						{
							strcpy(active_channel, parameter);
						}

					}else
					{
						printf("*Unknown command");
						printf("\n");
					}

				}else
				{
					printf("*Unknown command");
					printf("\n");
				}


			}
			else
			{
				printf("*Unknown command");
				printf("\n");
			}

		}
	}
	else
	{
		//say message
		if (active_channel[0] != '\0')
		{
			int err = send_say_message(userinput);
			if (err)
			{
				printf("Message failed\n");
			}
			else
			{
				//printf("Message succeeded\n");
			}
		}

	}
	printf(">");
}

void handle_server_input()
{

	struct sockaddr_in recv_server;
	ssize_t bytes;
	void *data;
	size_t len;
	socklen_t fromlen;
	fromlen = sizeof(recv_server);
	char recv_text[MAX_MESSAGE_LEN];
	data = &recv_text;
	len = sizeof recv_text;

	bytes = recvfrom(s, data, len, 0, (struct sockaddr*)&server, &fromlen);

	if (bytes < 0)
	{
		perror ("recvfrom failed\n");
	}
	else
	{
		//printf("%s",(char *)data);
		//printf("\n");
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");


		struct text* recv_text;
		recv_text = (struct text*)data;
		text_t message_type = recv_text->txt_type;

		if (message_type == TXT_SAY)
		{
			struct text_say* say_text;
			say_text = (struct text_say*)data;
			printf("[");
			printf("%s",say_text->txt_channel);
			printf("][");
			printf("%s",say_text->txt_username);
			printf("]: ");
			printf("%s",say_text->txt_text);
			printf("\n>");
		}
		else if (message_type == TXT_LIST)
		{
			struct text_list* list_text;
			list_text = (struct text_list*)data;

			printf("Existing channels:\n");
			int no_of_channels = list_text->txt_nchannels;
			struct channel_info* channel;
			int i;
			channel = list_text->txt_channels;
			for (i=0; i<no_of_channels; i++)
			{
				printf(" ");
				printf("%s", (channel+i)->ch_channel);
				printf("\n");

			}

			printf(">");

		}
		else if (message_type == TXT_WHO)
		{
			struct text_who* who_text;
			who_text = (struct text_who*)data;

			printf("Users on channel ");
			printf("%s",who_text->txt_channel);
			printf(":\n");
			int no_of_users = who_text->txt_nusernames;
			struct user_info* user;
			int i;
			user = who_text->txt_users;
			for (i=0; i<no_of_users; i++)
			{
				printf(" ");
				printf("%s",(user+i)->us_username);
				printf("\n");


			}

			printf(">");

		}
		else if (message_type == TXT_ERROR)
		{
			struct text_error* error_text;
			error_text = (struct text_error*)data;

			printf("Error: ");
			printf("%s",error_text->txt_error);
			printf("\n");
			printf(">");

		}

	}

}


