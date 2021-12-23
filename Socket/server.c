#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "sock.h"
#include <time.h>
#include <sys/time.h>

void report(const char* msg, int terminate) {
  perror(msg);
  if (terminate) exit(-1); /* failure */
}

char * getTime(){

    time_t rawtime;
    struct tm * timeinfo;
    struct timeval tp;
    gettimeofday(&tp,0);

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char * str=malloc(100);
    sprintf(str,"[%02d:%02d:%02d:%03ld]",timeinfo->tm_hour, timeinfo->tm_min,timeinfo->tm_sec,tp.tv_usec/1000);
    return str;

}

void readSocket(){
  int fd = socket(AF_INET,     /* network versus AF_LOCAL */
		  SOCK_STREAM, /* reliable, bidirectional: TCP */
		  0);          /* system picks underlying protocol */
  if (fd < 0) report("socket", 1); /* terminate */
  	
  /* bind the server's local address in memory */
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));          /* clear the bytes */
  saddr.sin_family = AF_INET;                /* versus AF_LOCAL */
  saddr.sin_addr.s_addr = htonl(INADDR_ANY); /* host-to-network endian */
  saddr.sin_port = htons(PortNumber);        /* for listening */
  
  if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
    report("bind", 1); /* terminate */
	
  /* listen to the socket */
  if (listen(fd, MaxConnects) < 0) /* listen for clients, up to MaxConnects */
    report("listen", 1); /* terminate */

  fprintf(stderr, "Listening on port %i for clients...\n", PortNumber);
  /* a server traditionally listens indefinitely */
  while (1) {
    struct sockaddr_in caddr; /* client address */
    int len = sizeof(caddr);  /* address length could change */
    
    int client_fd = accept(fd, (struct sockaddr*) &caddr, &len);  /* accept blocks */
    if (client_fd < 0) {
      report("accept", 0); /* don't terminated, though there's a problem */
      continue;
    }

    /* read from client */
    int i;
    for (i = 0; i < ConversationLen; i++) {
      char buffer[BuffSize + 1];
      memset(buffer, '\0', sizeof(buffer)); 
      int count = read(client_fd, buffer, sizeof(buffer));
      if (count > 0) {
	puts(buffer);
	write(client_fd, buffer, sizeof(buffer)); /* echo as confirmation */
      }
    }
    close(client_fd); /* break connection */
  }  /* while(1) */
}

int main() {
  printf("[PID:%d][%s][Server](Server has started.)\n",getpid(),getTime());
  readSocket();
  return 0;
}


