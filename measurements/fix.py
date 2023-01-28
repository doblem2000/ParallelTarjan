#  
# Course: High Performance Computing 2022/2023 

# Lecturer: Francesco Moscato  fmoscato@unisa.it 

# Group:
#  Langella	Antonio   0622702011   a.langella31@studenti.unisa.it
#  Marsico Michele   0622702012   m.marsico10@studenti.unisa.it
#  Paolino Salvatore   0622702016   s.paolino6@studenti.unisa.it 
#  Risi Davide   0622702013   d.risi2@studenti.unisa.it
 
#  Copyright (C) 2023 - All Rights Reserved 

#  This file is part of Parallelize-Tarjan-MPI-CUDA. 

#  Parallelize-Tarjan-MPI-CUDA is free software: you can redistribute it and/or modify 
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License, or 
#  (at your option) any later version. 

#  Parallelize-Tarjan-MPI-CUDA is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
#  GNU General Public License for more details. 

#  You should have received a copy of the GNU General Public License 
#  along with Parallelize-Tarjan-MPI-CUDA.  If not, see <http://www.gnu.org/licenses/>. 
#  

import os
import functools

path = os.path.join(os.path.dirname(os.path.realpath(__file__)),"measure/mpi")
os.chdir(path)
folders = [f for f in os.listdir('.')]
for folder in folders:
  os.chdir(os.path.join(path, folder))
  files = [f for f in os.listdir('.')]
  print("FILES:", files)
  for filename in files:
    with open(filename, 'r') as file:
      head, tail = file.read().split('\n', 1)
    #print(head)
    split = head.split(",")
    #print(split)
    split[-2], split[-3] = split[-3], split[-2]
    #print(split)
    head = functools.reduce(lambda x,y: x+","+y,split) + "\n"
    #print(head)
    with open(filename, 'w') as file:
      file.write(head)
      file.write(tail)
  