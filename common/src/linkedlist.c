/* 
 * Course: High Performance Computing 2022/2023 
 * 
 * Lecturer: Francesco Moscato  fmoscato@unisa.it 
 *
 * Group:
 * Langella	Antonio   0622702011   a.langella31@studenti.unisa.it
 * Marsico Michele   0622702012   m.marsico10@studenti.unisa.it
 * Paolino Salvatore   0622702016   s.paolino6@studenti.unisa.it 
 * Risi Davide   0622702013   d.risi2@studenti.unisa.it
 * 
 * Copyright (C) 2023 - All Rights Reserved 
 *
 * This file is part of Parallelize-Tarjan-MPI-CUDA. 
 *
 * Parallelize-Tarjan-MPI-CUDA is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version. 
 *
 * Parallelize-Tarjan-MPI-CUDA is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License 
 * along with Parallelize-Tarjan-MPI-CUDA.  If not, see <http://www.gnu.org/licenses/>. 
 * 
 */

/**
 * @file linkedlist.c
 * @brief This file implements the abstract data type linkedlist.
 */

#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include <assert.h>

/**
 * @brief This struct represents the node of the list data structure. 
 * 
 */
struct lnode_int_t{
    int info;
    lnode_int_t* nextLink;
    lnode_int_t* prevLink;
};

/**
 * @brief This struct represents the linkedlist data structure.
 * 
 */
struct linkedlist_int{
    lnode_int_t* FirstNode; // puntatore al primo nodo
    lnode_int_t* LastNode; // puntatore all'ultimo nodo
    int length;   // lunghezza array    
};

/**
 * @brief This function takes an integer as input and creates a new lnode_int_t node.
 * 
 * @param info an integer representing the information stored by the node.
 * @return lnode_int_t*.
 */
lnode_int_t *nodeCreate(int info) {
    lnode_int_t *new_elem = (lnode_int_t *) malloc (sizeof(lnode_int_t));
    if(new_elem == NULL){
        printf("Error. Node not allocate\n");
        exit(1);
    }
    if (new_elem != NULL) {
        new_elem->info = info;
        new_elem->nextLink = NULL;
        new_elem->prevLink = NULL;
    }
    return new_elem;
}
/**
 * @brief This function takes a lnode_int_t node as input and deallocates it.
 * 
 * @param n pointer to lnode_int_t node.
 */
void nodeDestroy(lnode_int_t* n) {
    free(n);
}

/**
 * @brief This function generates and initializes a linkedlist_int data structure.
 * 
 * @return linkedlist_int* 
 */
linkedlist_int *linkedlist_int_init(){
    //linkedlist initialization
    linkedlist_int* l = (linkedlist_int *) malloc(sizeof(linkedlist_int));
    l->length = 0;
    l->FirstNode = NULL;
    l->LastNode = NULL;
    return l;
}
/**
 * @brief This function takes as input a linkedlist_int data structure and deallocates the occupied memory.
 * 
 * @param a is linkedlist_int data structure.
 */
void linkedlist_int_free(linkedlist_int *a){
    //linkedlist deallocation
    lnode_int_t *last = a->LastNode;
    lnode_int_t *node = a->LastNode;
    while(node != NULL){
        node = last->prevLink;
        nodeDestroy(last);
        last = node;
    }
}
/**
 * @brief This function takes as input a linkedlist_int data structure and returns the number of nodes contained in it. 
 * 
 * @param a is linkedlist_int data structure.
 * @return int 
 */
int linkedlist_int_length(linkedlist_int *a){
    //Gets number of elements in the linkedlist
    return a->length;
} 

/**
 * @brief This function takes as input a linkedlist_int data structure and an integer. It inserts the element passed as input at the last position in the data structure. 
 * \n The linkedlist_int can work like a Stack data structure if the linkedlist_int_pop() and linkedlist_int_push() functions are used.
 * 
 * @param a is linkedlist_int data structure.
 * @param elem is an integer which represents the elem to be inserted.
 */
void linkedlist_int_push(linkedlist_int *a, int elem){
    //Adds an element at the end of the linkedlist
    if(a == NULL){
        printf("Error. Invalid LinkedList");
        exit(1);
    }
    lnode_int_t * new_elem = nodeCreate(elem);
    //lnode_int_t* n = a->LastNode;
    if(a->length == 0){
        //Nel caso in cui la linkedlist è vuota
        a->LastNode = new_elem;
        a->FirstNode = new_elem;
    }else{
        //Nel caso in cui la linkedlist NON è vuota
        lnode_int_t* last = a->LastNode;
        new_elem->prevLink = last;
        last->nextLink = new_elem;
        a->LastNode = new_elem;
    }
    a->length +=1;
    return;
}
/**
 * @brief This function takes as input a linkedlist_int data structure. It removes the element at the last position in the data structure.
 * \n The linkedlist_int can work like a Stack data structure if the linkedlist_int_pop() and linkedlist_int_push() functions are used.
 * 
 * @param a is linkedlist_int data structure.
 * @return int 
 */
int linkedlist_int_pop(linkedlist_int *a){
    //Pops last element of the linkedlist
    if(a == NULL){
        printf("Error. Invalid LinkedList\n");
        exit(1);
    }
    if(a->length == 0){
       printf("Error list is empty\n"); 
       exit(1);
    }else if(a->length == 1){
        //La lista ha un solo elemento.
        lnode_int_t* last_node = a->LastNode;
        int inf = last_node->info;
        a->FirstNode = NULL;
        a->LastNode = NULL;
        a->length -=1;
        nodeDestroy(last_node);
        return inf;
    }else{
        //La lista ha più di un elemento.
        lnode_int_t* last_node = a->LastNode;
        int inf = last_node->info;
        lnode_int_t* penultimo_node = last_node->prevLink;
        penultimo_node->nextLink = NULL;
        a->LastNode = penultimo_node;
        a->length -=1;
        nodeDestroy(last_node);
        return inf;
    }
} 
/**
 * @brief This function takes as input a linkedlist_int data structure and an integer to insert. It inserts the element passed as input into the data structure. 
 * \n The linkedlist_int works as an ordered list data structure if the insert and remove functions are used.
 * 
 * @param a is linkedlist_int data structure.
 * @param elem is an integer which represents the elem to be inserted.
 */
void linkedlist_int_insert(linkedlist_int *a, int elem){
    //Insert the element in order
    if(a == NULL){
        printf("Error. Invalid LinkedList\n");
        exit(1);
    }

    //Se la lista è vuota  
    if(a->length == 0){
        lnode_int_t *new_elem = nodeCreate(elem); //inizializza a NULL i puntatori del nodo
        a->FirstNode = new_elem;
        a->LastNode = new_elem;
        a->length +=1;
        return;
    }

    //Se la lista NON è vuota
    int isFirstElement = 1;
    lnode_int_t* node =  a->FirstNode;
    while(elem > node->info){
        isFirstElement = 0;
        if(node->nextLink == NULL){
            //Sono arrivato alla fine della lista
            lnode_int_t *new_elem = nodeCreate(elem);
            new_elem->nextLink = NULL;
            new_elem->prevLink = node;
            node->nextLink = new_elem;
            a->LastNode = new_elem;
            a->length +=1;
            return;
        }
        node = node->nextLink;
    }
    
    lnode_int_t *new_elem = nodeCreate(elem);
    if(isFirstElement == 1){
        //L'inserimento deve avvenire in testa
        a->FirstNode = new_elem;
        new_elem->nextLink = node;
        node->prevLink = new_elem;
        a->length +=1;
        return;
    }
    //La lista non è vuota e l'inserimento deve avvenire nel mezzo (Non in testa e non in coda) 
    lnode_int_t * prev = node->prevLink;
    prev->nextLink = new_elem;
    node->prevLink = new_elem;
    new_elem->nextLink = node;
    new_elem->prevLink = prev;
    a->length +=1;
    return;

}

/**
 * @brief This function takes as input a linkedlist_int data structure and an integer. It removes the element passed as input from the data structure.
 * \n The linkedlist_int works like a ordered list data structure if the insert and remove functions are used.
 * 
 * @param a is linkedlist_int data structure.
 * @param elem is an integer which represents the elem to be deleted.
 */

void linkedlist_int_delete(linkedlist_int *a, int elem){
    //Delete the element if it exists
    if(a == NULL){
        printf("Error. Invalid LinkedList\n");
        exit(1);
    }
    //Se la lista è vuota  
    if(a->length == 0){
        return;
    }

    //La lista ha un solo elemento
    if(a->length == 1){

        //Se l'elemento è presente nella lista allora viene cancellato 
        if(a->FirstNode->info == elem){
            nodeDestroy(a->FirstNode);
            a->length -=1;
            a->FirstNode = NULL;
            a->LastNode = NULL;
        }
        //Se l'elemento non è presente non viene fatto niente 
        return;
    }

    //La lista ha più di un elemento
    int isFirstElement = 1;
    lnode_int_t* node =  a->FirstNode;
    while(elem != node->info){
        isFirstElement = 0;
        if(node->nextLink == NULL){
            //FINITO DI SCORRERE LA LISTA E IL NODO NON È STATO TROVATO
            return;
        }
        node = node->nextLink;
    }
    if(isFirstElement == 1){
        //DEVO CANCELLARE IL PRIMO ELEMENTO
        a->FirstNode = node->nextLink;
        a->FirstNode->prevLink = NULL;
        nodeDestroy(node);
        a->length -=1;
        return;
    }
    if(node == a->LastNode){
        //DEVO CANCELLARE L'ULTIMO ELEMENTO
        lnode_int_t *prev = node->prevLink;
        prev->nextLink = NULL;
        a->LastNode = prev;
        nodeDestroy(node);
        a->length -=1;
        return;
    }
    //Devo cancellare un nodo nel mezzo (Non in testa e non in coda)
    lnode_int_t *prev= node->prevLink; 
    lnode_int_t *next= node->nextLink;
    prev->nextLink = next;
    next->prevLink = prev;
    nodeDestroy(node);
    a->length -=1;
    return;

} 
/**
 * @brief This function takes as input a linkedlist_int data structure and an integer to insert. It inserts the element passed as input into the data structure. 
 * \n The linkedlist_int works like a queue data structure if the linkedlist_int_enqueue() and linkedlist_int_dequeue() functions are used. 
 * 
 * @param a is linkedlist_int data structure.
 * @param elem is an integer which represents the elem to be inserted.
  */
void linkedlist_int_enqueue(linkedlist_int *a, int elem){
    //enqueue an element at the end of the linkedlist
    if(a == NULL){
        printf("Error. Invalid LinkedList");
        exit(1);
    }
    lnode_int_t * new_elem = nodeCreate(elem);
    if(a->length == 0){
        //Nel caso in cui la linkedlist è vuota
        a->LastNode = new_elem;
        a->FirstNode = new_elem;
    }else{
        //Nel caso in cui la linkedlist NON è vuota
        lnode_int_t* first = a->FirstNode;
        new_elem->nextLink = first;
        first->prevLink = new_elem;
        a->FirstNode = new_elem;
    }
    a->length +=1;
    return;

}
/** 
 * @brief This function takes as input a linkedlist_int data structure. It removes the element at the top of the data structure. 
 * \n The linkedlist_int works like a queue data structure if the linkedlist_int_enqueue() and linkedlist_int_dequeue() functions are used.
 * 
 * @param a is linkedlist_int data structure.
 * @return int 
 */
int linkedlist_int_dequeue(linkedlist_int *a){
    //dequeue the last item of the linkedlist
    
    if(a == NULL){
        printf("Error. Invalid LinkedList\n");
        exit(1);
    }
    if(a->length == 0){
       printf("Error! List is empty\n"); 
       exit(1);
    }else if(a->length == 1){
        //La lista ha un solo elemento.
        lnode_int_t* last_node = a->LastNode;
        int inf = last_node->info;
        a->FirstNode = NULL;
        a->LastNode = NULL;
        a->length -=1;
        nodeDestroy(last_node);
        return inf;
    }else{
        //La lista ha più di un elemento.
        lnode_int_t* last_node = a->LastNode;
        int inf = last_node->info;
        lnode_int_t* penultimo_node = last_node->prevLink;
        penultimo_node->nextLink = NULL;
        a->LastNode = penultimo_node;
        a->length -=1;
        nodeDestroy(last_node);
        return inf;
    }
} 

/**
 * @brief This function prints all nodes in the linkedlist_int.  
 * 
 * @param a is linkedlist_int data structure.
 */
void linkedlist_int_print(linkedlist_int *a){
    //Prints all the elements of the linkedlist
    lnode_int_t* l = a->FirstNode;
    printf ("PRINT DEBUG: \n");
    printf ("Numero nodi: %d\n",a->length);
    while (l != NULL) {
        printf("%d ",l->info);
        l = l->nextLink;
    }
    printf ("\nFINE DEBUG\n");
    
} 
/**
 * @brief This function returns the element at the top of the linkedlist_int data structure.  
 * 
 * @param a is linkedlist_int data structure.
 * @return int 
 */
int linkedlist_int_top(linkedlist_int *a){
    assert(a->LastNode != NULL);
    return a->LastNode->info;
}

typedef struct linkedlist_ptr linkedlist_ptr;
