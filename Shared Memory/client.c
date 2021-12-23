/** Compilation: gcc -o memwriter memwriter.c -lrt -lpthread **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>          
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "shmem.h"

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
void shmemo(char *name, char str[]){
  int fd = shm_open(BackingFile,      /* name from smem.h */
		    O_RDWR | O_CREAT| O_APPEND, /* read/write, create if needed */
		    AccessPerms);     /* access permissions (0644) */
  if (fd < 0) report_and_exit("Can't open shared mem segment...");

  ftruncate(fd, ByteSize); /* get the bytes */

  caddr_t memptr = mmap(NULL,       /* let system pick where to put segment */
			ByteSize,   /* how many bytes */
			PROT_READ | PROT_WRITE, /* access protections */
			MAP_SHARED, /* mapping visible to other processes */
			fd,         /* file descriptor */
			0);         /* offset: start at 1st byte */
  if ((caddr_t) -1  == memptr) report_and_exit("Can't get segment...");
  
  fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize - 1);
  fprintf(stderr, "backing file:       /dev/shm%s\n", BackingFile );

  /* semahore code to lock the shared mem */
  sem_t* semptrw = sem_open(WriteSemaphoreName, /* name */
			   O_CREAT,       /* create the semaphore */
			   AccessPerms,   /* protection perms */
			   0);            /* initial value */

  char str2[512];
  snprintf(str2, 512, "[PID:%d][%s][%s]:%s\n",getpid(),getTime(),name,str);
  //printf("string is: %s\n",str2);
  strcpy(memptr, str2); /* copy some ASCII bytes to the segment */
  //sleep(1);
  sem_post(semptrw);
  //sleep(1);
  /* clean up */
  munmap(memptr, sizeof(memptr)); /* unmap the storage */
  close(fd);
  sem_close(semptrw);
  //shm_unlink(BackingFile); /* unlink from the backing file */
}



int main() {  
  sem_t* semptrr = sem_open(ReadSemaphoreName, /* name */
        O_CREAT,       /* create the semaphore */
        AccessPerms,   /* protection perms */
        0); 
  if (semptrr == (void*) -1) report_and_exit("sem_open");

  printf("Write your name\n"); 
  char *name=malloc(100);
  scanf(" %[^\n]%*c",name);
  sem_post(semptrr);
  while (!sem_wait(semptrr))
  {  
    printf("Message: ");
    char str[100]; 
    scanf(" %[^\n]%*c",str);  
    shmemo(name,str);
  }
  sem_close(semptrr);
  
  return 0;  
} 


