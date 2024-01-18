#include <iostream>
#include <sys/time.h>
#include <ctime>

#define SIZE_LITTLE 100000
#define SIZE_BIG 1000000
#define STEP 100000

using namespace std;

enum Octs
{
    TOP_LEFT_FRONT = 0,
    TOP_RIGHT_FRONT = 1,
    BOTTOM_RIGHT_FRONT = 2,
    BOTTOM_LEFT_FRONT = 3,
    TOP_LEFT_BOTTOM = 4,
    TOP_RIGHT_BOTTOM = 5,
    BOTTOM_RIGHT_BOTTOM = 6,
    BOTTOM_LEFT_BOTTOM = 7,
};

class Point
{
public:
    int x;
    int y;
    int z;

    Point() : x(-1), y(-1), z(-1) {} // "Пустая" точка

    Point(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
};

class Octree
{
    Point *point;

    Point *top_left_front, *bottom_right_back; // Граничные точки
    Octree *children[8];                       // Хранение узлов

public:
    Octree() // Пустой узел
    {
        point = new Point();
        top_left_front = nullptr;
        bottom_right_back = nullptr;
        for (int i = 0; i < 8; ++i)
        {
            children[i] = nullptr;
        }
    }

    Octree(int x, int y, int z) // Узел с точкой
    {
        point = new Point(x, y, z);
        top_left_front = nullptr;
        bottom_right_back = nullptr;
        for (int i = 0; i < 8; ++i)
        {
            children[i] = nullptr;
        }
    }

    Octree(int x1, int y1, int z1, int x2, int y2, int z2) // Региональный узел
    {
        if ((x2 < x1) || (y2 < y1) || (z2 < z1))
        {
            return;
        }
        point = nullptr;
        top_left_front = new Point(x1, y1, z1);
        bottom_right_back = new Point(x2, y2, z2);
        for (int i = 0; i < 8; ++i)
        {
            children[i] = new Octree();
        }
    }

    // Очистка выделенной памяти
    ~Octree()
    {
        if (point)
        {
            delete point;
        }

        if (top_left_front)
        {
            delete top_left_front;
        }

        if (bottom_right_back)
        {
            delete bottom_right_back;
        }

        for (int i = 0; i < 8; ++i)
        {
            delete children[i];
        }
    }

    bool octree_add(int x, int y, int z)
    {
        if (!top_left_front || !bottom_right_back)
        {
            return false;
        }

        if (x < top_left_front->x || x > bottom_right_back->x || y < top_left_front->y || y > bottom_right_back->y || z < top_left_front->z || z > bottom_right_back->z)
        {
            return false; // Координаты точки за пределами зоны охвата дерева
        }

        int mid_x = (top_left_front->x + bottom_right_back->x) / 2;
        int mid_y = (top_left_front->y + bottom_right_back->y) / 2;
        int mid_z = (top_left_front->z + bottom_right_back->z) / 2;

        int index = -1;
        // Поиск нужного октанта
        if (x <= mid_x)
        {
            if (y <= mid_y)
            {
                if (z <= mid_z)
                {
                    index = TOP_LEFT_FRONT;
                }
                else
                {
                    index = TOP_LEFT_BOTTOM;
                }
            }
            else
            {
                if (z <= mid_z)
                {
                    index = BOTTOM_LEFT_FRONT;
                }
                else
                {
                    index = BOTTOM_LEFT_BOTTOM;
                }
            }
        }
        else
        {
            if (y <= mid_y)
            {
                if (z <= mid_z)
                {
                    index = TOP_RIGHT_FRONT;
                }
                else
                {
                    index = TOP_RIGHT_BOTTOM;
                }
            }
            else
            {
                if (z <= mid_z)
                {
                    index = BOTTOM_RIGHT_FRONT;
                }
                else
                {
                    index = BOTTOM_RIGHT_BOTTOM;
                }
            }
        }
        // Нелистовой узел, идём дальше
        if (children[index]->point == nullptr)
        {
            return children[index]->octree_add(x, y, z);
        } // Пустой узел в существующем октанте
        else if (children[index]->point->x == -1)
        {
            delete children[index];
            children[index] = new Octree(x, y, z);
            return true;
        } // Такой узел уже существует
        else if ((children[index]->point->x == x) && (children[index]->point->y == y) && (children[index]->point->z == z))
        {
            return false;
        } // Октанта нет, создаём
        else
        {
            int x_ = children[index]->point->x;
            int y_ = children[index]->point->y;
            int z_ = children[index]->point->z;
            delete children[index];
            children[index] = nullptr;

            if (index == TOP_LEFT_FRONT)
            {
                children[index] = new Octree(top_left_front->x, top_left_front->y, top_left_front->z,
                                             mid_x, mid_y, mid_z);
            }
            else if (index == TOP_RIGHT_FRONT)
            {
                children[index] = new Octree(mid_x + 1, top_left_front->y, top_left_front->z,
                                             bottom_right_back->x, mid_y, mid_z);
            }
            else if (index == BOTTOM_RIGHT_FRONT)
            {
                children[index] = new Octree(mid_x + 1, mid_y + 1, top_left_front->z,
                                             bottom_right_back->x, bottom_right_back->y, mid_z);
            }
            else if (index == BOTTOM_LEFT_FRONT)
            {
                children[index] = new Octree(top_left_front->x, mid_y + 1, top_left_front->z,
                                             mid_x, bottom_right_back->y, mid_z);
            }
            else if (index == TOP_LEFT_BOTTOM)
            {
                children[index] = new Octree(top_left_front->x, top_left_front->y, mid_z + 1,
                                             mid_x, mid_y, bottom_right_back->z);
            }
            else if (index == TOP_RIGHT_BOTTOM)
            {
                children[index] = new Octree(mid_x + 1, top_left_front->y, mid_z + 1,
                                             bottom_right_back->x, mid_y, bottom_right_back->z);
            }
            else if (index == BOTTOM_RIGHT_BOTTOM)
            {
                children[index] = new Octree(mid_x + 1, mid_y + 1, mid_z + 1,
                                             bottom_right_back->x, bottom_right_back->y, bottom_right_back->z);
            }
            else if (index == BOTTOM_LEFT_BOTTOM)
            {
                children[index] = new Octree(top_left_front->x, mid_y + 1, mid_z + 1,
                                             mid_x, bottom_right_back->y, bottom_right_back->z);
            }

            return children[index]->octree_add(x_, y_, z_) && children[index]->octree_add(x, y, z);
        }
    }

    bool octree_lookup(int x, int y, int z)
    {
        if (!top_left_front || !bottom_right_back)
        {
            return false;
        }

        if (x < top_left_front->x || x > bottom_right_back->x || y < top_left_front->y || y > bottom_right_back->y || z < top_left_front->z || z > bottom_right_back->z)
        {
            return false; // Координаты точки за пределами зоны охвата дерева
        }

        int mid_x = (top_left_front->x + bottom_right_back->x) / 2;
        int mid_y = (top_left_front->y + bottom_right_back->y) / 2;
        int mid_z = (top_left_front->z + bottom_right_back->z) / 2;

        int index = -1;
        // Поиск нужного октанта
        if (x <= mid_x)
        {
            if (y <= mid_y)
            {
                if (z <= mid_z)
                {
                    index = TOP_LEFT_FRONT;
                }
                else
                {
                    index = TOP_LEFT_BOTTOM;
                }
            }
            else
            {
                if (z <= mid_z)
                {
                    index = BOTTOM_LEFT_FRONT;
                }
                else
                {
                    index = BOTTOM_LEFT_BOTTOM;
                }
            }
        }
        else
        {
            if (y <= mid_y)
            {
                if (z <= mid_z)
                {
                    index = TOP_RIGHT_FRONT;
                }
                else
                {
                    index = TOP_RIGHT_BOTTOM;
                }
            }
            else
            {
                if (z <= mid_z)
                {
                    index = BOTTOM_RIGHT_FRONT;
                }
                else
                {
                    index = BOTTOM_RIGHT_BOTTOM;
                }
            }
        }
        // Нелистовой узел, идём дальше
        if (children[index]->point == nullptr)
        {
            return children[index]->octree_lookup(x, y, z);
        } // Узел пустой, без точки
        else if (children[index]->point->x == -1)
        {
            return false;
        } // Узел найден
        else
        {
            if (x == children[index]->point->x && y == children[index]->point->y && z == children[index]->point->z)
            {
                return true;
            }
        }
        return false; // Узел не найден в дереве
    }

    bool octree_remove(int x, int y, int z)
    {
        if (!top_left_front || !bottom_right_back)
        {
            return false;
        }

        if (x < top_left_front->x || x > bottom_right_back->x || y < top_left_front->y || y > bottom_right_back->y || z < top_left_front->z || z > bottom_right_back->z)
        {
            return false; // Координаты точки за пределами зоны охвата дерева
        }

        int mid_x = (top_left_front->x + bottom_right_back->x) / 2;
        int mid_y = (top_left_front->y + bottom_right_back->y) / 2;
        int mid_z = (top_left_front->z + bottom_right_back->z) / 2;

        int index = -1;
        // Поиск нужного октанта
        if (x <= mid_x)
        {
            if (y <= mid_y)
            {
                if (z <= mid_z)
                {
                    index = TOP_LEFT_FRONT;
                }
                else
                {
                    index = TOP_LEFT_BOTTOM;
                }
            }
            else
            {
                if (z <= mid_z)
                {
                    index = BOTTOM_LEFT_FRONT;
                }
                else
                {
                    index = BOTTOM_LEFT_BOTTOM;
                }
            }
        }
        else
        {
            if (y <= mid_y)
            {
                if (z <= mid_z)
                {
                    index = TOP_RIGHT_FRONT;
                }
                else
                {
                    index = TOP_RIGHT_BOTTOM;
                }
            }
            else
            {
                if (z <= mid_z)
                {
                    index = BOTTOM_RIGHT_FRONT;
                }
                else
                {
                    index = BOTTOM_RIGHT_BOTTOM;
                }
            }
        }
        // Нелистовой узел, идём дальше
        if (children[index]->point == nullptr)
        {
            return children[index]->octree_remove(x, y, z);
        } // Точка найдена
        else if (children[index]->point->x != -1 && x == children[index]->point->x && y == children[index]->point->y && z == children[index]->point->z)
        {
            delete children[index]->point;
            children[index]->point = nullptr;
            return true;
        }

        return false; // Точка не найдена
    }
};

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

int main()
{
    Octree tree(0, 0, 0, 10, 10, 10);

    cout << "min = 0, max = 10" << endl;
    cout << endl;

    cout << (tree.octree_add(9, 7, 4) ? "(9, 7, 4) Node has been added\n" : "(9, 7, 4) Node hasn't been added\n");
    cout << (tree.octree_add(2, 6, 1) ? "(2, 6, 1) Node has been added\n" : "(2, 6, 1) Node hasn't been added\n");
    cout << (tree.octree_add(4, 10, 3) ? "(4, 10, 3) Node has been added\n" : "(4, 10, 3) Node hasn't been added\n");
    cout << (tree.octree_add(6, 2, 3) ? "(6, 2, 3) Node has been added\n" : "(6, 2, 3) Node hasn't been added\n");
    cout << (tree.octree_add(4, 6, 8) ? "(4, 6, 8) Node has been added\n" : "(4, 6, 8) Node hasn't been added\n");
    cout << (tree.octree_add(2, 7, 6) ? "(2, 7, 6) Node has been added\n" : "(2, 7, 6) Node hasn't been added\n");
    cout << (tree.octree_add(6, 1, 9) ? "(6, 1, 9) Node has been added\n" : "(6, 1, 9) Node hasn't been added\n");
    cout << (tree.octree_add(5, 5, 1) ? "(5, 5, 1) Node has been added\n" : "(5, 5, 1) Node hasn't been added\n");
    cout << (tree.octree_add(1, 2, 9) ? "(1, 2, 9) Node has been added\n" : "(1, 2, 9) Node hasn't been added\n");
    cout << (tree.octree_add(4, 5, 11) ? "(4, 5, 11) Node has been added\n" : "(4, 5, 11) Node hasn't been added\n");
    cout << (tree.octree_add(1, 2, 9) ? "(1, 2, 9) Node has been added\n" : "(1, 2, 9) Node hasn't been added\n");
    cout << endl;

    cout << (tree.octree_lookup(9, 7, 4) ? "(9, 7, 4) Node found\n" : "(9, 7, 4) Node not found\n");
    cout << (tree.octree_lookup(4, 10, 3) ? "(4, 10, 3) Node found\n" : "(4, 10, 3) Node not found\n");
    cout << (tree.octree_lookup(5, 10, 3) ? "(5, 10, 3) Node found\n" : "(5, 10, 3) Node not found\n");
    cout << (tree.octree_lookup(5, 5, 1) ? "(5, 5, 1) Node found\n" : "(5, 5, 1) Node not found\n");
    cout << endl;

    cout << (tree.octree_remove(3, 3, 3) ? "(3, 3, 3) Node has been removed\n" : "(3, 3, 3) Node is not in the tree\n");
    cout << (tree.octree_remove(2, 7, 6) ? "(2, 7, 6) Node has been removed\n" : "(2, 7, 6) Node is not in the tree\n");
    cout << endl;

    double start, end;

    for (int i = SIZE_LITTLE; i <= SIZE_BIG; i += STEP)
    {
        Octree *octree = new Octree(0, 0, 0, SIZE_BIG, SIZE_BIG, SIZE_BIG);

        cout << i << endl;

        start = wtime();
        for (int j = 0; j < i; j++)
        {
            octree->octree_add(j, j, j);
        }
        end = (wtime() - start) * 1000;
        cout << "Add: " << end << endl;

        start = wtime();
        for (int j = 0; j < i; j++)
        {
            octree->octree_lookup(j, j, j);
        }
        end = (wtime() - start) * 1000;
        cout << "Lookup: " << end << endl;

        start = wtime();
        for (int j = i - 1; j >= 0; j--)
        {
            octree->octree_remove(j, j, j);
        }
        end = (wtime() - start) * 1000;
        cout << "Remove: " << end << endl
             << endl;

        delete octree;
    }

    return 0;
}