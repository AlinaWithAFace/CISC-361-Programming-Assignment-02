struct UserNode  
{ 
  char* user;
  int logged_on;
  struct UserNode *next; 
};

struct UserNode* userAppend(struct UserNode*, char*);

struct UserNode* findUser(struct UserNode*, char*);

struct UserNode* userRemoveNode(struct UserNode*, char*);

void freeUserNode(struct UserNode*);

void userFreeAll(struct UserNode*); 
