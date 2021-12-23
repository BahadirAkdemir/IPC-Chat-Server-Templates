#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
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

void readFile(){
  struct flock lock;
  lock.l_type = F_WRLCK;    /* read/write (exclusive) lock */
  lock.l_whence = SEEK_SET; /* base for seek offsets */
  lock.l_start = 0;         /* 1st byte in file */
  lock.l_len = 0;           /* 0 here means 'until EOF' */
  lock.l_pid = getpid();    /* process id */
  int fd; /* file descriptor to identify a file within a process */
  
  if ((fd = open(FileName, O_RDONLY)) < 0)  /* -1 signals an error */
    report_and_exit("open to read failed...");

  /* If the file is write-locked, we can't continue. */
  fcntl(fd, F_GETLK, &lock); /* sets lock.l_type to F_UNLCK if no write lock */
  if (lock.l_type != F_UNLCK)
    report_and_exit("file is still write locked...");

  while(10==10){
      lock.l_type = F_RDLCK; /* prevents any writing during the reading */
    if (fcntl(fd, F_SETLK, &lock) < 0)
      report_and_exit("can't get a read-only lock...");
    
    /* Read the bytes (they happen to be ASCII codes) one at a time. */
    int c; /* buffer for read bytes */
    while (read(fd, &c, 1) > 0){
      write(STDOUT_FILENO, &c, 1); /* write one byte to the standard output */
    }    /* 0 signals EOF */
    

    /* Release the lock explicitly. */
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) < 0)
      report_and_exit("explicit unlocking failed...");
    
    sleep(1);
    
  }
  close(fd);
}
void removeFile(){
  struct flock lock;
  lock.l_type = F_WRLCK;    /* read/write (exclusive versus shared) lock */
  lock.l_whence = SEEK_SET; /* base for seek offsets */
  lock.l_start = 0;         /* 1st byte in file */
  lock.l_len = 0;           /* 0 here means 'until EOF' */
  lock.l_pid = getpid();    /* process id */
  
  int fd; /* file descriptor to identify a file within a process */
  if ((fd=open(FileName,O_RDONLY | O_WRONLY|O_CREAT | O_TRUNC,0666)) < 0)  /* -1 signals an error */
    report_and_exit("open failed...");
  
  if (fcntl(fd, F_SETLK, &lock) < 0) /** F_SETLK doesn't block, F_SETLKW does **/
    report_and_exit("fcntl failed to get lock...");
  

  /* Now release the lock explicitly. */
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) < 0)
    report_and_exit("explicit unlocking failed...");
  
  close(fd);
}

int main() {
    printf("[PID:%d][%s][Server](Server has started.)\n",getpid(),getTime());
    removeFile();
    readFile(); 
  return 0;  
}
