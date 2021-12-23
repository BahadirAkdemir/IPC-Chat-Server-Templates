#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <semaphore.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include "sock.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const char* books[] = {"War and Peace",
		       "Pride and Prejudice",
		       "The Sound and the Fury"};

void report(const char* msg, int terminate) {
  perror(msg);
  if (terminate) exit(-1); /* failure */
}
void handler(){
  printf("person with pid:%d is about to terminate",getpid());
  _exit(0);
}

void set_handler() {
  struct sigaction current;
  sigemptyset(&current.sa_mask);         /* clear the signal set */
  current.sa_flags = 0;                  /* enables setting sa_handler, not sa_action */
  current.sa_handler = handler;         /* specify a handler */
  sigaction(SIGTERM, &current, NULL);    /* register the handler */
}

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1); /* EXIT_FAILURE */
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

void writeSocket(char *name, char str[],sem_t* semptrr){
  char str2[512];
  snprintf(str2, 512, "[PID:%d][%s][%s]:%s\\",getpid(),getTime(),name,str);
  /* fd for the socket */
  int sockfd = socket(AF_INET,      /* versus AF_LOCAL */
		      SOCK_STREAM,  /* reliable, bidirectional */
		      0);           /* system picks protocol (TCP) */
  if (sockfd < 0) report("socket", 1); /* terminate */

  /* get the address of the host */
  struct hostent* hptr = gethostbyname(Host); /* localhost: 127.0.0.1 */ 
  if (!hptr) report("gethostbyname", 1); /* is hptr NULL? */
  if (hptr->h_addrtype != AF_INET)       /* versus AF_LOCAL */
    report("bad address family", 1);
  
  /* connect to the server: configure server's address 1st */
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = 
     ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
  saddr.sin_port = htons(PortNumber); /* port number in big-endian */
  
  if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0)
    report("connect", 1);
  
  /* Write some stuff and read the echoes. */
  //puts("Connect to server, about to write some stuff...");
  int i;
  char str4[512];
  sprintf(str4,"%d\\",getpid());
  for (i = 0; i < 3; i++) {
    if(i==0){
      if (write(sockfd, str2, strlen(str2)) > 0) {
      /* get confirmation echoed from server and print */
      char buffer[BuffSize + 1];
      memset(buffer, '\0', sizeof(buffer));
    }
    }
    if(i==1){
      if (write(sockfd, str4, strlen(str4)) > 0) {
      /* get confirmation echoed from server and print */
      char buffer[BuffSize + 1];
      memset(buffer, '\0', sizeof(buffer));
    }
    }
    if(i==2){
      if (write(sockfd, str, strlen(str)) > 0) {
      /* get confirmation echoed from server and print */
      char buffer[BuffSize + 1];
      memset(buffer, '\0', sizeof(buffer));
    }
    }
  }
  //puts("Client done, about to exit...");
  sem_post(semptrr);
  close(sockfd); /* close the connection */
}
int main() {
  sem_t* semptrr = sem_open("ReaderSemaphore", /* name */
        O_CREAT,       /* create the semaphore */
        0644,   /* protection perms */
        1);
  sem_post(semptrr); 
  if (semptrr == (void*) -1) report_and_exit("sem_open");

  printf("Write your name\n");
  char *name=malloc(100);
  scanf(" %[^\n]%*c",name);
  while (!sem_wait(semptrr)){
    printf("Message: ");
    char str[512];
    scanf(" %[^\n]%*c",&str);
    writeSocket(name,str,semptrr);
  }
  return 0;
}


