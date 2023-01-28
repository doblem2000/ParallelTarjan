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

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct linkedlist_int linkedlist_int;
typedef struct lnode_int_t lnode_int_t;

linkedlist_int *linkedlist_int_init(); //linkedlist initialization
void linkedlist_int_free(linkedlist_int *a); //linkedlist deallocation
int linkedlist_int_length(linkedlist_int *a); //Gets number of elements in the linkedlist
void linkedlist_int_push(linkedlist_int *a, int elem); //Adds an element at the end of the linkedlist
int linkedlist_int_pop(linkedlist_int *a); //Pops last element of the linkedlist
void linkedlist_int_insert(linkedlist_int *a, int elem); //Innsert the element in order
void linkedlist_int_delete(linkedlist_int *a, int elem); //Delete the element if it exists
void linkedlist_int_enqueue(linkedlist_int *a, int elem); //enqueue an element at the end of the linkedlist
int linkedlist_int_dequeue(linkedlist_int *a); //dequeue of the linkedlist
void linkedlist_int_print(linkedlist_int *a); //Prints all the elements of the linkedlist
int linkedlist_int_top(linkedlist_int *a);
/*
typedef struct linkedlist_ptr linkedlist_ptr;

linkedlist_ptr *linkedlist_ptr_init(); //linkedlist initialization
void linkedlist_ptr_free(linkedlist_ptr *a); //linkedlist deallocation
int linkedlist_ptr_length(linkedlist_ptr *a); //Gets number of elements in the linkedlist
void linkedlist_ptr_push(linkedlist_ptr *a, void *elem); //Adds an element at the end of the linkedlist
void* linkedlist_ptr_pop(linkedlist_ptr *a); //Pops last element of the linkedlist
int linkedlist_ptr_insert(linkedlist_ptr *a, void *elem); //Innsert the element in order
void linkedlist_ptr_delete(linkedlist_ptr *a, void *elem); //Delete the element if it exists
void linkedlist_ptr_enqueue(linkedlist_ptr *a, void *elem); //enqueue an element at the end of the linkedlist
void* linkedlist_ptr_dequeue(linkedlist_ptr *a); //dequeue of the linkedlist
void linkedlist_ptr_print(linkedlist_ptr *a); //Prints all the elements of the linkedlist
void linkedlist_ptr_cpy(linkedlist_ptr *to,linkedlist_ptr *from); //Copy all the elements from a linkedlist to another
*/

#endif