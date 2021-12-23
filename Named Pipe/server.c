/* Filename: fifoserver.c */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
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

void reader(){
   int fd;
   char str[512];
   sem_t* semptrr = sem_open("ReaderSemaphore", /* name */
			   O_CREAT,       /* create the semaphore */
			   0644,   /* protection perms */
			   0);            /* initial value */ 
   /* Create the FIFO if it does not exist */
   mkfifo("fifoChannel", 0666);
   fd = open("./fifoChannel", O_RDONLY);
   read(fd, str, sizeof(str));
   printf("%s\n", str);
   sem_post(semptrr);
   close(fd);
}

int main() {
  printf("[PID:%d][%s][Server](Server has started.)\n",getpid(),getTime());
   sem_t* semptrw = sem_open("WriterSemaphore", /* name */
			   O_RDWR|O_CREAT,       /* create the semaphore */
			   0644,   /* protection perms */
			   0);            /* initial value */
  

  if (semptrw == (void*) -1) report_and_exit("sem_open");
  sem_post(semptrw); 
  while (!sem_wait(semptrw))
  {
    reader();
    //sem_close(semptrw);
  }
  sem_close(semptrw);
  
  
  return 0;
}