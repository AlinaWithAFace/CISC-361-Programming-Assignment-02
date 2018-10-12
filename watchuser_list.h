struct UserNode  
{ 
  char* user;
  struct UserNode *next; 
};

struct UserNode* userAppend(struct UserNode*, char*);

struct UserNode* userRemoveNode(struct UserNode*, char*);

void freeUserNode(struct UserNode*);
