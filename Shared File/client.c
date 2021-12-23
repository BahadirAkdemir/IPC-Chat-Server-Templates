#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> 

#define FileName "data.dat" 

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

void writeMessage(char *name, char str[]){
  struct flock lock;
  lock.l_type = F_WRLCK;    /* read/write (exclusive versus shared) lock */
  lock.l_whence = SEEK_SET; /* base for seek offsets */
  lock.l_start = 0;         /* 1st byte in file */
  lock.l_len = 0;           /* 0 here means 'until EOF' */
  lock.l_pid = getpid();    /* process id */
  
  int fd; /* file descriptor to identify a file within a process */
  if ((fd = open(FileName, O_RDWR | O_CREAT | O_APPEND, 0666)) < 0)  /* -1 signals an error */
    report_and_exit("open failed...");
  
  if (fcntl(fd, F_SETLK, &lock) < 0) /** F_SETLK doesn't block, F_SETLKW does **/
    report_and_exit("fcntl failed to get lock...");
  else {
    char str2[100];
    snprintf(str2, 100, "[PID:%d][%s][%s]:%s\n",getpid(),getTime(),name,str);
    write(fd, str2, strlen(str2)); /* populate data file */
    fprintf(stderr, "Process %d has written to data file...\n", lock.l_pid);
  }

  /* Now release the lock explicitly. */
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) < 0)
    report_and_exit("explicit unlocking failed...");
  
  close(fd); /* close the file: would unlock if needed */
}
int main() {
  printf("Write your name\n");
  char *name=malloc(100);
  scanf(" %[^\n]%*c",name);
  while (1==1)
  {
    char str[100];
    printf("Message: ");
    scanf(" %[^\n]%*c",&str);
    writeMessage(name,&str);
  }
  
  return 0;  
}
