#include "Graph.h"
#include "Svg_save.h"
#include <iostream>

using namespace std;

void clearScreen();

void showMenu();

void manualInput();

void randomTests();

void timeMeasurement();

int main() {
    setlocale(LC_ALL, "Ru");
    
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
                break;
            default:
                cout << "Неверный выбор. Попробуйте снова.\n";
                cin.ignore();
                cin.get();
        }
    } while (choice != 4);
    
    return 0;
}

void clearScreen() {
    cout << "\n\n";
}

void showMenu() {
    cout << "Меню программы\n";
    cout << "1. Ручной ввод графов\n";
    cout << "2. Случайные тесты\n";
    cout << "3. Замер времени для разных размеров\n";
    cout << "4. Выход\n";
    cout << "Выберите действие: ";
}

void manualInput() {
    clearScreen();
    cout << "РУЧНОЙ ВВОД ГРАФОВ \n\n";
    
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
    
    mainGraph.printMatrix();
    mainGraph.printInfo();
    
    GraphVisualizer::saveGraphToSVG(mainGraph, "main_graph.svg");
    
    cout << "\n ВВОД ПАТТЕРНА (подграф для поиска) \n";
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
    
    pattern.printMatrix();
    pattern.printInfo();
    
    cout << "\n ПОИСК ИЗОМОРФНОГО ПОДГРАФА \n";
    auto start = chrono::high_resolution_clock::now();
    auto mappings = mainGraph.findIsomorphicSubgraph(pattern);
    auto end = chrono::high_resolution_clock::now();
    
    chrono::duration<double> duration = end - start;
    
    cout << "Время поиска: " << fixed << setprecision(6) 
         << duration.count() << " секунд\n";
    cout << "Найдено " << mappings.size() << " изоморфных подграфов\n\n";
    
    if (!mappings.empty()) {
        cout << "Сопоставления вершин (паттерн -> граф):\n";
        for (size_t i = 0; i < mappings.size(); ++i) {
            cout << i+1 << ": ";
            for (size_t j = 0; j < mappings[i].size(); ++j) {
                cout << j << " -> " << mappings[i][j];
                if (j < mappings[i].size() - 1) cout << ", ";
            }
            cout << "\n";
        }
        
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
    cout << "СЛУЧАЙНЫЕ ТЕСТЫ \n\n";
    
    int mainVertices, patternVertices;
    double edgeProbability;
    
    cout << "Введите параметры:\n";
    cout << "Количество вершин основного графа: ";
    cin >> mainVertices;
    cout << "Количество вершин подграфа: ";
    cin >> patternVertices;
    cout << "Вероятность ребра (0.0-1.0): ";
    cin >> edgeProbability;
    
    if (patternVertices > mainVertices) {
        cout << "Ошибка! Паттерн не может быть больше основного графа\n";
        return;
    }
    
    random_device rd;
    mt19937 gen(rd());
    
    Graph mainGraph;
    mainGraph.generateRandom(mainVertices, edgeProbability);
    
    Graph pattern;
    pattern.generateRandom(patternVertices, edgeProbability);
    
    cout << "\nСгенерированные графы:\n";
    cout << "Основной граф: ";
    mainGraph.printInfo();
    cout << "Подграф: ";
    pattern.printInfo();
    
    cout << "\nМатрица основного графа:\n";
    mainGraph.printMatrix();
    
    cout << "\nМатрица паттерна:\n";
    pattern.printMatrix();
    
    GraphVisualizer::saveGraphToSVG(mainGraph, "random_main.svg");
    
    cout << "\n ПОИСК ИЗОМОРФНОГО ПОДГРАФА \n";
    auto start = chrono::high_resolution_clock::now();
    auto mappings = mainGraph.findIsomorphicSubgraph(pattern);
    auto end = chrono::high_resolution_clock::now();
    
    chrono::duration<double> duration = end - start;
    
    cout << "Время поиска: " << fixed << setprecision(6) 
         << duration.count() << " секунд\n";
    cout << "Найдено " << mappings.size() << " изоморфных подграфов\n\n";
    
    if (!mappings.empty()) {
        cout << "Первое сопоставление вершин (подграф -> граф):\n";
        cout << "  ";
        for (size_t j = 0; j < mappings[0].size(); ++j) {
            cout << j << " -> " << mappings[0][j];
            if (j < mappings[0].size() - 1) cout << ", ";
        }
        cout << "\n";
        
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
    float probability = -1;
    clearScreen();  
    cout << "Тестирование времени поиска для разных размеров графов:\n";
    do {
    cout << "Введите вероятность соединения ребра и узла (от 0 до 1): ";
    
    while (!(cin >> probability)) {
        cout << "Ошибка! Введите число: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    if (probability > 1 || probability < 0) {
        cout << "Ошибка! Вероятность должна быть от 0 до 1.\n";
    }
    } while (probability > 1 || probability < 0);
    
    vector<pair<int, int>> testCases = {
        {6, 3},
        {8, 4},
        {10, 5},
        {12, 6},
        {15, 7},
    };
    
    cout << "Граф (верш)" << "     " << "Подграф (верш)" << "         "<<"Время (сек)"<< "        " << "Найдено" << "\n" << string(65, '-') << "\n";
    
    for (const auto& testCase : testCases) {
        int graphSize = testCase.first;
        int patternSize = testCase.second;
        
        Graph mainGraph;
        mainGraph.generateRandom(graphSize, probability);
        
        Graph pattern;
        pattern.generateRandom(patternSize, probability);
        
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
    
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}