#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int key;
    struct node *next;
} node;

typedef struct {
    node *head;
    int size;
} linked_list;

linked_list *new_linked_list(void) {
  linked_list *p = (linked_list *)calloc(1, sizeof(linked_list));
  // TODO: initialize struct if needed
  return p;
}

void pushFront(linked_list *p, int key) {
    node *n = (node *)calloc(1, sizeof(node));
    n -> key = key;
    n -> next = p -> head;
    p -> head = n;
    p -> size++;

}

void print(linked_list* l)
{
    node* now = l -> head;
    while (now != NULL){
        printf("%d", now->key);
        now = now->next;
        if(now != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}

void pushBack(linked_list* l, int key){
    node *n = (node *)calloc(1, sizeof(node));
    n -> key = key;
    node *prev = l -> head;
    while (prev -> next != NULL){
        prev = prev -> next;
    }
    prev -> next = n;
    l -> size++;
    print(l);

}

void popFront(linked_list *p){
    if (p -> size == 0) {
        printf("비어 있는 리스트\n");
        return;
    }
    p -> head = p -> head -> next;
    p -> size--;
    print(p);
       
}
void popBack(linked_list *p) {
    if (p -> size == 0) {
        printf("비어 있는 리스트\n");
        return;
    }
    node *prev;
    node *prev_next;
    prev = p -> head;
    prev_next = prev -> next;
    while (prev_next -> next != NULL) {
        prev = prev -> next;
        prev_next = prev -> next;
    }
    prev -> next = NULL;
    p -> size--;
    print(p);
}
node* search(linked_list *p, int key){
    node *s;
    int *search;
    s = p -> head;
    while (s -> next != NULL){
        
        if (s -> key == key) {
            break;
        }
        s = s -> next;
    }
    if (s -> key == key){
        return s;
    }
    else{
        node *s = (node *)calloc(1, sizeof(node));
        return s;
    }    

}
void delete(linked_list *p, int key){
    if (p -> size == 0) {
        printf("비어 있는 리스트\n");
        return;
    } 
    node *prev;
    node *prev_next;
    prev = p -> head;
    if (prev -> key == key) {
        p -> head = prev -> next;
        print(p);
        return;
    }
    
    while (prev -> next != NULL) {
        if (prev -> next -> key == key) {
            prev -> next = prev -> next -> next;
            p -> size--;
            printf("%d키 삭제 성공! 현재 길이 %d\n",key, p -> size);
            return;
        }
        prev = prev -> next;
    }
    printf("%d키 삭제 실패..",key);
}

int main() {
    linked_list *l = new_linked_list();
    node *n;
    l -> size = 0;
    popBack(l);
    popFront(l);
    delete(l,0);
    pushFront(l, 12);
    pushFront(l, 17);
    pushFront(l, 4);
    pushFront(l, 7);
    n = search(l, 17);
    if (n != NULL){
        printf("%d키를 찾았습니다\n", n -> key);
    }
    else {
        printf("키를 못찾았습니다.");
    }
    pushBack(l, 15);
    popBack(l);
    popFront(l);
    printf("%d\n", l -> size);
    // printf("%d", l -> head -> key);
    delete(l,17);
    delete(l,5);

}