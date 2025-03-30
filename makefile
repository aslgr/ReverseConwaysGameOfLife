# Kissat Solver
KISSAT_DIR = ./kissat
KISSAT_INCLUDE = $(KISSAT_DIR)/src
KISSAT_LIB = $(KISSAT_DIR)/build/libkissat.a


# Flags
CFLAGS += -I$(KISSAT_INCLUDE)
LDFLAGS += $(KISSAT_LIB) -lm
FLAGS += -Wall -g

CC = gcc
SRC = ./src
OBJ = ./obj
INPUTS = ./inputs

# Gerar lista de arquivos .c e .o
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(SRCS:$(SRC)/%.c=$(OBJ)/%.o)

# Compilar
ConwayGoLRGSMLC: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $@

# Create the obj directory once before compiling
$(OBJ):
	mkdir -p $(OBJ)

# Install Kissat
install_kissat:
	git clone https://github.com/arminbiere/kissat.git
	cd $(KISSAT_DIR) && ./configure && make	

# Clean
clean:
	rm -f ConwayGoLRGSMLC $(OBJ)/*.o
	rm -rf $(OBJ)

clean_kissat:
	cd $(KISSAT_DIR) && make clean

# Compact
compact:
	tar -cvzf ConwayGoLRGSMLC.tar.gz $(SRC) $(KISSAT_DIR) $(INPUTS) makefile README.md