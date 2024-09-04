#include "rbtree.h"

#include <stdlib.h>
void rotate_left(rbtree *t, node_t *n);
void rotate_right(rbtree *t, node_t *n);
void insert_case1(rbtree *t, node_t *n);

//rbtree 객체를 생성하는 코드. SENTINEL 노드를 사용하여 초기화
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *SENTINEL = (node_t *)malloc(sizeof(node_t));

  SENTINEL -> color = RBTREE_BLACK;
  SENTINEL -> left = SENTINEL;
  SENTINEL -> right = SENTINEL;
  SENTINEL -> parent = SENTINEL;

  p -> nil = SENTINEL;
  p -> root = p -> nil;
  // TODO: initialize struct if needed
  return p;
}

// 노드의 할아버지 노드를 찾는 코드.
node_t *grandparent(rbtree *t, node_t *n) {
  if ((n != t -> nil) && (n -> parent != t -> nil)) {
    return n -> parent -> parent;
  }
  else {
    return t -> nil;
  }
}

// 노드의 삼촌을 찾는 코드
node_t *uncle(rbtree *t, node_t *n) {
  node_t *g = grandparent(t, n);
  if (g == t -> nil) {
    return t -> nil;
  }
  if (n -> parent == g -> left) {
    return g -> right;
  }
  else {
    return g -> left;
  }
}
// 삽입 : case 1-5까지의 경우마다 해당 함수 실행. 1-3까지 재귀적으로 실행하여 트리 속성 4번을 해결할 위치까지 올라가고, 만약 꺾였다면 펴주고 회전하면 트리는 모든 속성을 만족

// 삽입 케이스 5. 4에서 red가 g의 left 혹은 right에만 있도록 만들어주고 p(빨강)의 색은 검정, g(검정)의 색을 빨강으로 만들고 회전 
void insert_case5(rbtree *t, node_t *n) {
  node_t *g = grandparent(t, n);

  n -> parent -> color = RBTREE_BLACK;
  g -> color = RBTREE_RED;
  if (n == n -> parent -> left) {
    rotate_right(t, g);
  }
  else {
    rotate_left(t, g);
  }
}

//왼쪽 회전하는 코드.
void rotate_left(rbtree *t, node_t *n) {
  node_t *c = n -> right;
  node_t *p = n -> parent;
  
  if (c -> left != t -> nil) {
    c -> left -> parent = n;
  }
  
  n -> right = c -> left;
  n -> parent = c;
  c -> left = n;
  c -> parent = p;
  
  if (p != t -> nil) {
    if (p -> left == n) {      
      p -> left = c;
    }
    else { 
      p -> right = c;
      
    }
  } else {
    t -> root = c;
  }
}

//오른쪽 회전하는 코드.
void rotate_right(rbtree *t, node_t *n) {
  node_t *c = n -> left;
  node_t *p = n -> parent;  
  if (c -> right != t -> nil) {
    c -> right -> parent = n;
  }
  
  n -> left = c -> right;
  n -> parent = c;     
  c -> right = n;
  c -> parent = p;
  
  if (p != t -> nil) {
    if (p -> right == n) {
      p -> right = c;
    }
    else {
      p -> left = c;
    }
  } else {
    t -> root = c;
  }
}

//삽입 케이스 4. 만약 꺾인 경우( 1)g.left = p and p.right = n 2)g.right = p and p.left = n) 펴주는 코드.
void insert_case4(rbtree *t, node_t *n) {
  node_t *g = grandparent(t, n);
  if ((n == n -> parent -> right) && (n -> parent == g -> left)) {
    rotate_left(t, n -> parent);
    n = n -> left;
  } else if ((n == n -> parent -> left) && (n -> parent == g -> right)){
    rotate_right(t, n -> parent);
    n = n -> right;
  }
  insert_case5(t, n);
}

void insert_case3(rbtree *t, node_t *n) {
  // 만약 부모의 색깔이 빨간색이라면, 4번 속성을 만족시키지 못한다. 재귀적으로 루트노드까지 색깔을 바꿔준다.
  // 색깔을 재귀적으로 변경해주다가 uncle의 색깔이 black이라면 회전을 시키게 된다.  
  node_t *u = uncle(t, n), *g;
  if ((u != t -> nil) && (u -> color == RBTREE_RED)){
    n -> parent -> color = RBTREE_BLACK;
    u -> color = RBTREE_BLACK;
    g = grandparent(t,n);
    g -> color = RBTREE_RED;
    insert_case1(t, g);
  }
  else {
    insert_case4(t, n);
  }
}

void insert_case2(rbtree *t, node_t *n) {
  //만약 부모의 색깔이 검정색이라면, rbtree의 5가지 속성을 다 만족시킨다.
  if (n -> parent -> color == RBTREE_BLACK) {
    return;
  }
  else {
    insert_case3(t, n);
  }
}

void insert_case1(rbtree *t, node_t *n) {
  //만약 부모가 없는 노드라면 -> 루트 노드이므로 색깔이 black이어야 한다.
  if (n -> parent == t -> nil){
    n -> color = RBTREE_BLACK;
  }
  else {
    insert_case2(t, n);
  }
}

//노드 생성 함수. 생성 시 빨간 노드를 생성해서 삽입 시 5번 속성을 깨지 않도록 함. 부모,자식은 모두 트리의 SENTINEL을 가리킴.
node_t *node_create(rbtree *t, const key_t key){
  node_t *n = (node_t *)malloc(sizeof(node_t));
  n -> color = RBTREE_RED;
  n -> key = key;
  n -> parent = t -> nil;
  n -> left = t -> nil;
  n -> right = t -> nil;
  return n;
}

// 트리에 삽입하는 함수. 노드를 생성하고 우선 BST와 동일하게 leaf에 삽입. 그 후 case 1~5까지 실행하여 균형을 맞춤.
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new_node = node_create(t, key); //노드 생성. 색은 빨강, left,right는 nil을 가리킴
  node_t *now = t -> root;
  
  //만약 root가 sentinel 즉, tree가 비어 있었다면
  if (now == t -> nil) {
    t -> root = new_node;
    insert_case1(t,new_node);
    return t -> root;
  }

  //1. 우선 bst와 동일하게 tree의 leaf에 노드를 추가 
  while (now != t -> nil) {
    if (key >= now -> key){
      if (now -> right == t -> nil) {
        now -> right = new_node;
        new_node -> parent = now;
        break;      
      }
      now = now -> right;
    }
    else {
      if (now -> left == t -> nil) {
        now -> left = new_node;
        new_node -> parent = now;
        break;
      }
      now = now -> left;
    }
  }  
  //2. leaf에 추가된 상태에서 case별로 rbtree의 속성을 만족시키게끔 tree 조정

  insert_case1(t, new_node);
  return t -> root;
}

// 노드를 삭제하는 함수. nil이 아닐 때까지 재귀적으로 돌면서 free
void delete_node(rbtree *t, node_t *n) {
  if (n != t -> nil) {
    delete_node(t, n -> left);
    delete_node(t, n -> right);
    free(n);
  }
}

// 트리를 삭제하는 함수. 우선 트리의 노드들을 모두 삭제한 후, nil과 t를 free
void delete_rbtree(rbtree *t) {
  if (t != NULL) {
    delete_node(t, t -> root);
    free(t -> nil);
    free(t);  
  }
}

//키에 해당하는 노드를 찾는 함수. 있으면 노드 포인터 return,없으면 NULL 포인터 return
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *now = t -> root;
  while (now != t -> nil) {
    if (key == now -> key){
      return now;
    }
    if (key > now -> key) {
      now = now -> right;
    }
    else {
      now = now -> left;
    }
  }
  return NULL;
}

//트리 최소값 노드 return
node_t *rbtree_min(const rbtree *t) {
  node_t *now = t -> root;
  while (1) {
    if (now -> left == t -> nil) {
      break;
    }
    now = now -> left;
  }
  return now;  
}

//트리 최댓값 노드 return
node_t *rbtree_max(const rbtree *t) {
  node_t *now = t -> root;
  while (1) {
    if (now -> right == t -> nil) {
      break;
    }
    now = now -> right;
  }
  return now;
}

//트리 형제 노드 return
node_t* sibling(rbtree *t,node_t *n) {
  if (n-> parent == t -> nil) {
    return t -> nil;
  }
  if (n == n -> parent -> left) {
    return n -> parent -> right;
  }
  else {
    return n -> parent -> left;
  }
}

//현재 노드가 SENTINEL인지 확인. 추후 SUCCESSOR의 자식을 확인할 때 사용
int is_leaf(rbtree* t, node_t* n) {
  return (n == t -> nil) ? 1 : 0;
}

//현재 구현에서 삭제할 노드에 SUCCESSOR의 값을 넣고, SUCCESSOR를 삭제하는 문제로 치환하였음.
//SUCCESSOR를 삭제하고 자식 노드를 SUCCESSOR의 부모와 연결해준 코드
void replace_node(rbtree *t, node_t* n, node_t* child) {
  if (n -> parent == t -> nil) {
    t -> root = child;
  } else if (n == n -> parent -> left) {
    n -> parent -> left = child;
  } else {
    n -> parent -> right = child;
  }
  child -> parent = n -> parent;
}

void delete_case1(rbtree* t, node_t* n);

//5에서 설명했던 회전을 수행.
void delete_case6(rbtree* t, node_t* n) {
  node_t *s  = sibling(t, n);
  s -> color = n -> parent -> color;
  n -> parent -> color = RBTREE_BLACK;
  if (n == n -> parent -> left) {
    s -> right -> color = RBTREE_BLACK;
    rotate_left(t, n -> parent);
  } else {
    s -> left -> color = RBTREE_BLACK;
    rotate_right(t, n -> parent);
  }
}
// 만약 s가 검정색이라면, s의 오른쪽 자녀가 빨간색 노드를 갖도록 만들면, p를 기준으로 회전시킨다. s가 p의 색을 물려받고, p가 검정, s의 자식이 검정색이 되면 균형이 맞춰지게 된다.
// case5에서는 s의 자식이 모두 검정이 아니고, s의 오른쪽 자식이 검정이고 왼쪽자식이 빨강일 때 회전시켜서 오른쪽 자식을 빨강으로 만든다.
void delete_case5(rbtree* t, node_t* n) {
  node_t *s = sibling(t, n);
  if (s -> color == RBTREE_BLACK) {
    if ((n == n -> parent -> left) && (s -> right -> color == RBTREE_BLACK) &&
    (s -> left -> color == RBTREE_RED)) {
      s -> color = RBTREE_RED;
      s -> left -> color = RBTREE_BLACK;
      rotate_right(t, s);
    } else if ((n == n -> parent -> right) && (s -> left -> color == RBTREE_BLACK) && (s -> right -> color == RBTREE_RED)) {
      s -> color = RBTREE_RED;
      s -> right -> color = RBTREE_BLACK;
      rotate_left(t, s);
    }
  }
  delete_case6(t, n);
}
//만약 s와 n이 검정색이고 p가 빨강이라면, s를 빨강,p를 검정으로 하면 s의 갯수는 변화가 없는데,n의 검정 갯수가 하나 늘어서 모든 특성을 만족하게 된다. 
//이것은 s의 자식이 둘 다 검정색일 때만 가능한 경우이다.
void delete_case4(rbtree* t, node_t* n) {
  node_t *s = sibling(t, n);
  if ((n -> parent -> color == RBTREE_RED) && (s -> color == RBTREE_BLACK) &&
  (s -> left -> color == RBTREE_BLACK) && (s -> right -> color ==RBTREE_BLACK)) {
    s -> color = RBTREE_RED;
    n -> parent -> color = RBTREE_BLACK;
  } else {
    delete_case5(t, n);
  }
}
//case 3의 경우 n,p,s가 모두 검정일 경우, s를 빨강으로 칠한다. 그러면 n의 서브트리와 s 서브트리의 검정색 노드 갯수가 동일해진다. 
//case 2,3을 통해서 parent는 동일 깊이 타 노드의 서브트리에 비해 1개 적은 서브트리를 가지게 되었다.
//해당 과정을 반복하다가 n,s,p가 모두 검검검이 아닐 때 case4를 수행한다. 최종 목표는 1)n이 검정, s가 검정이고 자식이 모두 검정일 때, s와 p의 색을 바꾸기(-case4) 2)n이 검정,s가 검정, s의 바깥쪽 자식이 빨강(-case5,6)이 되고, 이것을 회전시키는 것이다.
void delete_case3(rbtree* t, node_t* n) {
  node_t *s = sibling(t, n);
  if ((n -> parent -> color == RBTREE_BLACK) && (s -> color == RBTREE_BLACK) &&
  (s -> left -> color == RBTREE_BLACK) && (s -> right -> color == RBTREE_BLACK)) {
    s -> color = RBTREE_RED;
    delete_case1(t, n -> parent); //타 노드들보다 검정색 노드가 하나 적은 것을 다시 delete_case1에 호출
  } else {
    delete_case4(t, n);
  }
}

//현재 상황은, 노드 n을 루트로 하는 서브트리의 검정색의 갯수가 동일 깊이의 타 노드를 루트로 하는 서브트리의 검정색의 갯수보다 한 개가 적다는 것이고, 이것을 해결하기 위한 과정을 진행.
//case2, 3에서 하려고 하는 작업은 n이 검은색 형제노드와 붉은색 부모 노드를 가지게 하는 것이다. 형제노드가 루트로 하는 서브트리의 검정색 갯수는 한 개 더 많다. 이 때 case 4,5,6을 진행하여 균형을 맞출 수 있다.
//case2의 경우 n의 노드의 형제가 빨간색일 경우이고 이 경우에는 p를 빨간색으로 바꾸고 왼쪽 회전 수행한다. 
void delete_case2(rbtree* t, node_t* n) {
  node_t *s = sibling(t, n);
  if (s -> color == RBTREE_RED) {
    n -> parent -> color = RBTREE_RED;
    s -> color = RBTREE_BLACK;
    if (n == n -> parent -> left) {
      rotate_left(t, n -> parent);
    } else {
      rotate_right(t, n -> parent);
    }
  }
  delete_case3(t, n);
}

//만약 부모가 nil이면 루트를 삭제한 것으로 종료. 아니라면 case2로 넘어감.
void delete_case1(rbtree* t, node_t* n) {
  if (n -> parent != t -> nil) {
    delete_case2(t, n);
  }
}

//SUCCESOR의 CHILD는 NIL이 무조건 한 개가 있을 수 밖에 없어서, nil이 아닌 노드를 child로 선정하고, 둘 다 nil이면 child는 nil.
void delete_one_child(rbtree *t, node_t *n) {
  node_t *child = is_leaf(t, n -> right)? n -> left: n -> right;
  replace_node(t, n, child);
  //만약 삭제되는 색이 검정이고, 대체하는 child의 색이 빨강이면 child를 검정으로 바꾸면 된다. 아니면 deletecase1 호출.
  if (n -> color == RBTREE_BLACK) {
    if (child -> color == RBTREE_RED) {
      child -> color = RBTREE_BLACK;
    } else {
      delete_case1(t, child);
    }
    free(n);
  }
}

//n노드 이하에서 최소 노드 찾는 함수
node_t* min_node(rbtree *t, node_t *n){
  node_t *now = n;
  while (now -> left != t -> nil) {
    now = now -> left;
  }
  return now;
}

//n노드 이하에서 최대 노드 찾는 함수
node_t* max_node(rbtree *t, node_t *n){
  node_t *now = n;
  while (now -> right != t -> nil) {
    now = now -> right;
  }
  return now;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  //0. NULL포인터가 들어오게 된다면 false return
  if (p == NULL) {
    return 0;
  }
  node_t *node_replace;
  // color_t color_erased;
  //1. 이진탐색트리에서 만약 오른쪽 노드가 있다면 오른쪽 트리 최솟값, 없다면 왼쪽 트리 최댓값을 찾는다.
  if (p -> right != t -> nil) {
    node_replace = min_node(t, p -> right);
  } else if (p -> left != t -> nil) {
    node_replace = max_node(t, p -> left);
  } else {
    node_replace = t -> nil;
  }
  //2. node_replace의 값을 현재 노드에 복사해주고 node_replace를 삭제하는 문제로 치환한다.
  if (node_replace != t -> nil) {
    p -> key = node_replace -> key;
    delete_one_child(t, node_replace);
  } else {
    if (p == t -> root) {
      t -> root = t -> nil;
    } else {      
      replace_node(t, p, t -> nil);
      if (p -> color == RBTREE_BLACK) {
        delete_case1(t, t -> nil);
      }      
    }
    free(p);
  }
  //t의 nil SUCCESSOR로 대체해야될 때, 임시로 t -> nil을 p의 자식, p를 t -> nil의 부모로 설정하므로 이후 다시 t-> nil의 부모를 t -> nil로 설정
  t -> nil -> parent = t -> nil;
  return 1;
}

//rbtree의 key를 inorder방식으로 array에 추가하는 함수
void get_key_rbtree(const rbtree *t, node_t* node, key_t *arr, const size_t n, size_t *index) {
  if (node -> left != t -> nil) {
    get_key_rbtree(t, node -> left, arr, n, index);
  }  
  if (*index < n){
    arr[*index] = node -> key;
    (*index)++;
  } else {
    return;
  }
  if (node -> right != t -> nil) {
    get_key_rbtree(t, node -> right, arr, n, index);
  }
}

//rbtree를 sort된 array로 만드는 함수
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  size_t index = 0;
  get_key_rbtree(t, t->root, arr, n, &index);  
  return 0;
}
