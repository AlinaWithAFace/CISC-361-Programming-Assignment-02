
// A simple C program to introduce 
// a linked list 
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h>
#include "linked_list.h"

struct Node* append(struct Node* head, char* str){
    struct Node* current = head;
    

    if(head != NULL){
        while(current->next != NULL){
            current = current->next;
        }
    }

    struct Node* new = (struct Node*)malloc(sizeof(struct Node));
    new->data = (char*)malloc(strlen(str));
    new->next = NULL;
    strcpy(new->data, str);

    if(head != NULL){
        current->next = new;
    }else{
        head = new;
    }

    return head;
}

void traverse(struct Node* head, int num){
    struct Node* current = head;
    int i = 0;

    if(i == 0){
        while(current != NULL){
            printf("%s\n", current->data);
            current = current->next;
            i++;
        }
    }else{
        while(current != NULL && i < num){
            printf("%s\n", current->data);
            current = current->next;
            i++;
        }
    }
}

int find(struct Node* head, char* str){
    if(head != NULL){
        struct Node* current = head;

        while(current != NULL){
            if(strcmp(current->data, str) == 0){
                return 1;
            }
            current = current->next;
        }
    }

    return 0;
}

void update(struct Node* head, char* old_str, char* new_str){
    
}

void freeAll(struct Node* head){
    struct Node* current = head;
    while(current != NULL){
        struct Node* toDelete = current;
        //printf("%s\n", toDelete->data);
        current = current->next;
        //free(toDelete->data);
        free(toDelete->data);
        free(toDelete);
    }
}
  
// Program to create a simple linked  
// list with 3 nodes 
/*
int main() 
{ 
  struct Node* head = NULL; 

  for(int i = 0; i < 10; i++){
      head = append(head, "bbb");
  }

  traverse(head);

  freeAll(head);
  return 0; 
} 
*/