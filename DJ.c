#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NODES 100
#define INF 1e9

// Edge structure
typedef struct Edge {
    int dest;
    float weight;
    struct Edge *next;
} Edge;

// Node structure
typedef struct Node {
    int id;
    Edge *edges;
} Node;

// Graph structure
Node graph[MAX_NODES];
int num_nodes = 0;
int exits[MAX_NODES];
int num_exits = 0;

// Function to add an edge
void add_edge(int src, int dest, float weight) {
    Edge *new_edge = (Edge *)malloc(sizeof(Edge));
    new_edge->dest = dest;
    new_edge->weight = weight;
    new_edge->next = graph[src].edges;
    graph[src].edges = new_edge;
}

// Function to delete an edge between two nodes (undirected graph)
void delete_edge(int src, int dest) {
    // Delete edge from src to dest
    Edge* curr = graph[src].edges;
    Edge* prev = NULL;

    while (curr) {
        if (curr->dest == dest) {
            if (prev) {
                prev->next = curr->next;
            } else {
                graph[src].edges = curr->next;
            }
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    // Delete edge from dest to src (undirected graph)
    curr = graph[dest].edges;
    prev = NULL;

    while (curr) {
        if (curr->dest == src) {
            if (prev) {
                prev->next = curr->next;
            } else {
                graph[dest].edges = curr->next;
            }
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    printf("Path from %d to %d removed due to wall collapse!\n", src, dest);
}

// Simulate sensor data update
void update_weights() {
    for (int i = 1; i <= num_nodes; i++) {
        Edge *edge = graph[i].edges;
        while (edge) {
            edge->weight += (rand() % 100 - 50) * 0.5; // random change
            if (edge->weight < 1)
                edge->weight = 1;
            edge = edge->next;
        }
    }
}

// Dijkstra's algorithm to multiple exits
void dijkstra_multiple_exits(int start) {
    float dist[MAX_NODES];
    bool visited[MAX_NODES];
    int prev[MAX_NODES];

    for (int i = 0; i < MAX_NODES; i++) {
        dist[i] = INF;
        visited[i] = false;
        prev[i] = -1;
    }
    dist[start] = 0;

    for (int count = 1; count <= num_nodes; count++) {
        float min = INF;
        int u = -1;
        for (int i = 1; i <= num_nodes; i++) {
            if (!visited[i] && dist[i] < min) {
                min = dist[i];
                u = i;
            }
        }
        if (u == -1)
            break;
        visited[u] = true;

        Edge *edge = graph[u].edges;
        while (edge) {
            int v = edge->dest;
            if (!visited[v] && dist[u] + edge->weight < dist[v]) {
                dist[v] = dist[u] + edge->weight;
                prev[v] = u;
            }
            edge = edge->next;
        }
    }

    // Find the closest exit
    int best_exit = -1;
    float best_cost = INF;
    for (int i = 0; i < num_exits; i++) {
        int exit_node = exits[i];
        if (dist[exit_node] < best_cost) {
            best_cost = dist[exit_node];
            best_exit = exit_node;
        }
    }

    if (best_exit == -1 || best_cost == INF) {
        printf("No path found to any exit.\n");
    } else {
        printf("Safest path Danger to exit %d: %.2f\n", best_exit, best_cost);
        int path[MAX_NODES];
        int count = 0;
        for (int at = best_exit; at != -1; at = prev[at]) {
            path[count++] = at;
        }
        printf("Path: ");
        for (int i = count - 1; i >= 0; i--) {
            printf("%d ", path[i]);
        }
        printf("\n");
    }
}

// Read graph and exits from file
void read_graph_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        exit(1);
    }

    fscanf(file, "%d", &num_nodes);

    int src, dest;
    float weight;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n') continue; // skip empty lines

        if (strncmp(line, "EXITS", 5) == 0) {
            char *token = strtok(line + 6, " ");
            while (token != NULL) {
                exits[num_exits++] = atoi(token);
                token = strtok(NULL, " ");
            }
        } else {
            if (sscanf(line, "%d %d %f", &src, &dest, &weight) == 3) {
                add_edge(src, dest, weight);
                add_edge(dest, src, weight); // bi-directional
            }
        }
    }

    fclose(file);
}

// Function to print graph
void print_graph() {
    for (int i = 1; i <= num_nodes; i++) {
        Edge* curr = graph[i].edges;
        while (curr) {
            if (i < curr->dest) { // To avoid printing twice
                printf("Node %d to Node %d with weight %.2f\n", i, curr->dest, curr->weight);
            }
            curr = curr->next;
        }
    }
}


int main() {
    const char *filename = "graph.txt"; // Change filename if needed
    read_graph_from_file(filename);

    int start_node;
    printf("Enter Your Room No : ");
    scanf("%d", &start_node);

    printf("\nInitial path:\n");
    dijkstra_multiple_exits(start_node);

    delete_edge(2, 9); // wall collapse simulation
    //print_graph();

    printf("\nUpdating The Danger Values (simulated sensor data)...\n\n");
    update_weights();

    printf("Path after weight update:\n");
    dijkstra_multiple_exits(start_node);

    return 0;
}
