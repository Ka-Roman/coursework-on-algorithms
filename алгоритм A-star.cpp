#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <set>
#include <algorithm>
#include <limits>
#include <chrono>

using namespace std;

// Структура узла
struct Node {
    int x, y;
    double g, h, f;
    Node* parent;

    Node(int x, int y) : x(x), y(y), g(0), h(0), f(0), parent(nullptr) {}

    // Для приоритетной очереди (меньший f — выше приоритет)
    bool operator<(const Node& other) const {
        return f > other.f;
    }
};

// Размер сетки
int ROWS, COLS;

// Сетка: 0 — свободно, 1 — препятствие
vector<vector<int>> grid;

// Эвристика
double heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);  // Манхэттенское
}

// Проверка, находится ли точка в сетке
bool isValid(int x, int y) {
    return x >= 0 && x < ROWS && y >= 0 && y < COLS;
}

// Ввод сетки с консоли
void inputGrid() {
    cout << "Введите количество строк сетки: ";
    cin >> ROWS;

    cout << "Введите количество столбцов сетки: ";
    cin >> COLS;

    // Изменяем размер сетки
    grid.resize(ROWS, vector<int>(COLS));

    cout << "\nВведите сетку (" << ROWS << "x" << COLS << "):\n";
    cout << "0 - свободная клетка, 1 - препятствие\n\n";

    for (int i = 0; i < ROWS; ++i) {
        cout << "Строка " << i + 1 << " (введите " << COLS << " чисел через пробел): ";
        for (int j = 0; j < COLS; ++j) {
            cin >> grid[i][j];
            // Проверка на допустимые значения
            if (grid[i][j] != 0 && grid[i][j] != 1) {
                cout << "Ошибка: допустимы только значения 0 или 1. Использую 0.\n";
                grid[i][j] = 0;
            }
        }
    }

    // Вывод введенной сетки для проверки
    cout << "\nВведенная сетка:\n";
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
}

// Генерация сетки
void genarationInputGrid(int obstacle_coefficient, int mesh_size) {

    ROWS = mesh_size;
    COLS = mesh_size;

    // Изменяем размер сетки и вспомогательных структур
    grid.resize(ROWS, vector<int>(COLS));

    int start = 1;
    int end = 100;
    int random_number;
    int probability = obstacle_coefficient;

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            random_number = rand() % (end - start + 1) + start;
            if (random_number <= probability) {
                grid[i][j] = 1;
            }
            else {
                grid[i][j] = 0;
            }

            // Проверка на допустимые значения
            if (grid[i][j] != 0 && grid[i][j] != 1) {
                grid[i][j] = 0;
            }
        }
    }
}

// Ввод координат
bool inputCoordinates(int& x, int& y, const string& pointName) {
    cout << "\nВведите координаты " << pointName << ":\n";
    cout << "Строка (от 0 до " << ROWS - 1 << "): ";
    cin >> x;
    cout << "Столбец (от 0 до " << COLS - 1 << "): ";
    cin >> y;

    // Проверка валидности координат
    if (!isValid(x, y)) {
        cout << "Ошибка: координаты вне диапазона сетки!\n";
        return false;
    }

    // Проверка, что точка не на препятствии
    if (grid[x][y] == 1) {
        cout << "Ошибка: точка находится на препятствии!\n";
        return false;
    }

    return true;
}

// Генерация тестов
void testGeneration(int& startX, int& startY, int& endX, int& endY, int obstacle_coefficient, int mesh_size) {
    genarationInputGrid(obstacle_coefficient, mesh_size);//нужно вводить с квлавиатуры
    startX = 0, startY = 0;
    endX = ROWS - 1, endY = COLS - 1;
}

// Поиск пути
vector<pair<int, int>> aStar(int startX, int startY, int endX, int endY) {
    // Начальная и конечная точки
    Node* start = new Node(startX, startY);
    Node* end = new Node(endX, endY);
    start->h = heuristic(startX, startY, endX, endY);
    start->f = start->g + start->h;

    // Открытое множество (приоритетная очередь)
    // Компаратор для приоритетной очереди
    auto cmp = [](Node* a, Node* b) { return a->f > b->f; };
    priority_queue<Node*, vector<Node*>, decltype(cmp)> openSet(cmp);
    openSet.push(start);

    // Закрытое множество (уже обработанные узлы)
    set<pair<int, int>> closedSet;

    // Все созданные узлы (чтобы не потерять память)
    vector<Node*> allNodes;
    allNodes.push_back(start);

    // Для отслеживания лучших g-значений для каждой ячейки
    vector<vector<double>> bestG(ROWS, vector<double>(COLS, numeric_limits<double>::max()));
    bestG[startX][startY] = 0;

    while (!openSet.empty()) {
        // Берём узел с наименьшим f
        Node* current = openSet.top();
        openSet.pop();

        // Если текущий узел уже в закрытом множестве с лучшим g, пропускаем
        if (current->g > bestG[current->x][current->y]) {
            continue;
        }

        // Если достигли цели
        if (current->x == end->x && current->y == end->y) {
            // Восстанавливаем путь
            vector<pair<int, int>> path;
            Node* pathNode = current;
            while (pathNode != nullptr) {
                path.push_back({ pathNode->x, pathNode->y });
                pathNode = pathNode->parent;
            }
            // Разворачиваем путь (от старта к финишу)
            reverse(path.begin(), path.end());

            return path;
        }

        // Добавляем в закрытое множество
        closedSet.insert({ current->x, current->y });

        // Проверяем соседей (4 направления)
        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };

        for (int i = 0; i < 4; ++i) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];

            // Проверка границ и препятствий
            if (!isValid(nx, ny) || grid[nx][ny] == 1) {
                continue;
            }

            // Рассчитываем новую стоимость g
            double newG = current->g + 1.0;  // Стоимость шага = 1

            // Если новая стоимость лучше предыдущей
            if (newG < bestG[nx][ny]) {
                bestG[nx][ny] = newG;

                Node* neighbor = new Node(nx, ny);
                neighbor->g = newG;
                neighbor->h = heuristic(nx, ny, end->x, end->y);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = current;

                openSet.push(neighbor);
                allNodes.push_back(neighbor);
            }
        }
    }

    // Путь не найден
    for (Node* node : allNodes) delete node;
    return {};
}

// Отображение пути на сетке
void displayPath(const vector<pair<int, int>>& path) {
    cout << "\nПуть на сетке:\n";

    // Создаем копию сетки для отображения
    vector<vector<char>> displayGrid(ROWS, vector<char>(COLS));

    // Заполняем отображаемую сетку
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (grid[i][j] == 1) {
                displayGrid[i][j] = '#';  // Препятствие
            }
            else {
                displayGrid[i][j] = '.';  // Свободная клетка
            }
        }
    }

    // Отмечаем путь
    for (size_t i = 0; i < path.size(); ++i) {
        int x = path[i].first;
        int y = path[i].second;

        if (i == 0) {
            displayGrid[x][y] = 'S';  // Старт
        }
        else if (i == path.size() - 1) {
            displayGrid[x][y] = 'E';  // Финиш
        }
        else {
            displayGrid[x][y] = '*';  // Путь
        }
    }

    // Выводим сетку с путем
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            cout << displayGrid[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    setlocale(0, "");
    srand(time(0));

    cout << "=== Алгоритм A* для поиска пути ===\n";
    cout << "Выберите вариант запуска программы:\n";
    cout << "0 - Ручной ввод данных\n";
    cout << "1 - Автоматический ввод данных(тесты)\n";

    int option;
    cin >> option;

    int startX, startY;
    int endX, endY;

    if (option == 1)
    {
        int mesh_size;
        int obstacle_coefficient;
        int count_tests;
        cout << "Размер сетки:\n";
        cin >> mesh_size;
        cout << "Процент препятствий (от 0 до 100):\n";
        cin >> obstacle_coefficient;
        cout << "Количество тестов:\n";
        cin >> count_tests;
        int completed_test = 0;
        int failed_test = 0;
        auto sum_duration = 0;
        auto max_duration = 0;
        auto min_duration = 99999999;

        while (count_tests) {
            cout << "\n" << count_tests << " ";
            testGeneration(startX, startY, endX, endY, obstacle_coefficient, mesh_size);

            auto start_time = std::chrono::high_resolution_clock::now(); // Начало замера времени

            // Поиск пути
            vector<pair<int, int>> path = aStar(startX, startY, endX, endY);
            auto end_time = std::chrono::high_resolution_clock::now(); // Конец замера времени
            if (empty(path)) {
                cout << "Тест провален";
                failed_test++;
            }
            else {
                completed_test++;

                // Отображение пути на сетке
                //displayPath(path);

                auto duration = end_time - start_time; //Вычисляем разницу
                sum_duration += chrono::duration_cast<chrono::microseconds>(duration).count();
                if (max_duration < chrono::duration_cast<chrono::microseconds>(duration).count())
                {
                    max_duration = chrono::duration_cast<chrono::microseconds>(duration).count();
                }
                if (min_duration > chrono::duration_cast<chrono::microseconds>(duration).count())
                {
                    min_duration = chrono::duration_cast<chrono::microseconds>(duration).count();
                }
                cout << "Время выполнения: " << chrono::duration_cast<chrono::microseconds>(duration).count() << " мкс";
            }
            count_tests--;
        }
        cout << "\nУспешно выполненых тестов: " << completed_test << endl;
        cout << "Проваленных тестов: " << failed_test << endl;
        cout << "Среднее время теста: " << sum_duration / completed_test << endl;
        cout << "Максимальное время теста: " << max_duration << endl;
        cout << "Минимальное время теста: " << min_duration << endl;
    }
    else {
        // Ввод сетки
        inputGrid();

        // Ввод начальной точки
        int startX, startY;
        while (!inputCoordinates(startX, startY, "старта")) {
            // Повторяем ввод, пока не будут введены корректные координаты
        }

        // Ввод конечной точки
        int endX, endY;
        while (!inputCoordinates(endX, endY, "конечной точки")) {
            // Повторяем ввод, пока не будут введены корректные координаты
        }

        auto start_time = std::chrono::high_resolution_clock::now(); // Начало замера времени

        // Проверка, что начальная и конечная точки разные
        if (startX == endX && startY == endY) {
            cout << "Ошибка: начальная и конечная точки совпадают!\n";
            return 1;
        }

        cout << "\nПоиск пути от (" << startX << ", " << startY
            << ") до (" << endX << ", " << endY << ")...\n";

        // Поиск пути
        vector<pair<int, int>> path = aStar(startX, startY, endX, endY);

        auto end_time = std::chrono::high_resolution_clock::now(); // Конец замера времени

        // Вывод результата
        if (path.empty()) {
            cout << "\nПуть не найден!\n";
        }
        else {
            cout << "\nНайденный путь (длина: " << path.size() - 1 << " шагов):\n";
            for (size_t i = 0; i < path.size(); ++i) {
                cout << "(" << path[i].first << ", " << path[i].second << ")";
                if (i < path.size() - 1) {
                    cout << " -> ";
                    if ((i + 1) % 5 == 0) cout << "\n";
                }
            }
            cout << endl;

            // Отображение пути на сетке
            displayPath(path);
        }

        auto duration = end_time - start_time; //Вычисляем разницу
        cout << "Время выполнения (наносекунды): " << chrono::duration_cast<chrono::nanoseconds>(duration).count() << " нс" << endl;
    }
    return 0;
}