import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import load_iris
from sklearn.cluster import KMeans


def main():
    iris = load_iris()
    data = iris.data
    inertia = []
    for k in range(1, 11):
        kmeans = KMeans(n_clusters = k, init = 'k-means++', random_state = 0)
        kmeans.fit(data)
        inertia.append(kmeans.inertia_)
    
    deriviative = np.diff(inertia)
    second_derivative = np.diff(deriviative)
    elbow_k = np.argmax(second_derivative) + 1
    
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
