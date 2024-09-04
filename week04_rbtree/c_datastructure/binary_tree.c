#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int key;
    struct node *parent;
    struct node *left;
    struct node *right;
} node;

typedef struct {
    node *root;

} BST;


BST* new_bst(void){
    BST *p = (BST *)calloc(1, sizeof(BST));
    return p;
}

void delete_tree(node* n){
    if (n -> left != NULL) {
        delete_tree(n -> left);
    }
    if (n -> right != NULL) {
        delete_tree(n -> right);
    }
    free(n);
}

void delete_BST(BST *t){
    delete_tree(t -> root);
}

node* createnode(int key){
    node* new_node = (node *)calloc(1, sizeof(node));
    new_node -> key = key;
    new_node -> parent = NULL;
    new_node -> left = NULL;
    new_node -> right = NULL;
    return new_node;
}

node* tree_insert(BST *t, int key) {
    // 루트가 NULL인 경우 새 노드를 루트로 설정
    node* new_node = createnode(key);
    node *now = t -> root;
    if (now == NULL){
        t -> root =  new_node;
        return t -> root;
    }
    while (now != NULL) {
        if (new_node -> key > now -> key) {
            if (now -> right == NULL) {
                now -> right = new_node;
                new_node -> parent = now;
                return t -> root;
            }
            now = now -> right;
        }
        else{
            if (now -> left == NULL) {
                now -> left = new_node;
                new_node -> parent = now;
                return t -> root;
            }
            now = now -> left;
        }
    }    
}
// node* insert(node* root, int key){
//     node *new = createnode(key);
//     if (root == NULL) {
//         return new;
//     }
//     else{
//         while (1) {
//             if (new -> key > root -> key){
//                 if (root -> right == NULL){
//                     root -> right = new;
//                     new -> parent = root;
//                     return root;
//                 }                
//                 root = root -> right;                
//             }
//             else{
//                 if (root -> left == NULL){
//                     root -> left = new;
//                     new -> parent = root;
//                     return root;
//                 }
//                 root = root -> left;                
//             }
//         }        
//     }
// }

node* search(node* root, int key){
    if (key == root -> key) {
        return root;
    }
    if (key > root -> key) {
        if (root -> right == NULL){
            return NULL;
        }
        return search(root -> right, key);
    }
    else {
        if (root -> left == NULL){
            return NULL;
        }
        return search(root -> left, key);
    }
}

node* findmin(node* root){

}
node* findmax(node* root){

}
node* delete(node* root, int key){
    if (key == root -> key) {

        return root;
    }
    if (key > root -> key) {
        if (root -> right == NULL){
            return NULL;
        }
        return delete(root -> right, key);
    }
    else {
        if (root -> left == NULL){
            return NULL;
        }
        return delete(root -> left, key);
    }
}
void inorder(node* root){
    if (root -> left != NULL) {
        inorder(root -> left);
    }
        printf("%d -> ", root -> key);
    if (root -> right != NULL) {
        inorder(root -> right);
    }

}

void preorder(node* root){
    
    printf("%d -> ", root -> key);
    if (root -> left != NULL){
        preorder(root -> left);
    }
    if (root -> right != NULL){
        preorder(root -> right);
    }
}

void postorder(node* root){
    if (root -> left != NULL) {
        postorder(root -> left);
    }
    if (root -> right != NULL){
        postorder(root -> right);
    }
        printf("%d -> ", root -> key);
}

int height(node* root){

}
int countnodes(node* root){

}

int isbalanced(node* root){

}
int main() {    
    BST *bst = new_bst();
    node *a = tree_insert(bst, 10);
    printf("%d %d\n", bst -> root -> key, a -> key);
    node *b = tree_insert(bst, 1);
    node *c = tree_insert(bst, 2);
    node *d = tree_insert(bst, 5);
    node *e = tree_insert(bst, 15);
    node *f = tree_insert(bst, 20);
    node *s = search(bst -> root, 3);
    // printf("%d\n", s -> key);
    printf("%d %d\n", bst -> root -> key, a -> key);
    delete_BST(bst);
    printf("%d", bst -> root -> key);
    // postorder(bst -> root);
    // preorder(bst -> root);
    // inorder(bst -> root);
}