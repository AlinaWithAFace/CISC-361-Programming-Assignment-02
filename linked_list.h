//
// Created by alina on 2018-09-24.
//

#ifndef CISC_361_PROGRAMMING_ASSIGNMENT_02_LINKED_LIST_H
#define CISC_361_PROGRAMMING_ASSIGNMENT_02_LINKED_LIST_H

typedef struct node {
    int val;
    struct node *next;
} node_t;

void print_list(node_t *head);

int pop(node_t **head);

int remove_by_value(node_t **head, int val);

void delete_list(node_t *head);

int test_list(void);

#endif //CISC_361_PROGRAMMING_ASSIGNMENT_02_LINKED_LIST_H
