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

#include <stdlib.h>
#include <stdio.h>
#include "khash.h"

int main(int argc, char* argv[]){
    (void) argc;
    (void) argv;
    khash_t(m32) *ht;
    khint_t k;
    int ret;
    ht = kh_init(m32);

    for (int i = 1; i <= 100; ++i) {
		k = kh_put(m32, ht, i, &ret);
		kh_val(ht, k) = k;
	}
    for (int i = 1; i <= 200; ++i) {
		k = kh_put(m32, ht, i, &ret);
        if(ret!=0){ //If key is not present in the ht put smth
            kh_value(ht, k) = 42;
        }
        //printf("k: %d ret:%d value:%d exists:%d\n", k , ret, kh_value(ht, k), kh_exist(ht, k));
	}
    kh_put(m32, ht, 7777, &ret);
    for (int i = 1; i <= 400; ++i) {
        //printf("k: %d exists:%d\n", i,kh_exist(ht,i));
	}

    k = kh_get(m32, ht, 0);
    //printf("k: %d, exist: %d, val: %d, kh_end: %d\n",k,kh_exist(ht, k),kh_value(ht,k), kh_end(ht));
    
    kh_destroy(m32, ht);
}