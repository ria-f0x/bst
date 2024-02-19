#include "Tree.h"
#include <stdlib.h>
#include <stdbool.h>

/*
 * Приватные функции модуля, видны только в данном файле
 */
static void inorder_walk( Node *node, void (*fp)( void *item ) )
{
    if ( node != NULL ) {
        inorder_walk( node->left, fp );
        fp( node->item );
        inorder_walk( node->right, fp );
    }
}

static void preorder_walk( Node *node, void (*fp)( void *item ) )
{
    if ( node != NULL ) {
        fp( node->item );
        inorder_walk( node->left, fp );
        inorder_walk( node->right, fp );
    }
}

static void postorder_walk( Node *node, void (*fp)( void *item ) )
{
    if ( node != NULL ) {
        inorder_walk( node->left, fp );
        inorder_walk( node->right, fp );
        fp( node->item );
    }
}


static Node *tree_minimum( Node *node )
{
    while ( node->left != NULL ) {
        node = node->left;
    }
    return node;
}

static Node *node_seek_by_item( const Tree *tree, const void *item )
{
    Node *current_node = tree->root;

    while ( NULL != current_node ) {
        int comparison_result = tree->cmp_func( item, current_node->item );
        if ( 0 == comparison_result ) {
            return current_node;
        }
        else if ( comparison_result > 0 ) {
            current_node = current_node->right;
        }
        else if ( comparison_result < 0 ) {
            current_node = current_node->left;
        }
    }
    return NULL;

}

static void node_replace( Tree *tree, Node *old_node, Node *new_node )
{
    if ( old_node->parent == NULL ) {
        tree->root = new_node;
    }
    else if ( old_node->parent->left == old_node ) {
        old_node->parent->left = new_node;
    }
    else if ( old_node->parent->right == old_node ) {
        old_node->parent->right = new_node;
    }

    if ( new_node != NULL ) {
        new_node->parent = old_node->parent;
    }
}

static void node_free( const Tree *tree, Node *node )
{
   if ( tree->free_func != NULL ) tree->free_func( node->item );
   free( node );
   return;
}

/*
 * Публичный интерфейс модуля
 */
Tree *tree_new( int (*cmp_func)( const void *item_a, const void *item_b ),
                void (*free_func)( void *item ) )
{
    Tree *new_tree = malloc( sizeof( Tree ) );

    if ( new_tree != NULL ) {
        new_tree->root = NULL;
        new_tree->size = 0;
        new_tree->cmp_func = cmp_func;
        new_tree->free_func = free_func;
    }

    return new_tree;
}

void tree_walk( Tree *tree, void (*fp)( void *item ) )
{
    inorder_walk( tree->root, fp );
    return;
}

bool tree_insert( Tree *tree, void *item )
{
    if ( tree_search( tree, item ) != NULL ) return false;

    Node *new_node = malloc( sizeof( Node ) );
    if ( NULL == new_node ) return false;

    new_node->item = item;
    new_node->left = new_node->right = new_node->parent = NULL;

    Node *parent_of_new_node = NULL;
    Node *current_node = tree->root;

    while ( NULL != current_node ) {
        parent_of_new_node = current_node;
        if ( tree->cmp_func( item, current_node->item ) > 0 ) {
            current_node = current_node->right;
        }
        else {
            current_node = current_node->left;
        }
    }

    new_node->parent = parent_of_new_node;
    if ( NULL == parent_of_new_node ) {
        tree->root = new_node;
    }
    else if ( tree->cmp_func( new_node->item, parent_of_new_node->item ) > 0 ) {
        parent_of_new_node->right = new_node;
    }
    else {
        parent_of_new_node->left = new_node;
    }

    ++tree->size;
    return true;
}

void *tree_search( const Tree *tree, const void *item )
{
    Node *node_with_item = node_seek_by_item( tree, item );

    if ( node_with_item != NULL ) {
        return node_with_item->item;
    }
    else {
        return NULL;
    }
}

void tree_delete( Tree *tree, void *item )
{
    if ( tree->size == 0 ) return;

    Node *deleted_node = node_seek_by_item( tree, item );
    if ( NULL == deleted_node ) return;

    /*
     * У удаляемого узла нет левого дочернего поддерева,
     * Удаляем узел, а на его место ставится его правое поддерево
     *
     *
     *              ( )                         ( )
     * 				 |  						 |
     *              (X)    <----+    ===>       (R)
     *              / \         |         		/ \
     *             /   \        |
     *           NULL  (R) -----+
     *				   / \
     *
     *
     * Этот метод будет верен и в том случае, если правый дочерний узел R равен NULL
     *
     */

    if ( deleted_node->left == NULL ) {
        node_replace( tree, deleted_node, deleted_node->right );
    }

    /* Симметричный вариант, у удаляемого узла нет правого дочернего поддерева
     * Удаляем узел и на его место ставим левое дочернее поддерево
     *
     *
     *              ( )                         ( )
     * 				 |  						 |
     *      +-----> (X)              ===>       (L)
     *      |       / \                   		/ \
     *      |      /   \
     *      +--- (L)   NULL
     *		     / \
     *
     *
     */

    else if ( deleted_node->right == NULL ) {
        node_replace( tree, deleted_node, deleted_node->left );
    }

    /* Самая сложная ситуация - у удаляемого узла есть и правое, и левое дочерние поддеревья
     *
     *        ( )
     *   	   |
     *        (X)
     *        / \
     *       /   \
     *     (L)   (R)
     *     / \   / \
     *
     *
     * В этой ситуации возможны два варианта:
     *
     * а) Правое поддерево удаляемого поддерева имеет вид:
     *
     *        ( )                         ( )
     *   	   |					       |
     *        (X)                         (R)
     *        / \                         / \
     *       /   \                       /   \
     *    (L)     (R)       ====>     (L)     ( )
     *    / \     / \                 / \     / \
     *         NULL  ( )
     *               / \
     *
     *
     * то есть у правого дочернего узла (R) удаляемого элемента отсутсвует левое поддерево.
     *
     * В этом случае для удаления элемента X следует выполнить шаги:
     * 1) На место X ставим R
     * 2) На место левого поддерева R ( NULL ) ставим L
     *
     *
     * б) Если у R есть и левое и правое поддеревья:
     *
     *        ( )                                                          ( )
     *   	   |					    					                |                         	   |
     *        (X)                      ( )                                 (Z)
     *        / \                       |           *                      / \
     *       /   \                     (X)         (Z)                    /   \
     *    (L)     (R)       ====>      /           / \        ====>    (L)     (R)
     *    / \     / \                 /           /   \                / \     / \
     *           /   \             (L)          NULL  (R)                     /   \
     * 		   (Z)   ( )           / \                / \              	    (Y)   ( )
     *         / \   / \                            (Y)                     / \   / \
     *      NULL (Y)
     *
     *
     *  Нам нужно преобразовать правое поддерево R к виду как в случае 1,
     *  чтобы можно было свести случай 2 к случаю 1.
     *  То есть мы должны получить поддерево, у которого левый дочерний узел
     *  был бы пустым.
     *
     *          (R)                        (*)
     *          / \                        / \
     *         /   \                      /   \
     *       (Z)   ( )     ====>        NULL  (*)
     *       / \   / \                        / \
     *    NULL (Y)
     *
     *
     *  Для этого мы должны найти МИНИМАЛЬНЫЙ узел в поддереве R.
     *  Обозначим минимальный узел в поддереве R через Z.
     *  Где должен располагаться минимальный узел?
     *  Во-первых, он НЕ должен располагаться в любом правом поддереве.
     *  Во-вторых, у него НЕ может быть левого дочернего узла.
     *
     *  Таким образом, для нахождения Z следует спускаться от R вниз по левой ветке
     *  до тех пор, пока не наткнемся на узел без левого дочернего элемента.
     *  Этот узел и будет узлом с минимальным ключом Z.
     *
     *  Итак, мы нашли элемент с минимальным ключом.
     *  Далее нужно преобразовать поддерево R так, чтобы корнем его стал Z.
     *
     *  Для этого:
     *  1) На место Z ставим его правое поддерево Y
     *  2) На место Y ставим правое поддерево R удаляемого узла X
     *  3) И указываем узлу R в качестве родителя узел Z
     *
     *  После выполнения получим искомое поддерево:
     *
     *           (R)                   (Z)
     *           / \                   / \
     *          /   \               NULL (R)
     *     	  (Z)   ( )     ====>        / \
     *        / \   / \                (Y) ( )
     *     NULL (Y)
     *
     *
     */
    else {
        // находим минимальный узел в поддереве R
        Node *minimal_node = tree_minimum( deleted_node->right );

        // случай 2
        if ( minimal_node->parent != deleted_node ) {
            node_replace( tree, minimal_node, minimal_node->right );
            minimal_node->right = deleted_node->right;
            minimal_node->right->parent = minimal_node;
        }

        // общий для случаев 1,2 алгоритм
        node_replace( tree, deleted_node, minimal_node );
        minimal_node->left = deleted_node->left;
        minimal_node->left->parent = minimal_node;

    }
    //освободить память, занимаемую узлом
    node_free( tree, deleted_node );
    --tree->size;
}
