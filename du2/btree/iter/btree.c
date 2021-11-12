/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {
  *tree = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  bst_node_t *node = tree;
  while(node){
    if(node->key == key){
      *value = node->value;
      return true;
    }
    else if(node->key > key)
      node = node->left;
    else
      node = node->right;
  }
  return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
  bst_node_t *new = malloc(sizeof(bst_node_t));
  if(!new) return;
  new->key = key;
  new->value = value;
  new->left = NULL;
  new->right = NULL;

  if(!*tree){
    *tree = new;
    return;
  }

  bst_node_t *node = *tree;
  while(true){
    if(node->key == key){
      node->value = value;
      free(new);
      return;
    }
    else if(node->key > key){
      if(node->left)
        node = node->left;
      else{
        node->left = new;
        return;
      }
    }
    else{
      if(node->right)
        node = node->right;
      else{
        node->right = new;
        return;
      }
    }
  }
}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
  bst_node_t *node = (*tree)->right, *last = *tree;
  while(node->right){
    last = node;
    node = node->right;
  }
  target->key = node->key;
  target->value = node->value;
  last->right = node->left;
  free(node);
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
  bst_node_t *node = *tree, *last;
  while(node){
    if(node->key == key){
      if(!node->left && !node->right){
        if(last->left == node) 
          last->left = NULL;
        else 
          last->right = NULL;
        free(node);
      }
      else if(node->left && node->right){
        bst_replace_by_rightmost(node, &(node->left));
      }
      else{
        if(last->left == node) 
          last->left = node->right ? node->right : node->left;
        else 
          last->right = node->right ? node->right : node->left;
        free(node);
      }
      return;
    }
    last = node;
    if(node->key > key)
      node = node->left;
    else
      node = node->right;
  }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
  stack_bst_t s;
  stack_bst_init(&s);
  bst_node_t *node = *tree;
  while(node){
    stack_bst_push(&s, node);
    node = node->left;
  }
  while(!stack_bst_empty(&s)){
    node = stack_bst_pop(&s);
    if(node->right){
      bst_node_t *tmpNode = node->right;
      while(tmpNode){
        stack_bst_push(&s, tmpNode);
        tmpNode = tmpNode->left;
      }
    }
    free(node);
  }
  *tree = NULL;
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
  bst_node_t *node = tree;
  while(node){
    bst_print_node(node);
    stack_bst_push(to_visit, node);
    node = node->left;
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  stack_bst_t s;
  stack_bst_init(&s);
  bst_leftmost_preorder(tree, &s);
  bst_node_t *node;
  while(!stack_bst_empty(&s)){
    node = stack_bst_pop(&s);
    bst_leftmost_preorder(node->right, &s);
  }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
  bst_node_t *node = tree;
  while(node){
    stack_bst_push(to_visit, node);
    node = node->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  stack_bst_t s;
  stack_bst_init(&s);
  bst_leftmost_inorder(tree, &s);
  bst_node_t *node;
  while(!stack_bst_empty(&s)){
    node = stack_bst_pop(&s);
    bst_print_node(node);
    bst_leftmost_inorder(node->right, &s);
  }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {
  bst_node_t *node = tree;
  while(node){
    stack_bst_push(to_visit, node);
    stack_bool_push(first_visit, true);
    node = node->left;
  }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  bst_node_t *node;
  stack_bst_t nStack;
  stack_bool_t bStack;
  stack_bst_init(&nStack);
  stack_bool_init(&bStack);
  bst_leftmost_postorder(tree, &nStack, &bStack);
  while(!stack_bst_empty(&nStack)){
    node = stack_bst_top(&nStack);
    if(stack_bool_pop(&bStack)){
      stack_bool_push(&bStack, false);
      bst_leftmost_postorder(node->right, &nStack, &bStack);
    }
    else{
      stack_bst_pop(&nStack);
      bst_print_node(node);
    }
  }
}
