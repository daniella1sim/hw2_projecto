import matplotlib
matplotlib.use('Agg')

import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import load_iris
from sklearn.cluster import KMeans


def calculate_inertia(data, centroids, labels):
    inertia = 0.0
    for i, point in enumerate(data):
        centroid = centroids[labels[i]]
        inertia += np.sum((point - centroid) ** 2)
    return inertia


def main():
    iris = load_iris()
    data = iris.data
    inertia = []
    for k in range(1, 11):
        kmeans = KMeans(n_clusters = k, init = 'k-means++', random_state = 0)
        kmeans.fit(data)
        inertia_value = calculate_inertia(data, kmeans.cluster_centers_, kmeans.labels_)
        inertia.append(inertia_value)
    
    deriviative = np.diff(inertia)
    second_derivative = np.diff(deriviative)
    elbow_k = np.argmax(second_derivative) + 2
    
    plt.plot(range(1,11), inertia)
    plt.title('Elbow Method for selection of optimal "K" clusters')
    plt.xlabel('K')
    plt.ylabel('Average dispersion')
     
    plt.annotate('Elbow Point', xy=(elbow_k, inertia[elbow_k-1]), xytext=(elbow_k+1, inertia[elbow_k-1]+100),
                 arrowprops=dict(facecolor='black', shrink=0.002))

    plt.savefig('elbow.png')
    return 0

if __name__ == "__main__":
    main()
