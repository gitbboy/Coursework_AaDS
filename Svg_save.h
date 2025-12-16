#ifndef SVG_SAVE_H
#define SVG_SAVE_H

#include "Graph.h"
#include <fstream>
#include <cmath>

using namespace std;

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
                        << "\" stroke=\"black\" stroke-width=\"3\" stroke-opacity=\"1.0\"/>\n";
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
                << "\" text-anchor=\"middle\" font-size=\"12\" font-weight=\"bold\">" << i << "</text>\n";
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
        
        // ИСХОДНЫЙ ГРАФ
        svg << "  <!-- Рёбра исходного графа -->\n";
        const auto& mainAdjMatrix = mainGraph.getAdjMatrix();
        for (int i = 0; i < mainV; ++i) {
            for (int j = i + 1; j < mainV; ++j) {
                if (mainAdjMatrix[i][j]) {
                    Point p1 = getCirclePoint(i, mainV, radius, leftCenterX, centerY);
                    Point p2 = getCirclePoint(j, mainV, radius, leftCenterX, centerY);
                    svg << "  <line x1=\"" << p1.x << "\" y1=\"" << p1.y 
                        << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y 
                        << "\" stroke=\"black\" stroke-width=\"2\" stroke-opacity=\"1.0\"/>\n";
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
                        << "\" stroke=\"red\" stroke-width=\"4\" stroke-opacity=\"1.0\"/>\n";
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
                << "\" text-anchor=\"middle\" font-size=\"12\" font-weight=\"bold\">" << i << "</text>\n";
        }
        
        // НАЙДЕННЫЙ ПОДГРАФ
        // Рёбра подграфа
        svg << "  <!-- Рёбра изоморфного подграфа -->\n";
        for (int i = 0; i < patternV; ++i) {
            for (int j = i + 1; j < patternV; ++j) {
                if (patternAdjMatrix[i][j]) {
                    Point p1 = getCirclePoint(i, patternV, radius, rightCenterX, centerY);
                    Point p2 = getCirclePoint(j, patternV, radius, rightCenterX, centerY);
                    svg << "  <line x1=\"" << p1.x << "\" y1=\"" << p1.y 
                        << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y 
                        << "\" stroke=\"red\" stroke-width=\"4\" stroke-opacity=\"1.0\"/>\n";
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
                << "\" text-anchor=\"middle\" font-size=\"12\" font-weight=\"bold\">" 
                << i << "</text>\n";
        }
        
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

#endif