#include<stdio.h> 
#include<stdlib.h> 
#include<string.h>
#include "watchmail_list.h"
#include <pthread.h>

struct MailNode* mailAppend(struct MailNode* head, char* filepath, pthread_t id){
    struct MailNode* current = head;
    

    if(head != NULL){
        while(current->next != NULL){
            current = current->next;
        }
    }

    struct MailNode* new = (struct MailNode*)malloc(sizeof(struct MailNode));

    new->filepath = filepath;

    new->next = NULL;
    strcpy(new->filepath, filepath);
    new->thread_id = id;

    if(head != NULL){
        current->next = new;
    }else{
        head = new;
    }

    return head;
}

void mailTraverse(struct MailNode* head){
    struct MailNode* current = head;
    
    while(current->next != NULL){
        //printf("AAAAAAAAAA\n");
        printf("File: %s Id: %i\n", current->filepath, current->thread_id);
        current = current->next;
    }
}

struct MailNode* mailListRemoveNode(struct MailNode* head, char* filepath){
    struct MailNode* current = head;

    //At head of list
    if(strcmp(current->filepath, filepath) == 0){
        head = current->next;
        printf("AAAA\n");
        freeMailNode(current);
        //printf("BBB\n");
        return head;
    }

    while(current!=NULL){
        struct MailNode* next = current->next;

        if(next != NULL){
            if(strcmp(next->filepath, filepath) == 0){
                current->next = next->next;
                freeMailNode(next);
            }
        }

        current = next;
    }

    return head;
}

void freeMailNode(struct MailNode* node){
    printf("%i, %s\n",node->thread_id,node->filepath);
    pthread_cancel(node->thread_id);
    pthread_join(node->thread_id, NULL);
    free(node->filepath);
    free(node);
}




void mailFreeAll(struct MailNode* head){
    struct MailNode* current = head;
    while(current != NULL){
        struct MailNode* toDelete = current;
        current = current->next;
        pthread_cancel(toDelete->thread_id);
        pthread_join(toDelete->thread_id, NULL);
        free(toDelete->filepath);
        free(toDelete);
    }
}