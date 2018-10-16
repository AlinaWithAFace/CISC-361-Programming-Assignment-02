#include<stdio.h> 
#include<stdlib.h> 
#include<string.h>
#include "watchuser_list.h"

struct UserNode* userAppend(struct UserNode* head, char* user){
    struct UserNode* current = head;
    

    if(head != NULL){
        while(current->next != NULL){
            current = current->next;
        }
    }

    struct UserNode* new = (struct UserNode*)malloc(sizeof(struct UserNode));

    new->user = user;
    new->logged_on = 0;

    new->next = NULL;
    strcpy(new->user, user);

    if(head != NULL){
        current->next = new;
    }else{
        head = new;
    }

    return head;
}

struct UserNode* findUser(struct UserNode* head, char* user){
    struct UserNode* current = head;

    while(current != NULL){
        if(strcmp(user, current->user) == 0){
            return current;
        }
        current = current->next;
    }

    return NULL;
}

struct UserNode* userListRemoveNode(struct UserNode* head, char* user){
    struct UserNode* current = head;

    //At head of list
    if(strcmp(current->user, user) == 0){
        head = current->next;
        freeUserNode(current);
        return head;
    }

    while(current!=NULL){
        struct UserNode* next = current->next;

        if(next != NULL){
            if(strcmp(next->user, user) == 0){
                current->next = next->next;
                freeUserNode(next);
            }
        }

        current = next;
    }

    return head;
}

void freeUserNode(struct UserNode* node){
    free(node->user);
    free(node);
}

void userFreeAll(struct UserNode* head){
    struct UserNode* current = head;
    while(current != NULL){
        struct UserNode* toDelete = current;
        current = current->next;
        free(toDelete->user);
        free(toDelete);
    }
}