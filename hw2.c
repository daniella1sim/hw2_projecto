/**
 * @file kmeans.c
 * @brief Calculating a k mean clusters from a given input file
 * 
 * This file contains code that given a number k, iterations and input file returns the centroids of the k mean cluster.
 * 
 * @author 322721705 and 211493176
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 0.0001
#define MAX_PATH 200

/**
 * @brief Represents a linked list of points.
 *
 * This structure holds a dynamically allocated array of points and a pointer to the next element in the list.
 * It is used to manage the points assigned to a cluster in a linked list format.
 */
typedef struct POINT_LIST{
    double *head;
    struct POINT_LIST *next;
    int size;
} POINT_LIST;


/**
 * @brief Represents a cluster with its centroid, previous centroid, and list of points.
 *
 * This structure is used in clustering algorithms to manage clusters,
 * including their current and previous centroids and the list of points assigned to the cluster.
 */
typedef struct CLUSTER{
    double *centroid;
    double *prev;
    POINT_LIST *point_list;
    int size;
} CLUSTER;


/**
 * @brief Represents a linked list of clusters.
 *
 * This structure is used to manage multiple clusters in a linked list format.
 */
typedef struct CLUSTER_LIST{
    CLUSTER *head;
    struct CLUSTER_LIST *next;
    int size;
} CLUSTER_LIST;


/**
 * @brief point to a given point list.
 * 
 * This function takes a point in R^D and adds it to the head of a linked list representing point list
 * 
 * @param point_list Pointer to a point list
 * @param point Pointer to a double array representing the point
 * @return None, all changed parameters are called by reference
 */
void addPoint( POINT_LIST **point_list , double *point) {
    POINT_LIST *tmp = (POINT_LIST *)calloc(1, sizeof(POINT_LIST));
    if (tmp == NULL) {
            printf("An error has occurred!");
            exit(1);
    }
    tmp->head = point;
    tmp->next = *point_list;
    if (*point_list == NULL) {
        tmp->size = 1;
    } else {
        tmp->size = (*point_list)->size + 1;
    }
    *point_list = tmp;
}   


/**
 * @brief Adds cluster to a cluster list
 * 
 * This function adds a cluster to a linked list representing a cluster list, also check memory allocation errors.
 * 
 * @param cluster_list Pointer to a cluster list
 * @param cluste Pointer to a cluster
 * @return None, all changed parameters are called by reference
 */
void addCluster(CLUSTER_LIST **cluster_list , CLUSTER *cluster) {
    CLUSTER_LIST *tmp = (CLUSTER_LIST *)calloc(1, sizeof(CLUSTER_LIST));
    if (tmp == NULL) {
            printf("An error has occurred!");
            exit(1);
        }
    tmp->head = cluster;
    tmp->next = *cluster_list;
    *cluster_list = tmp;
}


/**
 * @brief Initiates cluster
 * 
 * This function takes a point in R^D and int D.
 * It allocates memory for cluster, checks memory allocation errors and initializes its centroid and prev using the coordinates of the point.
 * 
 * @param point Pointer to a point
 * @param D Number of dimensions
 * @return Pointer to the initiated cluster
 */
CLUSTER *createCluster(double *point, int D){
    CLUSTER *cluster = calloc(1, sizeof(CLUSTER));
    int i;
    if (cluster == NULL){
        printf("An error has occurred!");
        exit(1);
    }
    cluster->centroid = (double *)calloc(D + 1, sizeof(double));
     if (cluster->centroid == NULL){
        printf("An error has occurred!");
        exit(1);
    }
    cluster->prev = (double *)calloc(D + 1, sizeof(double));
     if (cluster->prev == NULL){
        printf("An error has occurred!");
        exit(1);
    }
    for (i = 0; i < D; i++) {
        cluster->prev[i] = (double) point[i];
        cluster->centroid[i] = (double) point[i];
    }
    cluster->point_list = NULL;
    cluster->size = 0;
    return cluster;
}


/**
 * @brief Clears point list in given cluster.
 * 
 * This function frees all memory allocated in cluster list  by iterating on the linked list and using free()
 * 
 * @param cluster Pointer to a point list
 * @return None, all changed parameters are called by reference
 */
void clearCluster(CLUSTER *cluster) {
    POINT_LIST *curr = cluster->point_list;
    POINT_LIST *next;
    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
    cluster->point_list = NULL;
    cluster->size = 0;
}


/**
 * @brief Calculates and updates cenetroid of a cluster
 * 
 * This function takes a cluster, iterates over its point list and calculates the mean of each coordinate of point.
 * Then the function updates value of prev and centroid in cluster.
 * 
 * @param cluster Cluster
 * @param D Dimension
 * @return None, all changed parameters are called by reference
 */
void updateCentroid(CLUSTER *cluster, int D){
    int i;
    double sum;
    POINT_LIST *curr;
    for (i = 0; i < D; i++){
        sum = 0.0;
        curr = cluster->point_list;
        while (curr != NULL) {
            sum += curr->head[i];
            curr = curr->next;
        }
        cluster->centroid[i] = sum / cluster->size;
    }
}


/**
 * @brief Calculates the dimension of the given vectors
 * 
 * This function tallies the coordinates listed in the first line.
 * @param D pointer to parameter D
 * @return None, changed parameter is called by reference.
 */

void dimension(int *D){
    char ch;
    while((ch = getchar()) != '\n'){
        if (ch == ','){
            (*D)++;
        }
    } 
    (*D)++;
    fseek(stdin, 0, SEEK_SET);
}