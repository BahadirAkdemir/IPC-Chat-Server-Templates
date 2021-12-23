#define ProjectId 123
#define PathName  "queue.h" /* any existing, accessible file would do */
#define MsgLen    512
#define MsgCount  6

typedef struct { 
  long type;                 /* must be of type long */ 
  char payload[MsgLen + 1];  /* bytes in the message */  
} queuedMessage;
