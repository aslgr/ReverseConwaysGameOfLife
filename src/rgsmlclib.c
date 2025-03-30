#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rgsmlclib.h"

void print_grid(int *grid, int n, int m)
{
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            printf("%d ", grid[i * m + j]);
        }
        printf("\n");
    }
}

void add_row(int ***clauses, int *n_clauses, int m_clauses)
{
    // Realoca memória para uma nova linha
    (*clauses) = realloc((*clauses), ((*n_clauses) + 1) * sizeof(int *));
    if ((*clauses) == NULL) {
        printf("Erro ao realocar memória para as linhas.\n");
        exit(1);
    }

    // Aloca memória para a nova linha com o número fixo de colunas
    (*clauses)[(*n_clauses)] = malloc(m_clauses * sizeof(int));
    if ((*clauses)[*n_clauses] == NULL) {
        printf("Erro ao alocar memória para a nova linha.\n");
        exit(1);
    }

    // Incrementa o número de linhas
    (*n_clauses)++;
}

void write_clause(kissat *solver, int *clause, int size)
{
    for(int i = 0; i < size; i++) {
        kissat_add(solver, clause[i]);
    }
    kissat_add(solver, 0);
}

void write_clause_aux(kissat *solver, int n_clauses, int **clauses)
{
    for (int i = 0; i < n_clauses; i++)
    {
        int count_clause = clauses[i][0];
        int clause[count_clause];
        for (int j = 0; j < count_clause; j++)
            clause[j] = clauses[i][j+1];

        write_clause(solver, clause, count_clause);
    }
}

// Verifica se uma célula está dentro da matriz
bool is_valid(int i, int j, int n, int m) {
    return i >= 0 && i < n && j >= 0 && j < m;
}

// Operações loneliness, stagnation, overcrowding, preservation e life
void operations(int i, int j, int n, int m, int ***clauses, int *n_clauses, int op)
{
    // Deslocamentos para os 8 vizinhos
    int dx[NEIGHBOURS] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[NEIGHBOURS] = {-1, 0, 1, -1, 1, -1, 0, 1};

    // Obter os vizinhos válidos
    int neighbors[NEIGHBOURS] = {0};
    int valid_neighbors = 0;

    for (int k = 0; k < NEIGHBOURS; k++) {
        int ni = i + dx[k];
        int nj = j + dy[k];

        if (is_valid(ni, nj, n, m)) {
            neighbors[valid_neighbors++] = ni * m + nj + 1; // Indexação positiva para SAT solver
        }
    }

    // Gerar combinações de vizinhos
    int total_combinations = 1 << valid_neighbors; // 2^valid_neighbors

    for (int comb = 0; comb < total_combinations; comb++) {
        int alive_count = 0;
        for (int bit = 0; bit < valid_neighbors; bit++) {
            if ((comb & (1 << bit))) {
                alive_count++;
            }
        }

        switch (op)
        {
        // Loneliness: A cell with fewer than 2 live neighbours (at least 7 dead neighbours) at time t0 is dead at time t1, irrespective of its own state at t0.
        case LONELINESS:
            
            if (alive_count == 1) 
            {
                add_row(clauses, n_clauses, NEIGHBOURS+2);

                int clause_index = 0;
                
                for (int bit = 0; bit < valid_neighbors; bit++) {
                    if (!(comb & (1 << bit)))
                        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = neighbors[bit];
                }

                (*clauses)[(*n_clauses)-1][0] = clause_index;
            }

        break;

        // Stagnation: A dead cell with exactly two live neighbours at time t0 will still be dead at time t1.
        case STAGNATION:

            if (alive_count == 2) 
            {
                add_row(clauses, n_clauses, NEIGHBOURS+2);

                int clause_index = 0;

                (*clauses)[(*n_clauses)-1][(clause_index++)+1] = (i * m + j + 1);

                for (int bit = 0; bit < valid_neighbors; bit++) {
                    if (comb & (1 << bit)) {
                        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = -neighbors[bit];
                    } else {
                        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = neighbors[bit];
                    }
                }

                (*clauses)[(*n_clauses)-1][0] = clause_index;
            }

        break;

        // Overcrowding: A cell with four or more live neighbours at time t0 will be dead at time t1 irrespective of its own state at t0.
        case OVERCROWDING:

            if (alive_count == 4) 
            {
                add_row(clauses, n_clauses, NEIGHBOURS+2);

                int clause_index = 0;

                for (int bit = 0; bit < valid_neighbors; bit++) {
                    if (comb & (1 << bit))
                        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = -neighbors[bit];
                }

                (*clauses)[(*n_clauses)-1][0] = clause_index;
            }

        break;

        // Preservation: A cell that is alive at time t0 with exactly two live neighbours will remain alive at time t1.
        case PRESERVATION:

            if (alive_count == 2) 
            {
                add_row(clauses, n_clauses, NEIGHBOURS+2);

                int clause_index = 0;

                (*clauses)[(*n_clauses)-1][(clause_index++)+1] = -(i * m + j + 1);

                for (int bit = 0; bit < valid_neighbors; bit++) {
                    if (comb & (1 << bit)) {
                        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = -neighbors[bit];
                    } else {
                        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = neighbors[bit];
                    }
                }

                (*clauses)[(*n_clauses)-1][0] = clause_index;
            }

        break;

        // Life: A: A cell with exactly 3 live neighbours at time t0 will be aliveat time t1, irrespective of its prior state.
        case LIFE:

            if (alive_count == 3)
            {
                add_row(clauses, n_clauses, NEIGHBOURS+2);

                int clause_index = 0;

                for (int bit = 0; bit < valid_neighbors; bit++) {
                    if (comb & (1 << bit)) {
                        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = -neighbors[bit];
                    } else {
                        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = neighbors[bit];
                    }
                }

                (*clauses)[(*n_clauses)-1][0] = clause_index;
            }

        break; 
        
        default:

            printf("Erro, operação não reconhecida\n");
            exit(1);

        break;
        }
    }
}

void solve_conway_gol(int *grid, int ***clauses, int *n_clauses, int n, int m)
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            if (grid[i * m + j] == 1) {
                operations(i, j, n, m, clauses, n_clauses, LONELINESS);
                operations(i, j, n, m, clauses, n_clauses, STAGNATION);
                operations(i, j, n, m, clauses, n_clauses, OVERCROWDING);
            } else {
                operations(i, j, n, m, clauses, n_clauses, PRESERVATION);
                operations(i, j, n, m, clauses, n_clauses, LIFE);
            }
        }
    }
}

void add_live_cells_limit(int *live_cells, int count_live_cells, int ***clauses, int *n_clauses, int n, int m)
{
    add_row(clauses, n_clauses, count_live_cells+1);

    int clause_index = 0;

    for (int i = 0; i < count_live_cells; i++)
        (*clauses)[(*n_clauses)-1][(clause_index++)+1] = -live_cells[i];

    (*clauses)[(*n_clauses)-1][0] = clause_index;
}