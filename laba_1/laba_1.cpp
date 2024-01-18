#include <iostream>
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <sys/time.h>
#include <time.h>

using namespace std;

enum Color
{
    RED,
    BLACK
};

class Node
{
public:
    int key;
    Color color;
    Node *parent;
    Node *left;
    Node *right;

    Node(int key = 0) : key(key), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}

    ~Node() {}
};

class RBTree
{
public:
    Node *root;

    RBTree() : root(nullptr) {}

    ~RBTree() {}

    void rbtree_add(RBTree &T, int key)
    {
        Node *tree = T.root;
        Node *parent = nullptr;

        while (tree != nullptr)
        {
            parent = tree;
            if (key < tree->key)
            {
                tree = tree->left;
            }
            else if (key > tree->key)
            {
                tree = tree->right;
            }
            else
            {
                cout << "Key is already in the tree" << endl;
                return;
            }
        }

        Node *node = new Node(key);

        if (T.root == nullptr)
        {
            T.root = node;
            node->parent = nullptr;
        }
        else
        {
            if (key < parent->key)
            {
                parent->left = node;
            }
            else
            {
                parent->right = node;
            }

            node->parent = parent;
        }
        node->color = RED;
        rbtree_add_fixup(T, node);
    }

    void rbtree_add_fixup(RBTree &T, Node *z)
    {
        while (z->parent && z->parent->parent && z->parent->color == RED)
        {
            // Родитель z - левый потомок
            if (z->parent == z->parent->parent->left)
            {
                Node *y = z->parent->parent->right;
                if (y && y->color == RED)
                {
                    // Случай 1 - дядя красный
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else
                {
                    if (z == z->parent->right)
                    {
                        // Случай 2 - дядя чёрный, узел правый (переход к случаю 3)
                        z = z->parent;
                        T.left_rotate(z);
                    }
                    // Случай 3 - дядя чёрный, узел левый
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    T.right_rotate(z->parent->parent);
                }
            }
            // Родитель z - правый потомок
            else
            {
                Node *y = z->parent->parent->left;
                if (y && y->color == RED)
                {
                    // Случай 4 - симметричен случаю 1 (правое поддерево)
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else
                {
                    if (z == z->parent->left)
                    {
                        // Случай 5 - симметричен случаю 2 (правое поддерево) (переход к случаю 6)
                        z = z->parent;
                        T.right_rotate(z);
                    }
                    // Случай 6 - симметричен случаю 3 (правое поддерево)
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    T.left_rotate(z->parent->parent);
                }
            }
        }
        T.root->color = BLACK;
    }

    // Левое поддерево "пересаживается" бывшему родителю,
    // которое становится левым потомком
    void left_rotate(Node *x)
    {
        Node *y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
        {
            y->left->parent = x;
        }
        if (x->parent)
        {
            y->parent = x->parent;
            if (x == x->parent->left)
            {
                x->parent->left = y;
            }
            else
            {
                x->parent->right = y;
            }
            x->parent = y;
            y->left = x;
        }
        else
        {
            x->parent = y;
            root = y;
            root->left = x;
            root->parent = nullptr;
        }
    }

    // Правое поддерево "пересаживается" бывшему родителю,
    // которое становится правым потомком
    void right_rotate(Node *x)
    {
        Node *y = x->left;
        x->left = y->right;
        if (y->right != nullptr)
        {
            y->right->parent = x;
        }
        if (x->parent)
        {
            y->parent = x->parent;
            if (x == x->parent->left)
            {
                x->parent->left = y;
            }
            else
            {
                x->parent->right = y;
            }
            y->right = x;
            x->parent = y;
        }
        else
        {
            x->parent = y;
            root = y;
            root->right = x;
            root->parent = nullptr;
        }
    }

    Node *rbtree_min(Node *node)
    {
        if (!node)
        {
            return nullptr;
        }
        Node *tree = node;
        Node *parent = nullptr;
        while (tree)
        {
            parent = tree;
            tree = tree->left;
        }
        return parent;
    }

    Node *rbtree_max(Node *node)
    {
        if (!node)
        {
            return nullptr;
        }
        Node *tree = node;
        Node *parent = nullptr;
        while (tree)
        {
            parent = tree;
            tree = tree->right;
        }
        return parent;
    }

    Node *rbtree_lookup(RBTree &T, int key)
    {
        Node *tree = T.root;
        while (tree != nullptr)
        {
            if (key == tree->key)
            {
                return tree;
            }
            else if (key < tree->key)
            {
                tree = tree->left;
            }
            else
            {
                tree = tree->right;
            }
        }
        return nullptr;
    }

    Node *rbtree_delete(RBTree &T, int key)
    {
        Node *z = rbtree_lookup(T, key);
        if (z == nullptr)
        {
            cout << "Key not found in tree" << endl;
            return nullptr;
        }
        Node *y = z;
        Color y_original_color = y->color;
        Node *x;

        // z не имеет левого поддерева
        if (z->left == nullptr)
        {
            x = z->right;
            rbtree_transplant(T, z, z->right);
        }
        // z не имеет правого поддерева
        else if (z->right == nullptr)
        {
            x = z->left;
            rbtree_transplant(T, z, z->left);
        }
        // z имеет оба поддерева
        else
        {
            y = rbtree_min(z->right);
            y_original_color = y->color;
            x = y->right;

            // Если x=/= nullptr и y - родитель z, то устанавливается связь
            if (y->parent == z && x)
            {
                x->parent = y;
            }
            // Иначе замена на правого потомка
            else
            {
                rbtree_transplant(T, y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            rbtree_transplant(T, z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        // Восстановление свойств, если узел был чёрным
        if (y_original_color == BLACK && x)
        {
            rbtree_delete_fixup(T, x);
        }
        delete z;
        return x;
    }

    void rbtree_transplant(RBTree &T, Node *u, Node *v)
    {
        // Удаление корневого узла
        if (u->parent == nullptr)
        {
            T.root = v;
        }
        // Проверка, является узел левым или правым потомком
        else if (u == u->parent->left)
        {
            u->parent->left = v;
        }
        else
        {
            u->parent->right = v;
        }
        // Установление связи
        if (v != nullptr)
        {
            v->parent = u->parent;
        }
    }

    void rbtree_delete_fixup(RBTree &T, Node *x)
    {
        while (x != T.root && x->color == BLACK)
        {
            // x - левый потомок
            if (x == x->parent->left)
            {
                Node *w = x->parent->right; // Правый брат
                // Случай 1 - брат красный
                if (w->color == RED)
                {
                    w->color = BLACK;
                    x->parent->color = RED;
                    T.left_rotate(x->parent);
                    w = x->parent->right;
                }
                // Случай 2 - брат чёрный, оба его потомка чёрные
                if (w->left && w->right && w->left->color == BLACK && w->right->color == BLACK)
                {
                    w->color = RED;
                    x = x->parent;
                }
                else
                {
                    // Случай 3 - брат чёрный, его правый потомок чёрный, левый - красный
                    if (w->right->color == BLACK)
                    {
                        w->left->color = BLACK;
                        w->color = RED;
                        T.right_rotate(w);
                        w = x->parent->right;
                    }
                    // Случай 4 - брат чёрный, его правый потомок красный
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    T.left_rotate(x->parent);
                    x = T.root;
                }
            }
            // x - правый потомок
            // Случаи 5-8 зеркальны 1-4
            else
            {
                Node *w = x->parent->left; // Левый брат

                if (w->color == RED)
                {
                    w->color = BLACK;
                    x->parent->color = RED;
                    T.right_rotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK)
                {
                    w->color = RED;
                    x = x->parent;
                }
                else
                {
                    if (w->left && w->left->color == BLACK)
                    {
                        w->right->color = BLACK;
                        w->color = RED;
                        T.left_rotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    T.right_rotate(x->parent);
                    x = T.root;
                }
            }
        }
        x->color = BLACK;
    }

    int rbtree_height(Node *node)
    {
        if (node == nullptr)
        {
            return 0;
        }
        int l_height = rbtree_height(node->left);
        int r_height = rbtree_height(node->right);
        return 1 + max(l_height, r_height);
    }

    void rbtree_print_dfs(Node *node, int level, int step = 0)
    {
        if (node == nullptr || step > level)
        {
            return;
        }
        rbtree_print_dfs(node->right, level, step + 1);

        if (step <= level)
        {
            for (int i = 0; i < 4 * step; i++)
            {
                cout << "  ";
            }

            if (node->color == RED)
            {
                cout << node->key << "(R)" << endl;
            }
            else
            {
                cout << node->key << "(B)" << endl;
            }
        }
        rbtree_print_dfs(node->left, level, step + 1);
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
    srand(time(0));

    // Демонстрация логарифмической зависимости высоты дерева от количества узлов
    RBTree tree_test;
    int max_elem = 50001;

    ofstream file("test.txt");

    for (int i = 0; i < max_elem; i++)
    {
        tree_test.rbtree_add(tree_test, i);
        if (i % 5000 == 0)
        {
            double start = wtime();
            for (int j = 0; j < 100000; j++)
            {
                tree_test.rbtree_lookup(tree_test, i);
            }
            double end = wtime() - start;
            file << end * 1000 << endl;
        }
    }
    file.close();

    // Заполнение дерева возрастающей последовательностью
    RBTree tree_in;
    for (int i = 1; i <= 50000; i++)
    {
        tree_in.rbtree_add(tree_in, i);
    }
    // Демонстрация вывода дерева
    cout << "Red-Black Tree (increasing):" << endl;
    tree_in.rbtree_print_dfs(tree_in.root, 1);
    cout << "Red-Black Tree (increasing):" << endl;
    tree_in.rbtree_print_dfs(tree_in.root, 3);

    // Заполнение дерева убывающей последовательностью
    RBTree tree_dec;
    for (int i = 50000; i >= 1; i--)
    {
        tree_dec.rbtree_add(tree_dec, i);
    }
    // Демонстрация вывода дерева
    cout << "Red-Black Tree (deccreasing):" << endl;
    tree_dec.rbtree_print_dfs(tree_dec.root, 1);
    cout << "Red-Black Tree (deccreasing):" << endl;
    tree_dec.rbtree_print_dfs(tree_dec.root, 3);

    // Демонстрация удаления узлов
    RBTree tree;
    tree.rbtree_add(tree, 16);
    tree.rbtree_add(tree, 21);
    tree.rbtree_add(tree, 5);
    tree.rbtree_add(tree, 3);
    tree.rbtree_add(tree, 24);
    tree.rbtree_add(tree, 2);
    tree.rbtree_add(tree, 7);
    tree.rbtree_add(tree, 9);
    cout << "Red-Black Tree:" << endl;
    int delete_key = 3;
    tree.rbtree_print_dfs(tree.root, 3);
    tree.rbtree_delete(tree, delete_key);
    cout << "Red-Black Tree (delete 3):" << endl;
    tree.rbtree_print_dfs(tree.root, 3);
    delete_key = 16;
    tree.rbtree_delete(tree, delete_key);
    cout << "Red-Black Tree (delete 16):" << endl;
    tree.rbtree_print_dfs(tree.root, 3);
    delete_key = 7;
    tree.rbtree_delete(tree, delete_key);
    cout << "Red-Black Tree (delete 7):" << endl;
    tree.rbtree_print_dfs(tree.root, 3);

    return 0;
}