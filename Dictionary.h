#pragma once
#include <iostream>
#include <string>
#include <vector>


using namespace std;

class Node {
public:
    string keys[2];
    vector<string> values[2];
    Node* children[3];
    int numKeys;
    bool isLeaf;

    Node() {
        numKeys = 0;
        isLeaf = true;
        for (int i = 0; i < 3; i++) {
            children[i] = nullptr;
        }
    }
};

class Dictionary {
public:
    Node* root;

    Dictionary() {
        root = nullptr;
    }

    void insert(string key, string value) {
        if (root == nullptr) {
            root = new Node();
            root->keys[0] = key;
            root->values[0].push_back(value);
            root->numKeys = 1;
        }
        else {
            // Шаг 1: Найти лиcт, в который нужно добавить key
            Node* current = root;
            while (!current->isLeaf) {
                int i;
                for (i = 0; i < current->numKeys; i++) {
                    if (key < current->keys[i]) {
                        break;
                    }
                }
                current = current->children[i];
            }
            
            // Шаг 2: Проверить если key уже существует в листе
            for (int i = 0; i < current->numKeys; i++) {
                if (current->keys[i] == key) {
                    current->values[i].push_back(value);
                    return;
                }
            }

            // Шаг 3: Вставить новоую пару key-value в лист
            int i = current->numKeys - 1;
            while (i >= 0 && current->keys[i] > key) {
                current->keys[i + 1] = current->keys[i];
                current->values[i + 1] = current->values[i];
                i--;
            }
            current->keys[i + 1] = key;
            current->values[i + 1].push_back(value);
            current->numKeys++;
            cout << key << ": " << value << " " << current->numKeys << " " << current->values[current->numKeys - 1].back() << endl;

            // Шаг 4: Проверить если лист полный то необходимо его разбить
            if (current->numKeys == 2) {
                cout << "full" << endl;
                Node* parent = findParent(current);
                split(current, parent, root);
            }
        }
    }

    Node* search(string key) {
        // Step 1: Find the leaf node where the key should be located
        Node* current = root;
        while (!current->isLeaf) {
            int i;
            for (i = 0; i < current->numKeys; i++) {
                if (key < current->keys[i]) {
                    break;
                }
            }
            current = current->children[i];
        }

        // Step 2: Check if the key exists in the leaf node
        for (int i = 0; i < current->numKeys; i++) {
            if (current->keys[i] == key) {
                return current;
            }
        }

        // Step 3: If the key does not exist, return an empty vector
        return nullptr;
    }

    void deleteKey(Node* root, string key) {
        // Найти узел, содержащий удаляемый ключ
        int i = 0;
        while (i < root->numKeys && key > root->keys[i]) {
            i++;
        }
        if (root->keys[i] == key && root->isLeaf) {
            // Узел является листом, удалить ключ из него
            for (int j = i; j < root->numKeys - 1; j++) {
                root->keys[j] = root->keys[j + 1];
                root->values[j] = root->values[j + 1];
            }
            root->numKeys--;
            // Выполнить необходимые операции для поддержания свойств 2, 3-дерева
            fixDelete(root);
        }
        else if (!root->isLeaf) {
            // Найти предшественника или преемника удаляемого ключа
            Node* child = root->children[i];
            while (!child->isLeaf) {
                child = child->children[child->numKeys];
            }
            string pred = child->keys[child->numKeys - 1];
            // Заменить удаляемый ключ предшественником
            root->keys[i] = pred;
            root->values[i] = child->values[child->numKeys - 1];
            // Рекурсивно удалить предшественника из соответствующего поддерева
            deleteKey(child, pred);
        }
    }

private:
    void split(Node* node, Node* parent, Node*& root) {
        // Создать новый узел, содержащий вторую пару key-value
        Node* newNode = new Node();
        newNode->numKeys = 1;
        newNode->keys[0] = node->keys[1];
        newNode->values[0] = node->values[1];
        newNode->isLeaf = node->isLeaf;

        // Обновить оригинальный узел, чтобы он содержал только первую пару key-value
        node->numKeys = 1;

        // Если оригинальный узел не лист, переместить потомков в новый узел
        if (!node->isLeaf) {
            newNode->children[0] = node->children[1];
            newNode->children[1] = node->children[2];
            node->children[1] = nullptr;
            node->children[2] = nullptr;
        }

        // Если родитель null, создать новый корневой узел
        if (parent == nullptr) {
            cout << "parent is null" << endl;
            parent = new Node();
            root = parent;
        }

        // Вставить вторую пару key-value и нового потомка в родителя
        int i = parent->numKeys - 1;
        cout << i;
        while (i >= 0 && parent->keys[i] > node->keys[0]) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }
        parent->keys[i + 1] = node->keys[0];
        parent->children[i + 2] = newNode;
        parent->numKeys++;

        // Если родительский узел стал полный, разбить его рекурсивно
        if (parent->numKeys == 3) {
            Node* grandparent = nullptr;
            split(parent, grandparent, root);
        }
    }

    Node* findParent(Node* f_root) {
        if (root == nullptr) {
            return nullptr;
        }
        Node* parent = nullptr;
        Node* current = root;
        while (!current->isLeaf) {
            parent = current;
            current = current->children[current->numKeys];
        }
        return parent;
    }

    void fixDelete(Node* f_root) {
        if (f_root->numKeys < 1) {
            // Узел содержит менее одного ключа, удалить его
            if (f_root == f_root->children[0]) {
                // Узел является корнем, сделать его единственным потомком
                f_root = f_root->children[1];
            }
            else {
                // Найти брата узла и выполнить операции для поддержания свойств 2, 3-дерева
                Node* parent = findParent(f_root);
                int i = 0;
                while (i < parent->numKeys && parent->children[i] != f_root) {
                    i++;
                }
                if (i > 0 && parent->children[i - 1]->numKeys > 1) {
                    // Использовать ключ из левого брата
                    Node* leftSibling = parent->children[i - 1];
                    f_root->keys[0] = parent->keys[i - 1];
                    f_root->values[0] = parent->values[i - 1];
                    parent->keys[i - 1] = leftSibling->keys[leftSibling->numKeys - 1];
                    parent->values[i - 1] = leftSibling->values[leftSibling->numKeys - 1];
                    leftSibling->numKeys--;
                }
                else if (i < parent->numKeys && parent->children[i + 1]->numKeys > 1) {
                    // Использовать ключ из правого брата
                    Node* rightSibling = parent->children[i + 1];
                    f_root->keys[0] = parent->keys[i];
                    f_root->values[0] = parent->values[i];
                    parent->keys[i] = rightSibling->keys[0];
                    parent->values[i] = rightSibling->values[0];
                    for (int j = 0; j < rightSibling->numKeys - 1; j++) {
                        rightSibling->keys[j] = rightSibling->keys[j + 1];
                        rightSibling->values[j] = rightSibling->values[j + 1];
                    }
                    rightSibling->numKeys--;
                }
                else {
                    // Объединить с братом
                    if (i > 0) {
                        Node* leftSibling = parent->children[i - 1];
                        leftSibling->keys[leftSibling->numKeys] = parent->keys[i - 1];
                        leftSibling->values[leftSibling->numKeys] = parent->values[i - 1];
                        for (int j = 0; j < f_root->numKeys; j++) {
                            leftSibling->keys[leftSibling->numKeys + j + 1] = f_root->keys[j];
                            leftSibling->values[leftSibling->numKeys + j + 1] = f_root->values[j];
                        }
                        leftSibling->numKeys += f_root->numKeys + 1;
                        // Удалить узел из родительского узла
                        for (int j = i - 1; j < parent->numKeys - 1; j++) {
                            parent->keys[j] = parent->keys[j + 1];
                            parent->values[j] = parent->values[j + 1];
                            parent->children[j + 1] = parent->children[j + 2];
                        }
                        parent->numKeys--;
                        delete f_root;
                    }
                    else {
                        Node* rightSibling = parent->children[i + 1];
                        f_root->keys[f_root->numKeys] = parent->keys[i];
                        f_root->values[f_root->numKeys] = parent->values[i];
                        for (int j = 0; j < rightSibling->numKeys; j++) {
                            f_root->keys[f_root->numKeys + j + 1] = rightSibling->keys[j];
                            f_root->values[f_root->numKeys + j + 1] = rightSibling->values[j];
                        }
                        f_root->numKeys += rightSibling->numKeys + 1;
                        // Удалить узел из родительского узла
                        for (int j = i; j < parent->numKeys - 1; j++) {
                            parent->keys[j] = parent->keys[j + 1];
                            parent->values[j] = parent->values[j + 1];
                            parent->children[j + 1] = parent->children[j + 2];
                        }
                        parent->numKeys--;
                        delete rightSibling;
                    }
                    // Проверить свойства 2, 3-дерева для родительского узла
                    if (parent->numKeys < 1) {
                        fixDelete(parent);
                    }
                }
            }
        }
    }
};
