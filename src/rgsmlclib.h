#include <stdio.h>
#include <stdlib.h>
#include <kissat.h>

#define NEIGHBOURS 8
#define LONELINESS 1
#define STAGNATION 2
#define OVERCROWDING 3
#define PRESERVATION 4
#define LIFE 5

// Funções auxiliares

void print_grid(int *grid, int n, int m);

void write_clause_aux(kissat *solver, int n_clauses, int **clauses);

void solve_conway_gol(int *grid, int ***clauses, int *n_clauses, int n, int m);

void add_live_cells_limit(int *live_cells, int count_live_cells, int ***clauses, int *n_clauses, int n, int m);
