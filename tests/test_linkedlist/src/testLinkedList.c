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

#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

int main(){
    linkedlist_int* ll= linkedlist_int_init();
    /*
    linkedlist_int_push(ll,1);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_push(ll,2);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_push(ll,3);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_push(ll,4);
    linkedlist_int_push(ll,5);
    linkedlist_int_push(ll,6);
    linkedlist_int_push(ll,7);
    linkedlist_int_push(ll,8);
    linkedlist_int_push(ll,9);
    printf("%d\n",linkedlist_int_length(ll));
    linkedlist_int_print(ll);

    printf("%d ",linkedlist_int_pop(ll));
    printf("%d ",linkedlist_int_pop(ll));
    printf("%d ",linkedlist_int_pop(ll));
    printf("%d ",linkedlist_int_pop(ll));
    printf("%d ",linkedlist_int_pop(ll));
    printf("%d ",linkedlist_int_pop(ll));
    printf("%d ",linkedlist_int_pop(ll));
    printf("%d ",linkedlist_int_pop(ll));
    printf("%d ",linkedlist_int_pop(ll));
    printf("\nn: %d\n",linkedlist_int_length(ll));

    //printf("\nMi aspetto un'eccezione\n");
    //printf("%d",linkedlist_int_pop(ll));

    */
   /*
    linkedlist_int_insert(ll,1);
    linkedlist_int_insert(ll,5);
    linkedlist_int_insert(ll,10);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_insert(ll,3);
    linkedlist_int_insert(ll,8);
    linkedlist_int_insert(ll,2);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_insert(ll,0);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_print(ll);

    linkedlist_int_delete(ll,1);
    linkedlist_int_print(ll);
    linkedlist_int_delete(ll,0);
    linkedlist_int_print(ll);
    linkedlist_int_delete(ll,10);
    linkedlist_int_print(ll);

    linkedlist_int_free(ll);
    */
    linkedlist_int_enqueue(ll,1);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_enqueue(ll,2);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_enqueue(ll,3);
    printf("n: %d\n",linkedlist_int_length(ll));
    linkedlist_int_enqueue(ll,4);
    linkedlist_int_enqueue(ll,5);
    linkedlist_int_enqueue(ll,6);
    linkedlist_int_enqueue(ll,7);
    linkedlist_int_enqueue(ll,8);
    linkedlist_int_enqueue(ll,9);
    printf("%d\n",linkedlist_int_length(ll));
    linkedlist_int_print(ll);

    printf("%d ",linkedlist_int_dequeue(ll));
    printf("%d ",linkedlist_int_dequeue(ll));
    printf("%d ",linkedlist_int_dequeue(ll));
    printf("%d ",linkedlist_int_dequeue(ll));
    printf("%d ",linkedlist_int_dequeue(ll));
    printf("%d ",linkedlist_int_dequeue(ll));
    printf("%d ",linkedlist_int_dequeue(ll));
    printf("%d ",linkedlist_int_dequeue(ll));
    printf("%d ",linkedlist_int_dequeue(ll));
    printf("\nn: %d\n",linkedlist_int_length(ll));
    linkedlist_int_print(ll);

    linkedlist_int_free(ll);
    return 0;
}