#include <stdio.h> 
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <time.h>
#include <sys/time.h>
#include "queue.h"

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
void send(char *name, char str[],sem_t* semptrr){
  //printf("gridm2\n");
  key_t key = ftok(PathName, ProjectId); 
  //printf("gridm2\n");
  if (key < 0) report_and_exit("couldn't get key...");
  //printf("gridm2\n");
  
  int qid = msgget(key, 0666 | IPC_CREAT); 
  //printf("gridm2\n");
  if (qid < 0) report_and_exit("couldn't get queue id...");
  //printf("gridm2\n");

  char str2[512];
  //printf("gridm2\n");
  snprintf(str2, 512, "[PID:%d][%s][%s]:%s",getpid(),getTime(),name,str);
  //printf("gridm2\n");
  
  queuedMessage msg;
  msg.type = 1;
  strcpy(msg.payload, str2);
  //printf("gridm\n");
  msgsnd(qid, &msg, sizeof(msg), IPC_NOWAIT); 
  printf("%s sent as type %i\n", msg.payload, (int) msg.type);
  //printf("gridm2\n");
  sem_post(semptrr);
}

int main() {
  sem_t* semptrr = sem_open("ReaderSemaphore", /* name */
        O_CREAT,       /* create the semaphore */
        0644,   /* protection perms */
        0);
  sem_post(semptrr); 
  if (semptrr == (void*) -1) report_and_exit("sem_open");

  printf("Write your name\n");
  char *name=malloc(100);
  scanf(" %[^\n]%*c",name);
  while (!sem_wait(semptrr)){
    printf("Message: ");
    char str[100];
    scanf(" %[^\n]%*c",&str);
    send(name,str,semptrr);
  }
  return 0;
}

