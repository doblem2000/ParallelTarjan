# Course: High Performance Computing 2022/2023 
# Lecturer: Francesco Moscato  fmoscato@unisa.it 
# Author: Antonio Langella
# 
# Group:
#  Langella	Antonio   0622702011   a.langella31@studenti.unisa.it
#  Marsico Michele   0622702012   m.marsico10@studenti.unisa.it
#  Paolino Salvatore   0622702016   s.paolino6@studenti.unisa.it 
#  Risi Davide   0622702013   d.risi2@studenti.unisa.it
#
#  Copyright (C) 2023 - All Rights Reserved 
#
#  This file is part of Parallelize-Tarjan-MPI-CUDA. 
#
#  Parallelize-Tarjan-MPI-CUDA is free software: you can redistribute it and/or modify 
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License, or 
#  (at your option) any later version. 
#
#  Parallelize-Tarjan-MPI-CUDA is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
#  GNU General Public License for more details. 
#
#  You should have received a copy of the GNU General Public License 
#  along with Parallelize-Tarjan-MPI-CUDA.  If not, see <http://www.gnu.org/licenses/>. 
#  

TARGETS  := common sequential sequential_pre tests tools
BIN_DIR := ./bin
DATA_DIR := ./data

TARGETS += $(if $(shell which mpicxx), common_mpi mpi, ) #add mpi targets if mpi compiler is present
TARGETS += $(if $(shell which nvcc), cuda cuda_texture, ) #add cuda targets if cuda compiler is present
TARGETS += $(if $(shell which nvcc && which mpicxx), cuda_mpi, ) #add cuda targets if cuda compiler is present

green := "\033[0;32m"
cyan := "\033[0;36m"
red := "\033[0;31m"
color_off := "\033[0m"

all: $(TARGETS)
	@echo ${green}Build completed successfully
	$(eval TMP := $(shell find . -name "*.out" -not -path "./bin/*"))
	@$(foreach d,$(TMP),cp $(d) ./bin;)

.PHONY: pre-build
pre-build:
ifeq (, $(shell which mpicxx))
	@echo ${red}mpicxx not found. Skipping mpi-related sources.${color_off}
endif
ifeq (, $(shell which nvcc))
	@echo ${red}nvcc not found. Skipping cuda-related sources.${color_off}
endif

.PHONY: $(TARGETS)
$(TARGETS): pre-build
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(DATA_DIR)
	@echo ${cyan}Building $@...${color_off}
	@cd $@ && $(MAKE) --no-print-directory

.PHONY: test
test:
	@for test in ./bin/test*; do \
		echo ${cyan}Executing test $$test ${color_off}; \
		$$test ./data/seed.bin && echo ${green}Test $$test passed successfully ${color_off}; \
	done
#	@$(foreach test, find bin -name "*test*", echo $(test);)

.PHONY: clean
clean:
	@$(foreach target,$(TARGETS),cd $(target); $(MAKE) clean; cd ..;)
	@rm -r $(BIN_DIR)
#	@cd $@ && $(MAKE) clean