#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <chrono>

using namespace std;

// Размер сетки
int ROWS, COLS;

// Сетка: 0 — свободно, 1 — препятствие
vector<vector<int>> grid;

// Для отслеживания посещенных клеток
vector<vector<bool>> visited;

// Для хранения родительских клеток (для восстановления пути)
vector<vector<pair<int, int>>> parent;

// Направления движения (4-связность)
int dx[] = { -1, 1, 0, 0 };
int dy[] = { 0, 0, -1, 1 };

// Проверка, находится ли точка в сетке и доступна ли она
bool isValid(int x, int y) {
    return x >= 0 && x < ROWS && y >= 0 && y < COLS && grid[x][y] == 0;
}

// Ввод сетки с консоли
void inputGrid() {
    cout << "Введите количество строк сетки: ";
    cin >> ROWS;

    cout << "Введите количество столбцов сетки: ";
    cin >> COLS;

    // Изменяем размер сетки и вспомогательных структур
    grid.resize(ROWS, vector<int>(COLS));
    visited.resize(ROWS, vector<bool>(COLS, false));
    parent.resize(ROWS, vector<pair<int, int>>(COLS, { -1, -1 }));

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
    visited.resize(ROWS, vector<bool>(COLS, false));
    parent.resize(ROWS, vector<pair<int, int>>(COLS, { -1, -1 }));

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
        cout << "Ошибка: координаты вне диапазона сетки или на препятствии!\n";
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

// Алгоритм поиска в ширину (BFS)
vector<pair<int, int>> bfs(int startX, int startY, int endX, int endY) {
    // Очищаем структуры данных
    for (int i = 0; i < ROWS; ++i) {
        fill(visited[i].begin(), visited[i].end(), false);
        fill(parent[i].begin(), parent[i].end(), make_pair(-1, -1));
    }

    // Очередь для BFS
    queue<pair<int, int>> q;

    // Начинаем со стартовой точки
    q.push({ startX, startY });
    visited[startX][startY] = true;

    // Массив для хранения расстояний 
    vector<vector<int>> distance(ROWS, vector<int>(COLS, -1));
    distance[startX][startY] = 0;

    bool found = false;

    while (!q.empty() && !found) {
        auto current = q.front();
        q.pop();
        int x = current.first;
        int y = current.second;

        // Если достигли цели
        if (x == endX && y == endY) {
            found = true;
            break;
        }

        // Проверяем всех соседей
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (isValid(nx, ny) && !visited[nx][ny]) {
                visited[nx][ny] = true;
                parent[nx][ny] = { x, y };
                distance[nx][ny] = distance[x][y] + 1;
                q.push({ nx, ny });

                // Если нашли цель, можно выйти
                if (nx == endX && ny == endY) {
                    found = true;
                }
            }
        }
    }

    // Восстанавливаем путь, если он был найден
    vector<pair<int, int>> path;
    if (found) {
        // Идем от конца к началу по родителям
        int x = endX, y = endY;
        stack<pair<int, int>> tempPath;

        while (x != -1 && y != -1) {
            tempPath.push({ x, y });
            auto p = parent[x][y];
            x = p.first;
            y = p.second;
        }

        // Переворачиваем путь
        while (!tempPath.empty()) {
            path.push_back(tempPath.top());
            tempPath.pop();
        }
    }

    return path;
}

// Отображение пути на сетке
void displayPath(const vector<pair<int, int>>& path, bool showDistances = false) {
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

    // Показываем посещенные клетки (опционально)
    if (showDistances) {
        cout << "\nПосещенные клетки (0 - не посещена, 1 - посещена):\n";
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                cout << (visited[i][j] ? "1" : "0") << " ";
            }
            cout << endl;
        }
    }
}

// Поиск всех кратчайших путей (BFS находит кратчайший путь)
vector<pair<int, int>> findShortestPathInfo(int startX, int startY, int endX, int endY) {
    vector<pair<int, int>> path = bfs(startX, startY, endX, endY);

    if (path.empty()) {
        /*
        cout << "\nПуть не найден!\n";
        */
    }
    else {
        /*
        int pathLength = path.size() - 1; // Количество шагов
        cout << "\nКратчайший путь найден!\n";
        cout << "Длина пути: " << pathLength << " шагов\n";

        cout << "\nПуть:\n";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << "(" << path[i].first << ", " << path[i].second << ")";
            if (i < path.size() - 1) {
                cout << " -> ";
                if ((i + 1) % 5 == 0) cout << "\n";
            }
        }
        cout << endl;
        */

        return path;
    }
}



int main() {
    setlocale(0, "");
    srand(time(0));

    cout << "=== Алгоритм поиска в ширину (BFS) для поиска пути ===\n";
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

            //cout << "\nПоиск кратчайшего пути от (" << startX << ", " << startY << ") до (" << endX << ", " << endY << ")...\n";

            // Поиск пути с помощью BFS
            vector<pair<int, int>> path = bfs(startX, startY, endX, endY);
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
        cout << "\nУспешно выполненых тестов:" << completed_test << endl;
        cout << "Проваленных тестов:" << failed_test << endl;
        cout << "Среднее время теста:" << sum_duration / completed_test << endl;
        cout << "Максимальное время теста:" << max_duration << endl;
        cout << "Минимальное время теста:" << min_duration << endl;
    }
    else {
        // Ввод сетки
        inputGrid();

        // Ввод начальной точки
        while (!inputCoordinates(startX, startY, "старта")) {
            // Повторяем ввод, пока не будут введены корректные координаты
        }

        // Ввод конечной точки
        while (!inputCoordinates(endX, endY, "конечной точки")) {
            // Повторяем ввод, пока не будут введены корректные координаты
        }

        // Проверка, что начальная и конечная точки разные
        if (startX == endX && startY == endY) {
            cout << "Ошибка: начальная и конечная точки совпадают!\n";
            return 1;
        }

        auto start_time = std::chrono::high_resolution_clock::now(); // Начало замера времени

        cout << "\nПоиск кратчайшего пути от (" << startX << ", " << startY << ") до (" << endX << ", " << endY << ")...\n";

        // Поиск пути с помощью BFS
        vector<pair<int, int>> path = findShortestPathInfo(startX, startY, endX, endY);
        if (empty(path)) {
            return 0;
        }

        auto end_time = std::chrono::high_resolution_clock::now(); // Конец замера времени

        // Отображение пути на сетке
        displayPath(path);

        auto duration = end_time - start_time; //Вычисляем разницу
        cout << "Время выполнения (наносекунды): " << chrono::duration_cast<chrono::nanoseconds>(duration).count() << " нс" << endl;
    }

    return 0;
}
