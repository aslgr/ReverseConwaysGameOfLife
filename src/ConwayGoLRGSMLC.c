#include <stdio.h>
#include <stdlib.h>
#include <kissat.h>
#include <time.h>
#include "rgsmlclib.h"

int main() 
{
    int n, m, n_clauses = 0, UNSAT_flag = 1;

    // Lendo os valores n e m
    scanf("%d %d", &n, &m);

    int *ActualStateGrid, *FinalStateGrid, *live_cells, **clauses = NULL;

    // Lendo o tabuleiro de entrada
    if((ActualStateGrid = (int *) malloc(n * m * sizeof(int))) != NULL) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                scanf("%d", &ActualStateGrid[i * m + j]);
            }
        }
    }
    else {
        printf("ERROR: Could not allocate memory for the actual state grid\n");
        return 1;
    }

    // Alocando espaço para o tabuleiro resposta
    if((FinalStateGrid = (int *) malloc(n * m * sizeof(int))) != NULL) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                FinalStateGrid[i * m + j] = 0;
            }
        }
    }
    else {
        printf("ERROR: Could not allocate memory for the final state grid\n");
        return 1;
    }

    // Alocando espaço para o vetor de células vivas
    if((live_cells = (int *) malloc(n * m * sizeof(int))) == NULL) {
        printf("ERROR: Could not allocate memory for the live cells\n");
        return 1;
    }

    // Função que cria cláusulas a partir das regras do jogo e do tabuleiro inicial
    solve_conway_gol(ActualStateGrid, &clauses, &n_clauses, n, m);

    printf("Revertendo tabuleiro. Por favor aguarde...\n");

    // Inicia o solver
    kissat *solver = kissat_init();
    // Silencia as mensagens do kissat
    kissat_set_option(solver, "quiet", 1);

    // Compartilha as cláusulas obtidas com o solver
    write_clause_aux(solver, n_clauses, clauses);

    // Tenta encontrar uma solução válida
    int result = kissat_solve(solver);

    int min_live_cells = n*m;

    clock_t tempo_inicial = clock(); // Captura o tempo inicial
    double tempo_max = 300.0;      // Tempo limite em segundos

    while (result == 10)
    {
        UNSAT_flag = 0;

        // Verificar se o tempo limite foi atingido
        double tempo = (double)(clock() - tempo_inicial) / CLOCKS_PER_SEC;
        if (tempo >= tempo_max) 
        {
            printf("\nTempo limite atingido de %.2f segundos.\n", tempo_max);
            break;
        }

        int count_live_cells = 0;

        for (int i = 0; i < n; i++) 
        {
            for (int j = 0; j < m; j++) 
            {
                int variable = i * m + j + 1; // Variável lógica 1-baseada
                int value = kissat_value(solver, variable); // Recupera o valor do solver

                if (value > 0)
                    live_cells[count_live_cells++] = value;
            }
        }

        if (count_live_cells < min_live_cells)
        {
            min_live_cells = count_live_cells;

            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    int variable = i * m + j + 1; // Variável lógica 1-baseada
                    int value = kissat_value(solver, variable); // Recupera o valor do solver
                    FinalStateGrid[i * m + j] = (value > 0) ? 1 : 0;
                }
            }
        }

        kissat_release(solver);

        // Inicia o solver
        solver = kissat_init();
        // Silencia as mensagens do kissat
        kissat_set_option(solver, "quiet", 1);

        add_live_cells_limit(live_cells, count_live_cells, &clauses, &n_clauses, n, m);

        write_clause_aux(solver, n_clauses, clauses);

        result = kissat_solve(solver);
    }
    
    kissat_release(solver);

    if (UNSAT_flag)
    {
        printf("\nUNSAT\n");
    } else {
        printf("\nResultado:\n\n%d %d\n", n, m);
        print_grid(FinalStateGrid, n, m);
        printf("\n");
    }

    return 0;
}