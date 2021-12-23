/** Compilation: gcc -o memreader memreader.c -lrt -lpthread **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>          
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "shmem.h"
#include <sys/time.h>
#include <time.h>

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

void shmemo(){
  //printf("girdim\n");
  int fd = shm_open(BackingFile, O_RDWR | O_CREAT, AccessPerms);  /* empty to begin */
  if (fd < 0) report_and_exit("Can't get file descriptor...");

  /* get a pointer to memory */
  caddr_t memptr = mmap(NULL,       /* let system pick where to put segment */
			ByteSize,   /* how many bytes */
			PROT_READ | PROT_WRITE, /* access protections */
			MAP_SHARED, /* mapping visible to other processes */
			fd,         /* file descriptor */
			0);         /* offset: start at 1st byte */
  if ((caddr_t) -1 == memptr) report_and_exit("Can't access segment...");

  /* create a semaphore for mutual exclusion */
  sem_t* semptrr = sem_open(ReadSemaphoreName, /* name */
			   O_CREAT,       /* create the semaphore */
			   AccessPerms,   /* protection perms */
			   0);            /* initial value */         

  if (semptrr == (void*) -1) report_and_exit("sem_open");

  /* use semaphore as a mutex (lock) by waiting for writer to increment it */
    int i;
    //printf("len: %d\n",strlen(MemContents));
    for (i = 0; i < ByteSize; i++)
      write(STDOUT_FILENO, memptr + i, 1); /* one byte at a time */
    
    //printf("yazdım\n"); 

  /* cleanup */
  munmap(memptr, ByteSize);

  close(fd);  
  shm_unlink(BackingFile);
  sem_post(semptrr);  
  sem_close(semptrr);  
}
 
int main() {
  printf("[PID:%d][%s][Server](Server has started.)\n",getpid(),getTime());
  sem_t* semptrw = sem_open(WriteSemaphoreName, /* name */
			   O_RDWR|O_CREAT,       /* create the semaphore */
			   AccessPerms,   /* protection perms */
			   0);            /* initial value */
  

  if (semptrw == (void*) -1) report_and_exit("sem_open");
  sem_post(semptrw);
  while (!sem_wait(semptrw))
  {
    shmemo();
    //sem_close(semptrw);
  }
  sem_close(semptrw);
  
  
  return 0;
}
 