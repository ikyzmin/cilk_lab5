#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>


#ifndef INT_MAX
#define INT_MAX 2000000000
#endif

const int SERIAL = 0;
const int CILK = 1;
const int OPENMP = 2;


void dijkstra(int start, bool **adj_matrix, int **cost, int size) {
    int *dist;
    int *parent;
    bool *in_tree;
    dist = (int *) calloc(size, sizeof(int));
    parent = (int *) calloc(size, sizeof(int));
    in_tree = (bool *) calloc(size, sizeof(bool));

    for (int i = 0; i < size; i++)
        dist[i] = INT_MAX;
    dist[start] = 0;

    int cur = start;

    while (!in_tree[cur]) {
        in_tree[cur] = true;
        for (int i = 0; i < size; i++) {
            if (adj_matrix[cur][i]) {
                int d = dist[cur] + cost[cur][i];

                if (d < dist[i]) {
                    dist[i] = d;
                    parent[i] = cur;
                }
            }
        }

        int min_dist = INT_MAX;
        for (int i = 0; i < size; i++) {
            if (!in_tree[i] && dist[i] < min_dist) {
                cur = i;
                min_dist = dist[i];
            }
        }
    }
    free(dist);
    free(parent);
    free(in_tree);

}

void dijkstrainit(bool **adj_matrix, int **cost, int size) {
    for (int i = 0; i < size; i++) {
        dijkstra(i, adj_matrix, cost, size);
    }
}

void dijkstrainitCilk(bool **adj_matrix, int **cost, int size) {
    for (int i = 0; i < size; i++) {
        cilk_spawn dijkstra(i, adj_matrix, cost, size);
    }
}

void dijkstrainitOmp(bool **adj_matrix, int **cost, int size) {
    for (int i = 0; i < size; i++) {
#pragma omp task
        dijkstra(i, adj_matrix, cost, size);
    }
}

void init(bool **adj_matrix, int **cost, int size) {

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            int x = rand() % 130;
            if (i != j) {
                adj_matrix[i][j] = true;
                cost[i][j] = x;
            } else {
                adj_matrix[i][j] = false;
                cost[i][j] = 0;
            }
        }
}

void restartExperiment(int TYPE) {
    srand(time(NULL));
    int lengths[] = {100, 200, 500, 1000, 2000, 3000, 4000, 5000};
    bool **adj_matrix;
    int **cost;
    double averageTime;
    switch (TYPE) {
        case SERIAL:
            printf("Serial___________\n");
            break;
        case CILK:
            printf("Cilk_____________\n");
            break;
        case OPENMP:
            printf("OpenMp___________\n");
            break;
        default:
            printf("Experiment type undefined");
            return;
    }
    for (int i = 0; i < 8; i++) {
        averageTime = 0;
        adj_matrix = (bool **) calloc(lengths[i], sizeof(bool *));
        cost = (int **) calloc(lengths[i], sizeof(int *));
        for (int j = 0; j < lengths[i]; j++) {
            adj_matrix[j] = (bool *) calloc(lengths[i], sizeof(bool));
            cost[j] = (int *) calloc(lengths[i], sizeof(int));
        }
        for (int j = 0; j < lengths[i]; j++) {
            for (int k = 0; k < lengths[i]; k++) {
                adj_matrix[j][k] = false;
            }
        }
        init(adj_matrix, cost, lengths[i]);
        printf("Size(%dx%d)\n", lengths[i], lengths[i]);
        for (int j = 0; j < 5; j++) {
            double st = omp_get_wtime();
            switch (TYPE) {
                case SERIAL:
                    dijkstrainit(adj_matrix, cost, lengths[i]);
                    break;
                case CILK:
                    dijkstrainitCilk(adj_matrix, cost, lengths[i]);
                    break;
                case OPENMP:
#pragma omp parallel num_threads(4)
#pragma omp single
                    dijkstrainitOmp(adj_matrix, cost, lengths[i]);
                    break;
                default:
                    printf("undefined type");

            }

            double fn = omp_get_wtime();
            averageTime += (fn - st);
        }
        averageTime /= 5;
        printf("Average time is %f seconds\n", averageTime);
        free(adj_matrix);
        free(cost);
    }
}

int main() {
    __cilkrts_set_param("nworkers", "4");
    //restartExperiment(SERIAL);
    restartExperiment(CILK);
    restartExperiment(OPENMP);
}
