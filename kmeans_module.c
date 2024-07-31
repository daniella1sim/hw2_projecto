/**
 * @file kmeans.c
 * @brief Calculating a k mean clusters from a given input file
 * 
 * This file contains code that given a number k, iterations and input file returns the centroids of the k mean cluster.
 * 
 * @author 322721705 and 211493176
 */
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


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


double **data = NULL;
int K;
int D;
int N;
int iter;
double eps;
CLUSTER_LIST *cluster_list = NULL;


/**
 * @brief Processes a given python matrix and stores it in a 2D array.
 * 
 * This function takes a 2D matrix in the form of a Python list and stores it in a 2D array.
 * 
 * @param array 2D matrix in the form of a Python list
 * @return None, all changed parameters are called by reference
 */
void processMatrix(PyObject* array) {
    int i;
    int j;
    PyObject *row;
    PyObject *item;

    data = (double **)malloc(N * sizeof(double *));
    if (data == NULL) {
        PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
        return NULL;
    }
    for (i = 0; i < N; i++) {
        data[i] = (double *)malloc(D * sizeof(double));
        if (data[i] == NULL) {
            PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
            for (j = 0; j < i; j++) {
                free(data[j]);
            }
            free(data);
            return NULL;
        }
    }
    for (i = 0; i < N; i++) {
        row = PyList_GetItem(array, (Py_ssize_t)i);
        for (j = 0; j < D; j++) {
            item = PyList_GetItem(row, (Py_ssize_t)j);
            data[i][j] = PyFloat_AsDouble(item);
        }
    }

}


/**
 * @brief Creates a cluster with a given point.
 * 
 * This function takes a point in Rd and creates a cluster with the given point as its centroid.
 * 
 * @param point Point in Rd
 * @return Pointer to the created cluster
 */
CLUSTER *createCluster(double *point){
    CLUSTER *cluster = calloc(1, sizeof(CLUSTER));
    int i;

    if (cluster == NULL){
        PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
        return NULL;
    }
    
    cluster->centroid = (double *)calloc(D, sizeof(double));
    cluster->prev = (double *)calloc(D, sizeof(double));

    if (cluster->centroid == NULL || cluster->prev == NULL){
        PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
        free(cluster);
        if (cluster->centroid != NULL) free(cluster->centroid);
        else if (cluster->prev != NULL) free(cluster->prev);
        return NULL;
    }
    
    for (i = 0; i < D; i++) {
        cluster->prev[i] = point[i];
        cluster->centroid[i] = point[i];
    }
    cluster->point_list = NULL;
    cluster->size = 0;
    return cluster;
}


/**
 * @brief Adds cluster to a cluster list
 * 
 * This function adds a cluster to a linked list representing a cluster list, also check memory allocation errors.
 * 
 * @param cluste Pointer to a cluster
 * @return None, all changed parameters are called by reference
 */
void addCluster(CLUSTER *cluster) {
    CLUSTER_LIST *tmp = (CLUSTER_LIST *)calloc(1, sizeof(CLUSTER_LIST));
    if (tmp == NULL) {
        PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
        return NULL;
    }
    tmp->head = cluster;
    tmp->next = cluster_list;
    cluster_list = tmp;
}


/**
 * @brief Initializes cluster list
 * 
 * This function takes the data matrix, pointer to cluster list, an int K and number of dimensions D.
 * It creates a linked list of size D of clusters and initiates their centroids.
 * 
 * @param array, 1d array of integers representing the indexes of the centroids
 * @return None, all changed parameters are called by reference.
 */
void *initializeClusters(PyObject *array) {
    CLUSTER *cluster;
    int i;
    long index;
    int j;
    PyObject *item;

    for (i = 0; i < K; i++) {
        item = PyList_GetItem(array, (Py_ssize_t)i);
        index = PyLong_AsLong(item);
        j = (int)index;
        cluster = createCluster(data[j]);
        addCluster(cluster);
    }
}


/**
 * @brief Sets up the data and clusters for the K-means algorithm using arguments from python.
 * 
 * This function parses input arguments, initializes data and clusters, and prepares for the K-means iterations.
 * 
 * @param data_py Python list of data points.
 * @param centroids_py Python list of initial centroids.
 */
void kmeansSetup(PyObject *data_py, PyObject *centroids_py) {
    Py_ssize_t num_rows = PyList_Size(data_py);
    Py_ssize_t num_cols = PyList_Size(PyList_GetItem(data_py, 0));
    N = (int)num_rows;
    D = (int)num_cols;
    Py_ssize_t centroid_size = PyList_Size(centroids_py);
    K = (int)centroid_size;

    processMatrix(data_py);
    initializeClusters(centroids_py);
}


/**
 * @brief Calculates distance between two points
 * 
 * This function takes 2 points in Rd and D and calculates the eucledean distance between the two points
 * 
 * @param point_a Point 1
 * @param point_b Point 2
 * @return Euclidean distance between the 2 points
 */
double calculateDistance(double *point_a, double *point_b){
    double sum = 0.0;
    double diff = 0.0;
    int i;
    for (i = 0; i < D; i++){
        diff = point_a[i] - point_b[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}


/**
 * @brief Finds the closest cluster to a point
 * 
 * This function takes a point and iterates through cluster list in order to find the closest cluster to the given point.
 * 
 * @param point point in Rd 
 * @return Pointer to the closest cluster to the point
 */
CLUSTER *findClosestCluster(double *point) {
    double minDist = INFINITY;
    double dist;
    CLUSTER *closestCluster = NULL;
    CLUSTER_LIST *curr = cluster_list;
    while (curr != NULL) {
        dist = calculateDistance(point, curr->head->centroid);
        if (dist < minDist) {
            minDist = dist;
            closestCluster = curr->head;
        }
        curr = curr->next;
    }
    return closestCluster;
}


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
        PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
        return NULL;
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
 * @brief Calculates and updates cenetroid of a cluster
 * 
 * This function takes a cluster, iterates over its point list and calculates the mean of each coordinate of point.
 * Then the function updates value of prev and centroid in cluster.
 * 
 * @param cluster Cluster
 * @return None, all changed parameters are called by reference
 */
void updateCentroid(CLUSTER *cluster){
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
 * @brief Adds all points to the cluster list
 * 
 * This function takes the data matrix, pointer to cluster list, an int N and number of dimensions D.
 * It iterates through all of the points (lines) in the matrix asnd adds each point to its closest cluster.
 * 
 * @return None, all changed parameters are called by reference.
 */
void addPointsToClusters() {
    int i;
    CLUSTER *cluster;
    for (i = 0; i < N; i++) {
        cluster = findClosestCluster(data[i]);
        addPoint(&cluster->point_list, data[i]);
        cluster->size++;
    }
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
 * @brief Performs a single iteration of the K-means algorithm.
 * 
 * This function assigns data points to the closest cluster, updates the centroids, and checks convergence.
 * 
 * @return 1 if the algorithm should continue, 0 if it has converged.
 */
int kmeansIteration() {
    int flag = 0;
    double diff;
    CLUSTER_LIST *curr;
    
    addPointsToClusters();
    
    curr = cluster_list;
    while (curr != NULL) {
        for (int i = 0; i < D; i++) {
            curr->head->prev[i] = curr->head->centroid[i];
        }
        updateCentroid(curr->head);
        diff = calculateDistance(curr->head->prev, curr->head->centroid);
        if (diff > eps) flag = 1;
        clearCluster(curr->head);
        curr = curr->next;
    }
    return flag;
}


/**
 * @brief Converts a cluster list to a Python centroid list.
 *
 * This function takes cluster list and converts it to a Python list of all cluster centroids.
 *
 * @return Python list
 */
PyObject* convert_c_matrix_to_py_list() {
    PyObject *py_list;
    PyObject *row_list;
    PyObject *item;
    int i;
    int j;
    CLUSTER_LIST *curr = cluster_list;

    py_list = PyList_New((Py_ssize_t)K);
    if (py_list == NULL) {
        PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
        return NULL;
    }

    for (i = 0; i < K; i++) {
        row_list = PyList_New((Py_ssize_t)D); 
        if (row_list == NULL) {
            PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
            Py_DECREF(py_list);
            return NULL;
        }
        
        for (j = 0; j < D; j++) {
            item = PyFloat_FromDouble(curr->head->centroid[j]); 
            if (item == NULL) {
                PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
                Py_DECREF(row_list);
                Py_DECREF(py_list);
                return NULL;
            }
            
            PyList_SET_ITEM(row_list, (Py_ssize_t)j, item);
        }
        PyList_SET_ITEM(py_list, (Py_ssize_t)i, row_list); 
        curr = curr->next;
    }
    return py_list;
}


/**
 * @brief Frees all memory allocated in the program
 * 
 * This function takes the data matrix, cluster list and N and frees all allocated data
 * 
 * @return None
 */

void freeMemory() {
    CLUSTER_LIST *curr = cluster_list;
    CLUSTER_LIST *next;
    int i;
    
    if (data != NULL) {
        for (i = 0; i < N; i++) {
            free((data)[i]);
        }
        free(data);
        data = NULL; 
    }
    while (curr != NULL) {
        next = curr->next;
        if (curr->head != NULL) {
            free(curr->head->centroid);
            free(curr->head->prev);
            clearCluster(curr->head);
            free(curr->head);
        }
        free(curr);
        curr = next;
    }
    cluster_list = NULL;
}


/**
 * @brief Runs the K-means algorithm
 * 
 * This function takes the number of iterations, epsilon value, data and initial cnetroids and runs the K-means algorithm.
 * 
 * @param self
 * @param args
 * @return Python list of centroids
 */
static PyObject* kmeans(PyObject *self, PyObject *args){
    PyObject *data_py;
    PyObject *centroids_py;
    PyObject *newdata;
    int flag;

    if (!PyArg_ParseTuple(args, "idOO", &iter, &eps, &data_py, &centroids_py)) {
        PyErr_SetString(PyExc_ValueError, "An error has occurred!");
        return NULL;
    }
    kmeansSetup(data_py, centroids_py);
    while (iter > 0){ 
        flag = kmeansIteration();
        if (flag == 0) break;
        iter--;
    }
    newdata = convert_c_matrix_to_py_list();
    if (newdata == NULL) PyErr_SetString(PyExc_MemoryError, "An error has occurred!");
    
    freeMemory();
    return newdata;  
}


/**
 * @brief Represents the methods of the module
 * 
 * This structure represents the methods of the module.
 */
static PyMethodDef KMeansMethods[] = {
    {"kmeans", kmeans, METH_VARARGS, "Run K-means clustering."},
    {NULL, NULL, 0, NULL}
};


/**
 * @brief Represents the module
 * 
 * This structure represents the module and its methods.
 */
static struct PyModuleDef kmeans_module = {
    PyModuleDef_HEAD_INIT,
    "kmeans_c",
    NULL,
    -1,
    KMeansMethods 
};


/**
 * @brief Initializes the module
 * 
 * This function initializes the module and returns it.
 * 
 * @return Module
 */
PyMODINIT_FUNC PyInit_kmeans_module(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeans_module);
    if (!m) {
        return NULL;
    }
    return m;
}
