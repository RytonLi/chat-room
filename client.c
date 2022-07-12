/*************************************************************************
    > File Name: client.c
    > Author: Rytonli
    > Mail: rytonli@163.com
    > Created Time: Mon 11 Jul 2022 04:45:26 PM CST
 ************************************************************************/

#define _GNU_SOURCE 1
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<poll.h>
#include<fcntl.h>

#define BUFFER_SIZE 64

int main(int argc, char* argv[]){
	if(argc <= 2){
		printf("usage: %s ip_address port_number\n", basename(argv[0]));
		return 1;
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in server_address;
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	inet_pton(AF_INET, ip, &server_address.sin_addr);
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd >= 0);
	if(connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
		printf("connection failed\n");
		close(sockfd);
		return 1;
	}
	printf("connect success\n");
	
	struct pollfd fds[2];

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	fds[1].fd = sockfd;
	fds[1].events = POLLIN | POLLRDHUP;
	fds[1].revents = 0;

	char read_buf[BUFFER_SIZE];
	int pipefd[2];
	int ret = pipe(pipefd);
	assert(ret >= 0);

	while(1){
		ret = poll(fds, 2, -1);
		if(ret < 0){
			printf("poll failed\n");
			break;
		}
		
		if(fds[1].revents & POLLRDHUP){
			printf("server close the connection\n");
			break;
		}
		else if(fds[1].revents & POLLIN){
			memset(read_buf, 0, sizeof(read_buf));
			recv(fds[1].fd, read_buf, BUFFER_SIZE - 1, 0);
			printf("%s\n", read_buf);
		}

		if(fds[0].revents & POLLIN){
			ret = splice(STDIN_FILENO, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE);
			ret = splice(pipefd[0], NULL, sockfd, NULL, 32768, SPLICE_F_MORE);
		}
	}
	close(sockfd);

  return 0;
}
