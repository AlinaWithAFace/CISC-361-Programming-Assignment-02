/*
    James Skripchuk & Alina Christenbury
    CISC361
    Shell
*/


// A simple C program to introduce 
// a linked list 
//But also has support for keys and values for alias
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h>
#include "linked_list.h"

struct Node* append(struct Node* head, char* str, char* key){
    struct Node* current = head;
    

    if(head != NULL){
        while(current->next != NULL){
            current = current->next;
        }
    }

    struct Node* new = (struct Node*)malloc(sizeof(struct Node));
    new->data = (char*)malloc(strlen(str));
    if(key == NULL){
        new->key = NULL;
    }else{
       new->key = (char*)malloc(strlen(key));
       strcpy(new->key, key); 
    }

    new->next = NULL;
    strcpy(new->data, str);

    if(head != NULL){
        current->next = new;
    }else{
        head = new;
    }

    return head;
}

void traverse(struct Node* head, int num, int keys){
    struct Node* current = head;
    int i = 0;

    while(current != NULL && i < num){
        if(keys == 1){
            printf("%s (%s)\n", current->data, current->key);
        }else{
            printf("%s\n", current->data);
        }
        current = current->next;
        i++;
    }
}

//Returns key given a value
char* find(struct Node* head, char* str){
    if(head != NULL){
        struct Node* current = head;

        while(current != NULL){
            if(strcmp(current->data, str) == 0){
                char* toReturn = (char*)malloc(strlen(str));
                strcpy(toReturn, current->key);
                return toReturn;
            }
            current = current->next;
        }
    }

    return NULL;
}


void freeAll(struct Node* head){
    struct Node* current = head;
    while(current != NULL){
        struct Node* toDelete = current;
        current = current->next;
        if(toDelete->key != NULL){
            free(toDelete->key);
        }
        free(toDelete->data);
        free(toDelete);
    }
}