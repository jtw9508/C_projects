#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int key;
    struct node *prev;
    struct node *next;
} node;

typedef struct {
    node *head;
    int size;
} dll;

dll *new_dll(void) {
    dll *p = (dll *)calloc(1, sizeof(dll));
    node *n = (node *)calloc(1, sizeof(node));
    p -> head = n;
    p -> head -> next = p -> head;
    p -> head -> prev = p -> head;
    p -> size = 0;
    return p;
}

void print(dll *p){
    if (p -> size == 0){
        printf("빈 리스트");
        return;
    }
    node *prev;
    prev = p -> head;
    while (prev -> next != p -> head){
        printf("%d -> ", prev -> key);
        prev = prev -> next;
    }
    printf("%d -> ", prev -> key);
    printf("%d\n", prev -> next -> key);
}

void splice(dll *p, node *s, node *l, node *i){
    s -> prev -> next = l -> next;
    l -> next -> prev  = s -> prev;
    l -> next = i -> next;
    l -> next -> prev = l; 
    i -> next = s;
    s -> prev = i;
}

void moveAfter(dll *p, node *a, node *x){
    splice(p, a, a, x);
    printf("%d노드를 %d노드 뒤로 이동했습니다\n",a->key, x ->key);
    print(p);
}
void moveBefore(dll *p, node *a, node *x){
    splice(p,a,a,x->prev);
    printf("%d노드를 %d노드 앞으로 이동했습니다\n",a->key, x ->key);
    print(p);
}
void insertAfter(dll *p, node *a, int key){
    node *i = (node *)calloc(1, sizeof(node));
    i -> next = i;
    i -> prev = i;
    i -> key = key;
    moveAfter(p, i, a);
}

void insertBefore(dll *p, node *a, int key){
    node *i = (node *)calloc(1, sizeof(node));
    i -> next = i;
    i -> prev = i;
    i -> key = key;
    moveBefore(p, i, a);
}

void pushFront(dll *p, int key){
    p -> size++;
    insertAfter(p, p->head, key);
}

void pushBack(dll *p, int key){
    p -> size++;
    insertBefore(p, p->head, key);
}

void delete(dll *p, int key){
    if (p -> size == 0){
        printf("비어 있는 리스트\n");
        return;
    }
    node *prev;
    prev = p -> head -> next;
    int i = 0;
    int stop = p -> size;
    while (i < stop){
        if (prev -> key == key) {
            prev -> prev -> next = prev -> next;
            prev -> next -> prev = prev -> prev;
            printf("%d노드 삭제 완료!\n", prev -> key);
            p -> size--;
            print(p);
            printf("%d\n", p -> size);
            free(prev);
            return;
        }
        prev = prev -> next;
        i++;
    }

    printf("%d노드가 존재하지 않습니다.\n",key);
}
node *popFront(dll *p){
    if (p -> size ==0){
        printf("비어 있는 리스트\n");
        return;
    }
    node *n;
    n = p -> head -> next; // 현재 더미노드 다음 노드 저장
    p -> head -> next = p -> head -> next -> next;
    p -> head -> next -> prev = p -> head;
    return n;    
}

node *popBack(dll *p){
    if (p -> size ==0){
        printf("비어 있는 리스트\n");
        return;
    }
    node *n;
    n = p -> head -> prev;
    p -> head -> prev = p -> head -> prev -> prev;
    p -> head -> prev -> next = p -> head;
    return n;
}

node* search(dll *p, int key){
    node* s = p -> head -> next;
    int i = 0;
    int stop = p -> size;
    print(p);
    while (i < stop){
        if (s -> key == key){
            return s;
        }
        printf("%d, %d\n", s -> key, key);
        s = s -> next;
        i++;
    }
    printf("%d노드를 찾지 못하였습니다.\n", key);
    node* t;
    t -> key = key;
    return t;
}
int isEmpty(dll *p){
    if (p -> size == 0){
        printf("비어있는 리스트\n");
        return 1;
    }
    else{
        printf("비어있지 않은 리스트\n");
        return 0;
    }
}
node* first(dll *p){
    node *f;
    f = p -> head -> next;
    printf("%d\n", p -> head -> next -> key);
    return f;
}

node* last(dll *p){
    node *f;
    f = p -> head -> prev;
    printf("%d\n", p -> head -> prev -> key);
    return f;
}

void join(dll *p, int key, dll *q){

}
void split(dll *p, int key){

} 

int main() {
    dll *l = new_dll();
    // first(l);
    // int b = isEmpty(l);
    // printf("%d\n", b);
    pushFront(l,3);
    // pushBack(l,2);
    // delete(l,1);
    // delete(l,2);
    // first(l);
    // last(l);
    node *k = search(l, 7);
    node *a = search(l, 3);
    // printf("%d %d\n", s -> key, a -> key);
    // pushFront(l,4);
    // pushFront(l,5);
    // node* n_1;
    // node* n_2;
    // n_1 = popFront(l);
    // n_2 = popBack(l);
    // printf("첫번째 삭제 노드 :%d\n두번째 삭제 노드: %d\n", n_1 -> key, n_2 -> key);
    // print(l);

}