#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
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
void Pipewriter(char *pipeName,char *name, char str[]){ 
  sem_t* semptrw = sem_open("WriterSemaphore", /* name */
			   O_CREAT,       /* create the semaphore */
			   0644,   /* protection perms */
			   0);            /* initial value */
  int fd = open(pipeName, O_CREAT | O_WRONLY); /* open as write-only */
  if (fd < 0) 
    report_and_exit("Error..");                       /** error **/
  char str2[512];
  snprintf(str2, 512, "[PID:%d][%s][%s]:%s",getpid(),getTime(),name,str);
  write(fd, str2, sizeof(str2));     
  sem_post(semptrw);
  close(fd);                                /* close pipe: generates an end-of-file */
  sem_close(semptrw);
}

int main() {
  char* pipeName = "./fifoChannel";
  mkfifo(pipeName, 0666);
  sem_t* semptrr = sem_open("ReaderSemaphore", /* name */
        O_CREAT,       /* create the semaphore */
        0644,   /* protection perms */
        0); 
  if (semptrr == (void*) -1) report_and_exit("sem_open");
  sem_post(semptrr);
  printf("Write your name\n");
  char *name=malloc(100); 
  scanf(" %[^\n]%*c",name);
  while (!sem_wait(semptrr)){
    printf("Message: ");
    char str[100];
    scanf(" %[^\n]%*c",&str);
    Pipewriter(pipeName,name,str);
  }
  sem_close(semptrr);
  unlink(pipeName); 

  return 0;
}
