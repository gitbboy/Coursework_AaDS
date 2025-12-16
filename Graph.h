#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <iomanip>
#include <unordered_set>
#include <sstream>

using namespace std;

class Graph {
private:
    int V;
    vector<list<int>> adj;
    vector<vector<bool>> adjMatrix;
    
    void buildAdjMatrix();
    bool isIsomorphic(const Graph& other, const vector<int>& mapping) const;
    bool backtrackIsomorphism(const Graph& pattern,
                             vector<int>& mapping,
                             vector<bool>& used,
                             int depth,
                             vector<vector<int>>& allMappings) const;
    
public:
    Graph();
    explicit Graph(int vertices);
    Graph(const vector<pair<int, int>>& edges, int vertices);
    
    void addEdge(int u, int v);
    void removeEdge(int u, int v);
    bool hasEdge(int u, int v) const;
    int getDegree(int v) const;
    int getVertexCount() const { return V; }
    int getEdgeCount() const;
    
    void generateRandom(int vertices, double edgeProbability);
    vector<vector<int>> findIsomorphicSubgraph(const Graph& pattern) const;
    
    void printMatrix() const;
    void printInfo() const;
    
    const vector<vector<bool>>& getAdjMatrix() const { return adjMatrix; }
    const vector<list<int>>& getAdjacencyList() const { return adj; }
};

// Реализация методов класса Graph (inline)
inline void Graph::buildAdjMatrix() {
    adjMatrix.clear();
    adjMatrix.resize(V, vector<bool>(V, false));
    
    for (int i = 0; i < V; ++i) {
        for (int neighbor : adj[i]) {
            adjMatrix[i][neighbor] = true;
            adjMatrix[neighbor][i] = true;
        }
    }
}

inline Graph::Graph() : V(0) {}

inline Graph::Graph(int vertices) : V(vertices), adj(vertices) {
    if (vertices > 0) {
        buildAdjMatrix();
    }
}

inline Graph::Graph(const vector<pair<int, int>>& edges, int vertices) : V(vertices), adj(vertices) {
    for (const auto& edge : edges) {
        int u = edge.first;
        int v = edge.second;
        if (u >= 0 && u < V && v >= 0 && v < V && u != v) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }
    buildAdjMatrix();
}

inline void Graph::addEdge(int u, int v) {
    if (u >= 0 && u < V && v >= 0 && v < V && u != v) {
        auto it = find(adj[u].begin(), adj[u].end(), v);
        if (it == adj[u].end()) {
            adj[u].push_back(v);
            adj[v].push_back(u);
            adjMatrix[u][v] = true;
            adjMatrix[v][u] = true;
        }
    }
}

inline void Graph::removeEdge(int u, int v) {
    if (u >= 0 && u < V && v >= 0 && v < V) {
        adj[u].remove(v);
        adj[v].remove(u);
        adjMatrix[u][v] = false;
        adjMatrix[v][u] = false;
    }
}

inline bool Graph::hasEdge(int u, int v) const {
    if (u >= 0 && u < V && v >= 0 && v < V) {
        return adjMatrix[u][v];
    }
    return false;
}

inline int Graph::getDegree(int v) const {
    if (v >= 0 && v < V) {
        return adj[v].size();
    }
    return 0;
}

inline int Graph::getEdgeCount() const {
    int count = 0;
    for (int i = 0; i < V; ++i) {
        count += adj[i].size();
    }
    return count / 2;
}

inline void Graph::generateRandom(int vertices, double edgeProbability) {
    V = vertices;
    adj.clear();
    adj.resize(V);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int i = 0; i < V; ++i) {
        for (int j = i + 1; j < V; ++j) {
            if (dis(gen) < edgeProbability) {
                adj[i].push_back(j);
                adj[j].push_back(i);
            }
        }
    }
    
    buildAdjMatrix();
}

inline bool Graph::isIsomorphic(const Graph& other, const vector<int>& mapping) const {
    int patternV = other.getVertexCount();
    
    unordered_set<int> used;
    for (int i = 0; i < patternV; ++i) {
        if (mapping[i] < 0 || mapping[i] >= V) return false;
        if (used.count(mapping[i])) return false;
        used.insert(mapping[i]);
    }
    
    for (int i = 0; i < patternV; ++i) {
        for (int j = i + 1; j < patternV; ++j) {
            if (other.hasEdge(i, j) != hasEdge(mapping[i], mapping[j])) {
                return false;
            }
        }
    }
    
    return true;
}

inline bool Graph::backtrackIsomorphism(const Graph& pattern,
                                       vector<int>& mapping,
                                       vector<bool>& used,
                                       int depth,
                                       vector<vector<int>>& allMappings) const {
    int patternV = pattern.getVertexCount();
    
    if (depth == patternV) {
        if (isIsomorphic(pattern, mapping)) {
            allMappings.push_back(mapping);
            return true;
        }
        return false;
    }
    
    bool found = false;
    
    for (int v = 0; v < V; ++v) {
        if (!used[v]) {
            if (pattern.getDegree(depth) > getDegree(v)) {
                continue;
            }
            
            mapping[depth] = v;
            bool compatible = true;
            
            for (int i = 0; i < depth; ++i) {
                if (pattern.hasEdge(i, depth) != hasEdge(mapping[i], v)) {
                    compatible = false;
                    break;
                }
            }
            
            if (compatible) {
                used[v] = true;
                found = backtrackIsomorphism(pattern, mapping, used, depth + 1, allMappings) || found;
                used[v] = false;
            }
            
            mapping[depth] = -1;
        }
    }
    
    return found;
}

inline vector<vector<int>> Graph::findIsomorphicSubgraph(const Graph& pattern) const {
    vector<vector<int>> allMappings;
    
    int patternV = pattern.getVertexCount();
    if (patternV > V) {
        return allMappings;
    }
    
    vector<int> mapping(patternV, -1);
    vector<bool> used(V, false);
    
    backtrackIsomorphism(pattern, mapping, used, 0, allMappings);
    
    return allMappings;
}

inline void Graph::printMatrix() const {
    cout << "\nМатрица смежности (" << V << "x" << V << "):\n   ";
    for (int i = 0; i < V; ++i) {
        cout << i << ' ';
    }
    cout << "\n";
    
    for (int i = 0; i < V; ++i) {
        cout << i << ":" << " ";
        for (int j = 0; j < V; ++j) {
            cout << (adjMatrix[i][j] ? "1" : "0") << ' ';
        }
        cout << "\n";
    }
}

inline void Graph::printInfo() const {
    cout << "Граф: " << V << " вершин, " << getEdgeCount() << " рёбер\n";
}

#endif