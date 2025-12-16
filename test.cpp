#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <string>
#include <sstream>

using namespace std;

// ==================== КЛАСС ГРАФА ====================
class Graph {
private:
    int V; // Количество вершин
    vector<list<int>> adj; // Списки смежности
    vector<vector<bool>> adjMatrix; // Матрица смежности
    
    void buildAdjMatrix() {
        adjMatrix.clear();
        adjMatrix.resize(V, vector<bool>(V, false));
        
        for (int i = 0; i < V; ++i) {
            for (int neighbor : adj[i]) {
                adjMatrix[i][neighbor] = true;
                adjMatrix[neighbor][i] = true;
            }
        }
    }
    
    bool isIsomorphic(const Graph& other, const vector<int>& mapping) const {
        int patternV = other.getVertexCount();
        
        // Проверяем биективность
        unordered_set<int> used;
        for (int i = 0; i < patternV; ++i) {
            if (mapping[i] < 0 || mapping[i] >= V) return false;
            if (used.count(mapping[i])) return false;
            used.insert(mapping[i]);
        }
        
        // Проверяем сохранение рёбер
        for (int i = 0; i < patternV; ++i) {
            for (int j = i + 1; j < patternV; ++j) {
                if (other.hasEdge(i, j) != hasEdge(mapping[i], mapping[j])) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    bool backtrackIsomorphism(const Graph& pattern,
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
        
        // Пробуем сопоставить текущую вершину pattern
        for (int v = 0; v < V; ++v) {
            if (!used[v]) {
                // Проверка по степени
                if (pattern.getDegree(depth) > getDegree(v)) {
                    continue;
                }
                
                mapping[depth] = v;
                bool compatible = true;
                
                // Проверка частичного сопоставления
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
    
public:
    Graph() : V(0) {}
    
    Graph(int vertices) : V(vertices), adj(vertices) {
        if (vertices > 0) {
            buildAdjMatrix();
        }
    }
    
    Graph(const vector<pair<int, int>>& edges, int vertices) : V(vertices), adj(vertices) {
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
    
    void addEdge(int u, int v) {
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
    
    void removeEdge(int u, int v) {
        if (u >= 0 && u < V && v >= 0 && v < V) {
            adj[u].remove(v);
            adj[v].remove(u);
            adjMatrix[u][v] = false;
            adjMatrix[v][u] = false;
        }
    }
    
    bool hasEdge(int u, int v) const {
        if (u >= 0 && u < V && v >= 0 && v < V) {
            return adjMatrix[u][v];
        }
        return false;
    }
    
    int getDegree(int v) const {
        if (v >= 0 && v < V) {
            return adj[v].size();
        }
        return 0;
    }
    
    int getVertexCount() const { return V; }
    
    int getEdgeCount() const {
        int count = 0;
        for (int i = 0; i < V; ++i) {
            count += adj[i].size();
        }
        return count / 2;
    }
    
    void generateRandom(int vertices, double edgeProbability) {
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
    
    vector<vector<int>> findIsomorphicSubgraph(const Graph& pattern) const {
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
    
    void printMatrix() const {
        cout << "\nМатрица смежности (" << V << "x" << V << "):\n   ";
        for (int i = 0; i < V; ++i) {
            cout << setw(3) << i;
        }
        cout << "\n";
        
        for (int i = 0; i < V; ++i) {
            cout << setw(3) << i << ":";
            for (int j = 0; j < V; ++j) {
                cout << setw(3) << (adjMatrix[i][j] ? "1" : "0");
            }
            cout << "\n";
        }
    }
    
    void printInfo() const {
        cout << "Граф: " << V << " вершин, " << getEdgeCount() << " рёбер\n";
    }
    
    const vector<vector<bool>>& getAdjMatrix() const { return adjMatrix; }
    const vector<list<int>>& getAdjacencyList() const { return adj; }
};

// ==================== SVG ВИЗУАЛИЗАЦИЯ ====================
class GraphVisualizer {
private:
    struct Point {
        double x, y;
        Point(double _x = 0, double _y = 0) : x(_x), y(_y) {}
    };
    
    static Point getCirclePoint(int index, int total, double radius, double centerX, double centerY) {
        double angle = 2 * M_PI * index / total;
        return Point(centerX + radius * cos(angle), centerY + radius * sin(angle));
    }
    
public:
    static void saveGraphToSVG(const Graph& graph, const string& filename, int width = 600, int height = 300) {
        ofstream svg(filename);
        if (!svg.is_open()) {
            cerr << "Ошибка создания SVG файла\n";
            return;
        }
        
        int V = graph.getVertexCount();
        double radius = min(width, height) * 0.3;
        double centerX = width * 0.25;
        double centerY = height * 0.5;
        
        svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        svg << "<svg width=\"" << width << "\" height=\"" << height << "\" "
            << "xmlns=\"http://www.w3.org/2000/svg\">\n";
        
        // Рёбра
        svg << "  <!-- Рёбра графа -->\n";
        const auto& adjMatrix = graph.getAdjMatrix();
        for (int i = 0; i < V; ++i) {
            for (int j = i + 1; j < V; ++j) {
                if (adjMatrix[i][j]) {
                    Point p1 = getCirclePoint(i, V, radius, centerX, centerY);
                    Point p2 = getCirclePoint(j, V, radius, centerX, centerY);
                    svg << "  <line x1=\"" << p1.x << "\" y1=\"" << p1.y 
                        << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y 
                        << "\" stroke=\"blue\" stroke-width=\"2\"/>\n";
                }
            }
        }
        
        // Вершины
        svg << "  <!-- Вершины графа -->\n";
        for (int i = 0; i < V; ++i) {
            Point p = getCirclePoint(i, V, radius, centerX, centerY);
            svg << "  <circle cx=\"" << p.x << "\" cy=\"" << p.y 
                << "\" r=\"15\" fill=\"lightblue\" stroke=\"black\" stroke-width=\"2\"/>\n";
            svg << "  <text x=\"" << p.x << "\" y=\"" << p.y + 5 
                << "\" text-anchor=\"middle\" font-size=\"12\">" << i << "</text>\n";
        }
        
        // Заголовок
        svg << "  <text x=\"" << centerX << "\" y=\"20\" text-anchor=\"middle\" "
            << "font-size=\"16\" font-weight=\"bold\">Исходный граф</text>\n";
        svg << "  <text x=\"" << centerX << "\" y=\"40\" text-anchor=\"middle\" "
            << "font-size=\"14\">" << V << " вершин, " << graph.getEdgeCount() << " рёбер</text>\n";
        
        svg << "</svg>\n";
        svg.close();
        
        cout << "Граф сохранён в " << filename << "\n";
    }
    
    static void saveComparisonToSVG(const Graph& mainGraph, const Graph& pattern, 
                                   const vector<int>& mapping, const string& filename, 
                                   int width = 800, int height = 400) {
        ofstream svg(filename);
        if (!svg.is_open()) {
            cerr << "Ошибка создания SVG файла\n";
            return;
        }
        
        int mainV = mainGraph.getVertexCount();
        int patternV = pattern.getVertexCount();
        
        double radius = min(width/2, height) * 0.3;
        double leftCenterX = width * 0.25;
        double rightCenterX = width * 0.75;
        double centerY = height * 0.5;
        
        svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        svg << "<svg width=\"" << width << "\" height=\"" << height << "\" "
            << "xmlns=\"http://www.w3.org/2000/svg\">\n";
        
        // ===== ЛЕВАЯ ЧАСТЬ: ИСХОДНЫЙ ГРАФ =====
        // Рёбра исходного графа
        svg << "  <!-- Рёбра исходного графа -->\n";
        const auto& mainAdjMatrix = mainGraph.getAdjMatrix();
        for (int i = 0; i < mainV; ++i) {
            for (int j = i + 1; j < mainV; ++j) {
                if (mainAdjMatrix[i][j]) {
                    Point p1 = getCirclePoint(i, mainV, radius, leftCenterX, centerY);
                    Point p2 = getCirclePoint(j, mainV, radius, leftCenterX, centerY);
                    svg << "  <line x1=\"" << p1.x << "\" y1=\"" << p1.y 
                        << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y 
                        << "\" stroke=\"#CCCCCC\" stroke-width=\"1\" opacity=\"0.5\"/>\n";
                }
            }
        }
        
        // Подсветка найденного подграфа в основном графе
        svg << "  <!-- Выделенный изоморфный подграф -->\n";
        const auto& patternAdjMatrix = pattern.getAdjMatrix();
        for (int i = 0; i < patternV; ++i) {
            for (int j = i + 1; j < patternV; ++j) {
                if (patternAdjMatrix[i][j]) {
                    int u = mapping[i];
                    int v = mapping[j];
                    Point p1 = getCirclePoint(u, mainV, radius, leftCenterX, centerY);
                    Point p2 = getCirclePoint(v, mainV, radius, leftCenterX, centerY);
                    svg << "  <line x1=\"" << p1.x << "\" y1=\"" << p1.y 
                        << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y 
                        << "\" stroke=\"red\" stroke-width=\"3\"/>\n";
                }
            }
        }
        
        // Вершины исходного графа
        svg << "  <!-- Вершины исходного графа -->\n";
        for (int i = 0; i < mainV; ++i) {
            Point p = getCirclePoint(i, mainV, radius, leftCenterX, centerY);
            bool isInPattern = false;
            for (int j = 0; j < patternV; ++j) {
                if (mapping[j] == i) {
                    isInPattern = true;
                    break;
                }
            }
            
            string fillColor = isInPattern ? "red" : "lightblue";
            svg << "  <circle cx=\"" << p.x << "\" cy=\"" << p.y 
                << "\" r=\"15\" fill=\"" << fillColor 
                << "\" stroke=\"black\" stroke-width=\"2\"/>\n";
            svg << "  <text x=\"" << p.x << "\" y=\"" << p.y + 5 
                << "\" text-anchor=\"middle\" font-size=\"12\">" << i << "</text>\n";
        }
        
        // ===== ПРАВАЯ ЧАСТЬ: НАЙДЕННЫЙ ПОДГРАФ =====
        // Рёбра подграфа
        svg << "  <!-- Рёбра изоморфного подграфа -->\n";
        for (int i = 0; i < patternV; ++i) {
            for (int j = i + 1; j < patternV; ++j) {
                if (patternAdjMatrix[i][j]) {
                    Point p1 = getCirclePoint(i, patternV, radius, rightCenterX, centerY);
                    Point p2 = getCirclePoint(j, patternV, radius, rightCenterX, centerY);
                    svg << "  <line x1=\"" << p1.x << "\" y1=\"" << p1.y 
                        << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y 
                        << "\" stroke=\"red\" stroke-width=\"3\"/>\n";
                }
            }
        }
        
        // Вершины подграфа
        svg << "  <!-- Вершины изоморфного подграфа -->\n";
        for (int i = 0; i < patternV; ++i) {
            Point p = getCirclePoint(i, patternV, radius, rightCenterX, centerY);
            svg << "  <circle cx=\"" << p.x << "\" cy=\"" << p.y 
                << "\" r=\"15\" fill=\"red\" stroke=\"black\" stroke-width=\"2\"/>\n";
            svg << "  <text x=\"" << p.x << "\" y=\"" << p.y + 5 
                << "\" text-anchor=\"middle\" font-size=\"12\">" 
                << i << "→" << mapping[i] << "</text>\n";
        }
        
        // ===== ЗАГОЛОВКИ И СТРЕЛКА =====
        // Заголовки
        svg << "  <!-- Заголовки -->\n";
        svg << "  <text x=\"" << leftCenterX << "\" y=\"20\" text-anchor=\"middle\" "
            << "font-size=\"16\" font-weight=\"bold\">Исходный граф</text>\n";
        svg << "  <text x=\"" << leftCenterX << "\" y=\"40\" text-anchor=\"middle\" "
            << "font-size=\"14\">" << mainV << " вершин, " 
            << mainGraph.getEdgeCount() << " рёбер</text>\n";
        
        svg << "  <text x=\"" << rightCenterX << "\" y=\"20\" text-anchor=\"middle\" "
            << "font-size=\"16\" font-weight=\"bold\">Найденный подграф</text>\n";
        svg << "  <text x=\"" << rightCenterX << "\" y=\"40\" text-anchor=\"middle\" "
            << "font-size=\"14\">" << patternV << " вершин, " 
            << pattern.getEdgeCount() << " рёбер</text>\n";
        
        // Стрелка между графами
        svg << "  <!-- Стрелка -->\n";
        svg << "  <line x1=\"" << leftCenterX + radius + 20 << "\" y1=\"" << centerY 
            << "\" x2=\"" << rightCenterX - radius - 20 << "\" y2=\"" << centerY 
            << "\" stroke=\"black\" stroke-width=\"2\" marker-end=\"url(#arrow)\"/>\n";
        svg << "  <defs>\n";
        svg << "    <marker id=\"arrow\" markerWidth=\"10\" markerHeight=\"10\" "
            << "refX=\"9\" refY=\"3\" orient=\"auto\" markerUnits=\"strokeWidth\">\n";
        svg << "      <path d=\"M0,0 L0,6 L9,3 z\" fill=\"black\"/>\n";
        svg << "    </marker>\n";
        svg << "  </defs>\n";
        
        // Сопоставление вершин
        svg << "  <!-- Сопоставление вершин -->\n";
        svg << "  <text x=\"" << width/2 << "\" y=\"" << height - 20 
            << "\" text-anchor=\"middle\" font-size=\"14\">Сопоставление: ";
        for (int i = 0; i < patternV; ++i) {
            svg << i << "→" << mapping[i];
            if (i < patternV - 1) svg << ", ";
        }
        svg << "</text>\n";
        
        svg << "</svg>\n";
        svg.close();
        
        cout << "Сравнение сохранено в " << filename << "\n";
    }
};

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================
void clearScreen() {
    cout << "\n";
}

void showMenu() {
    cout << "1. Ручной ввод графов\n";
    cout << "2. Случайные тесты\n";
    cout << "3. Замер времени для разных размеров\n";
    cout << "4. Выход\n";
    cout << "Выберите действие: ";
}

void manualInput() {
    clearScreen();
    cout << "РУЧНОЙ ВВОД ГРАФОВ\n\n";
    
    // Ввод основного графа
    int mainVertices;
    cout << "Введите количество вершин основного графа: ";
    cin >> mainVertices;
    
    Graph mainGraph(mainVertices);
    cout << "\nВведите рёбра основного графа (формат: u v):\n";
    cout << "Для завершения ввода введите -1 -1\n";
    
    while (true) {
        int u, v;
        cin >> u >> v;
        
        if (u == -1 && v == -1) break;
        
        if (u < 0 || u >= mainVertices || v < 0 || v >= mainVertices) {
            cout << "Ошибка! Вершины должны быть от 0 до " << mainVertices-1 << "\n";
            continue;
        }
        
        mainGraph.addEdge(u, v);
        cout << "Добавлено ребро: " << u << " - " << v << "\n";
    }
    
    // Вывод матрицы основного графа
    mainGraph.printMatrix();
    mainGraph.printInfo();
    
    // Сохранение SVG основного графа
    GraphVisualizer::saveGraphToSVG(mainGraph, "main_graph.svg");
    
    // Ввод паттерна
    cout << "\nВВОД ПАТТЕРНА (подграф для поиска)\n";
    int patternVertices;
    cout << "Введите количество вершин паттерна: ";
    cin >> patternVertices;
    
    if (patternVertices > mainVertices) {
        cout << "Ошибка! Паттерн не может быть больше основного графа\n";
        return;
    }
    
    Graph pattern(patternVertices);
    cout << "\nВведите рёбра паттерна (формат: u v):\n";
    cout << "Для завершения ввода введите -1 -1\n";
    
    while (true) {
        int u, v;
        cin >> u >> v;
        
        if (u == -1 && v == -1) break;
        
        if (u < 0 || u >= patternVertices || v < 0 || v >= patternVertices) {
            cout << "Ошибка! Вершины должны быть от 0 до " << patternVertices-1 << "\n";
            continue;
        }
        
        pattern.addEdge(u, v);
        cout << "Добавлено ребро: " << u << " - " << v << "\n";
    }
    
    // Вывод матрицы паттерна
    pattern.printMatrix();
    pattern.printInfo();
    
    // Поиск изоморфного подграфа с замером времени
    cout << "\n=== ПОИСК ИЗОМОРФНОГО ПОДГРАФА ===\n";
    auto start = chrono::high_resolution_clock::now();
    auto mappings = mainGraph.findIsomorphicSubgraph(pattern);
    auto end = chrono::high_resolution_clock::now();
    
    chrono::duration<double> duration = end - start;
    
    cout << "Время поиска: " << fixed << setprecision(6) 
         << duration.count() << " секунд\n";
    cout << "Найдено " << mappings.size() << " изоморфных подграфов\n\n";
    
    if (!mappings.empty()) {
        cout << "Сопоставления вершин (паттерн→граф):\n";
        for (size_t i = 0; i < mappings.size(); ++i) {
            cout << i+1 << ": ";
            for (size_t j = 0; j < mappings[i].size(); ++j) {
                cout << j << "→" << mappings[i][j];
                if (j < mappings[i].size() - 1) cout << ", ";
            }
            cout << "\n";
        }
        
        // Сохранение сравнения в SVG
        GraphVisualizer::saveComparisonToSVG(mainGraph, pattern, mappings[0], 
                                           "comparison.svg");
        
        cout << "\nПервый найденный подграф сохранён в comparison.svg\n";
        cout << "Откройте файл в браузере для просмотра.\n";
    } else {
        cout << "Изоморфный подграф не найден.\n";
    }
    
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

void randomTests() {
    clearScreen();
    cout << "=== СЛУЧАЙНЫЕ ТЕСТЫ ===\n\n";
    
    int mainVertices, patternVertices;
    double edgeProbability;
    
    cout << "Введите параметры:\n";
    cout << "Количество вершин основного графа: ";
    cin >> mainVertices;
    cout << "Количество вершин паттерна: ";
    cin >> patternVertices;
    cout << "Вероятность ребра (0.0-1.0): ";
    cin >> edgeProbability;
    
    if (patternVertices > mainVertices) {
        cout << "Ошибка! Паттерн не может быть больше основного графа\n";
        return;
    }
    
    // Генерация случайных графов
    random_device rd;
    mt19937 gen(rd());
    
    Graph mainGraph;
    mainGraph.generateRandom(mainVertices, edgeProbability);
    
    Graph pattern;
    pattern.generateRandom(patternVertices, edgeProbability);
    
    // Вывод информации
    cout << "\nСгенерированные графы:\n";
    cout << "Основной граф: ";
    mainGraph.printInfo();
    cout << "Паттерн: ";
    pattern.printInfo();
    
    // Вывод матриц
    cout << "\nМатрица основного графа:\n";
    mainGraph.printMatrix();
    
    cout << "\nМатрица паттерна:\n";
    pattern.printMatrix();
    
    // Сохранение SVG основного графа
    GraphVisualizer::saveGraphToSVG(mainGraph, "random_main.svg");
    
    // Поиск изоморфного подграфа с замером времени
    cout << "\n=== ПОИСК ИЗОМОРФНОГО ПОДГРАФА ===\n";
    auto start = chrono::high_resolution_clock::now();
    auto mappings = mainGraph.findIsomorphicSubgraph(pattern);
    auto end = chrono::high_resolution_clock::now();
    
    chrono::duration<double> duration = end - start;
    
    cout << "Время поиска: " << fixed << setprecision(6) 
         << duration.count() << " секунд\n";
    cout << "Найдено " << mappings.size() << " изоморфных подграфов\n\n";
    
    if (!mappings.empty()) {
        cout << "Первое сопоставление вершин (паттерн→граф):\n";
        cout << "  ";
        for (size_t j = 0; j < mappings[0].size(); ++j) {
            cout << j << "→" << mappings[0][j];
            if (j < mappings[0].size() - 1) cout << ", ";
        }
        cout << "\n";
        
        // Сохранение сравнения в SVG
        GraphVisualizer::saveComparisonToSVG(mainGraph, pattern, mappings[0], 
                                           "random_comparison.svg");
        
        cout << "\nРезультат сохранён в random_comparison.svg\n";
        cout << "Откройте файл в браузере для просмотра.\n";
    } else {
        cout << "Изоморфный подграф не найден.\n";
    }
    
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

void timeMeasurement() {
    clearScreen();
    cout << "=== ЗАМЕР ВРЕМЕНИ ===\n\n";
    
    cout << "Тестирование времени поиска для разных размеров графов:\n";
    cout << "Вероятность ребра фиксирована на 0.3\n\n";
    
    vector<pair<int, int>> testCases = {
        {6, 3},   // Граф 6 вершин, паттерн 3 вершины
        {8, 4},   // Граф 8 вершин, паттерн 4 вершины
        {10, 5},  // Граф 10 вершин, паттерн 5 вершин
        {12, 6},  // Граф 12 вершин, паттерн 6 вершин
        {15, 7},  // Граф 15 вершин, паттерн 7 вершин
    };
    
    cout << setw(15) << "Граф (верш)" 
         << setw(15) << "Паттерн (верш)"
         << setw(20) << "Время (сек)"
         << setw(15) << "Найдено"
         << "\n" << string(65, '-') << "\n";
    
    for (const auto& testCase : testCases) {
        int graphSize = testCase.first;
        int patternSize = testCase.second;
        
        Graph mainGraph;
        mainGraph.generateRandom(graphSize, 0.3);
        
        Graph pattern;
        pattern.generateRandom(patternSize, 0.3);
        
        auto start = chrono::high_resolution_clock::now();
        auto mappings = mainGraph.findIsomorphicSubgraph(pattern);
        auto end = chrono::high_resolution_clock::now();
        
        chrono::duration<double> duration = end - start;
        
        cout << setw(15) << graphSize
             << setw(15) << patternSize
             << setw(20) << fixed << setprecision(6) << duration.count()
             << setw(15) << mappings.size()
             << "\n";
    }
    
    cout << "\nРекомендации:\n";
    cout << "- Для паттернов до 6 вершин время поиска обычно < 1 секунды\n";
    cout << "- Для паттернов 7-8 вершин время может быть от 1 до 10 секунд\n";
    cout << "- Для паттернов > 8 вершин время может быть очень большим\n";
    
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

// ==================== ГЛАВНАЯ ФУНКЦИЯ ====================
int main() {
    setlocale(LC_ALL, "Russian");
    
    int choice;
    
    do {
        clearScreen();
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                manualInput();
                break;
            case 2:
                randomTests();
                break;
            case 3:
                timeMeasurement();
                break;
            case 4:
                cout << "Выход из программы.\n";
                break;
            default:
                cout << "Неверный выбор. Попробуйте снова.\n";
                cin.ignore();
                cin.get();
        }
    } while (choice != 4);
    
    return 0;
}