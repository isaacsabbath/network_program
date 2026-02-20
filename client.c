# include <sys/socket.h> /* basic socket definitions */
# include <sys/types.h> 
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <stdarg.h>	/* for variadic function */
# include <errno.h>
# include <fcntl.h>
# include <sys/time.h>
# include <sys/ioctl.h>
# include <netdb.h>

// Standard HTTP port
# define SERVER_PORT 80
							/*Preprocessor macros for readability */
# define MAX_LINE 4096 // size of the buffer(length) to read where the data goes
#define SA struct sockaddr

void err_n_die (const char *fmt, ...); //wrapper function to handle errors

int main(int argc, char  **argv)
{
	int 	sockfd, n; //to create a TCP socket
	int 	sendbytes;
	struct 	sockaddr_in	servaddr; // IPV4 genric socket addr structure
	char 	sendline[MAX_LINE];
	char	recvline[MAX_LINE];

	//ussage check
	if (argc != 2)
		err_n_die("usage: %s <server address>", argv[0]);
	//socket creation
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_n_die("Error while creating the socket");

	bzero(&servaddr, sizeof(servaddr)); // zero out the address
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT); //host to network

	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <=0)
		err_n_die("inet_pton error for %s ", argv[1]); // translates the text ip addr into a  binary

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_n_die("connect failed"); //connects to the server 

	// were connected 
	sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
	sendbytes = strlen(sendline);
	
	//send the request -- making sure you send it all
	if(write(sockfd, sendline, sendbytes) != sendbytes) // write the request into that socket
		err_n_die("write error"); // response from the server

	memset(recvline, 0, MAX_LINE);

	while((n = read(sockfd, recvline, MAX_LINE-1)) > 0)
	{
		printf("%s",recvline );

	}
	if(n < 0)
		err_n_die("read error");

	exit(0); //end successfully
}	

void err_n_die(const char *fmt, ...)
{
	int errno_save;
	va_list ap;

	// any system or library call can set errno, so we need to save it now
	errno_save = errno;

	//print out the fmt+args to standard out

	va_start(ap, fmt);
	vfprintf(stdout,fmt,ap);
	fprintf(stdout, "\n");
	fflush(stdout);

	if (errno_save != 0)
	{
		fprintf(stdout, "(erno = %d) : %s\n", errno_save,
		strerror(errno_save));
		fprintf(stdout, "\n");
		fflush(stdout);
	}
	va_end(ap);

	exit(1);
}