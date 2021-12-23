#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <stdlib.h>
#include "queue.h"
#include <time.h>
#include <sys/time.h>

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

void readMsg(){
  //printf("yazdım1\n");
  key_t key= ftok(PathName, ProjectId); /* key to identify the queue */
  //printf("yazdım2\n");
  if (key < 0) report_and_exit("key not gotten...");
  //printf("yazdım3\n");
  
  int qid = msgget(key, 0666 | IPC_CREAT); /* access if created already */
  //printf("yazdım4\n");
  if (qid < 0) report_and_exit("no access to queue...");
  //printf("yazdım5\n");
  queuedMessage msg; /* defined in queue.h */
  //printf("yazdım6\n");
  while(msgrcv(qid, &msg, sizeof(msg), 1, MSG_NOERROR | IPC_NOWAIT) >=0){
    printf("%s\n", msg.payload);
  }
  //printf("yazdım7\n");
    
  

  /** remove the queue **/
  //if (msgctl(qid, IPC_RMID, NULL) < 0)  /* NULL = 'no flags' */
    //report_and_exit("trouble removing queue...");
  //printf("yazdım8\n");
}
  
int main() { 
  printf("[PID:%d][%s][Server](Server has started.)\n",getpid(),getTime());
  while(1){
    readMsg();
  }
  
  return 0; 
} 
