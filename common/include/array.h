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

#ifndef ARRAY_H
#define ARRAY_H
struct array_int {
    int *items; // puntatore agli elementi dell'array
    int length;   // lunghezza array    
    int size;     // dimensione allocata (>= length)
    int C_EXP;   // costante di espansione
    int C_RED;   // costante di riduzione
};
typedef struct array_int array_int;

array_int *array_int_init(int length); // Array initialization
void array_int_free(array_int *a); // Array deallocation
void array_int_resize(array_int *a, int length); // Array resizing. Length is the new length of the array.
void array_int_push(array_int *a, int elem); // Adds an element at the end of the array
int array_int_pop(array_int *a); // Pops last element of the array
int array_int_get(array_int *a, int pos); // Gets element at pos
int array_int_length(array_int *a); // Gets number of elements in the array
void array_int_set(array_int *a, int pos, int elem); // Sets element at pos
void array_int_print(array_int *a); // Prints all the elements of the array
void array_int_sprint(array_int *a, char *s); // Stampa l'array su una stringa da passare come argomento
int *array_int_get_ptr(array_int *a);
int array_int_get_min(array_int *a);
void array_int_clear(array_int *a);

typedef struct array_ptr array_ptr;

array_ptr *array_ptr_init(int length);
void array_ptr_free(array_ptr *a);
void array_ptr_resize(array_ptr *a, int length);
void array_ptr_push(array_ptr *a, void *elem); // Adds an element at the end of the array
void *array_ptr_pop(array_ptr *a); // Pops last element of the array
void *array_ptr_get(array_ptr *a, int pos); // Gets element at pos
int array_ptr_length(array_ptr *a); // Gets number of elements in the array
void array_ptr_set(array_ptr *a, int pos, void *elem); // Sets element at pos
void array_ptr_print(array_ptr *a); // Prints all the elements of the array

#endif