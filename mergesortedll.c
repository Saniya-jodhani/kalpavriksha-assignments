#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node* mergeLists(Node *head1, Node *head2) {
    Node dummy;
    Node *tail = &dummy;
    dummy.next = NULL;

    while (head1 != NULL && head2 != NULL) {
        if (head1->data <= head2->data) {
            tail->next = head1;
            head1 = head1->next;
        }
        else {
            tail->next = head2;
            head2 = head2->next;
        }
        tail = tail->next;
    }

    if (head1 != NULL) {
        tail->next = head1;
    }
    else {
        tail->next = head2;
    }
    return dummy.next;

}