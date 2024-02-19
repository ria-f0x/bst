#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct node {
    void *item;
    struct node *left;
    struct node *right;
    struct node *parent;
} Node;

typedef struct {
   Node *root;
   int size;
   int (*cmp_func)( const void *item_a, const void *item_b);
   void (*free_func)( void *item );
} Tree;

Tree *tree_new( int (*cmp_func)( const void *item_a, const void *item_b ),
                void (*free_func)( void *item ) );

int tree_get_size( const Tree *tree );

bool tree_insert( Tree *tree, void *item );

void tree_delete( Tree *tree, void *item );

void tree_walk( Tree *tree, void (*fp)( void *item ) );

void *tree_search( const Tree *tree, const void *item );

// TODO
//void tree_free( Tree *tree );
