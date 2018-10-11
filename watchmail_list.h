#include <pthread.h>

struct MailNode  
{ 
  char* filepath;
  pthread_t thread_id;
  struct MailNode *next; 
};


struct MailNode* mailAppend(struct MailNode*, char*, pthread_t);

void freeMailNode(struct MailNode*);

struct MailNode* mailListRemoveNode(struct MailNode*, char*);


void mailTraverse(struct MailNode*);

void mailFreeAll(struct MailNode*);