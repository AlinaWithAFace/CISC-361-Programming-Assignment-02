struct Node  
{ 
  char* data; 
  struct Node *next; 
};

struct Node* append(struct Node*, char*);

void traverse(struct Node*, int num);

int find(struct Node*, char* );

void update(struct Node*, char* old_str, char*);

void freeAll(struct Node*);