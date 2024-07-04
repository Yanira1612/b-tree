#include <iostream>
#include <vector>
#include <algorithm>

const int MAX_KEYS = 4;

class Node {
public:
    bool isLeaf;
    std::vector<int> keys;
    std::vector<Node*> children;
    Node* next;

    Node(bool leaf) : isLeaf(leaf), next(nullptr) {}

    void print_node() const {
        std::cout << (isLeaf ? "Leaf Node: " : "Internal Node: ");
        for (int key : keys) {
            std::cout << key << " ";
        }
        std::cout << "\n";
    }
};

class BPlusTree {
private:
    int maxKeys;
    Node* root;

    void insertInternal(int key, Node* cursor, Node* child) {
        std::cout << "Inserting key " << key << " in internal node with keys: ";
        for (int k : cursor->keys) {
            std::cout << k << " ";
        }
        std::cout << "\n";

        if (cursor->keys.size() < maxKeys) {
            // Insert the key in the current node
            auto it = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key);
            cursor->keys.insert(it, key);
            cursor->children.insert(cursor->children.begin() + (it - cursor->keys.begin()), child);

            std::cout << "Key " << key << " inserted into internal node.\n";
            cursor->print_node();
        } else {
            // Split the node
            std::cout << "Node is full, performing split.\n";
            std::vector<int> tempKeys = cursor->keys;
            std::vector<Node*> tempChildren = cursor->children;

            auto it = std::upper_bound(tempKeys.begin(), tempKeys.end(), key);
            tempKeys.insert(it, key);
            tempChildren.insert(tempChildren.begin() + (it - tempKeys.begin()), child);

            int midIndex = tempKeys.size() / 2;
            int midKey = tempKeys[midIndex];

            cursor->keys.resize(midIndex);
            cursor->children.resize(midIndex + 1);
            std::copy(tempKeys.begin(), tempKeys.begin() + midIndex, cursor->keys.begin());
            std::copy(tempChildren.begin(), tempChildren.begin() + midIndex + 1, cursor->children.begin());

            std::cout << "Split complete. New internal node:\n";
            cursor->print_node();

            Node* newInternal = new Node(false);
            newInternal->keys.assign(tempKeys.begin() + midIndex + 1, tempKeys.end());
            newInternal->children.assign(tempChildren.begin() + midIndex + 1, tempChildren.end());

            if (cursor == root) {
                std::cout << "Creating new root node.\n";
                Node* newRoot = new Node(false);
                newRoot->keys.push_back(midKey);
                newRoot->children.push_back(cursor);
                newRoot->children.push_back(newInternal);
                root = newRoot;

                std::cout << "New root node:\n";
                newRoot->print_node();
            } else {
                std::cout << "Inserting key " << midKey << " into parent node.\n";
                insertInternal(midKey, findParent(root, cursor), newInternal);
            }
        }
    }

    void insertLeaf(int key, Node* cursor) {
        Node* padre = new Node(true);
        padre=findParent(this->root,cursor);
   //     int pos=findPosition(padre,cursor);
        std::cout << "Inserting key " << key << " in leaf node with keys: ";
        for (int k : cursor->keys) {
            std::cout << k << " ";
        }
        std::cout << "\n";

        if (cursor->keys.size() < maxKeys) {
            // Insert the key in the leaf node
            auto it = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key);
            cursor->keys.insert(it, key);

            std::cout << "Key " << key << " inserted into leaf node.\n";
            cursor->print_node();
        } else {
            // Split the leaf node
            std::cout << "Leaf node is full, performing split.\n";
            std::vector<int> tempKeys = cursor->keys;
            auto it = std::upper_bound(tempKeys.begin(), tempKeys.end(), key);
            tempKeys.insert(it, key);//inserta en la posicion correcta a la nueva clave
            
            int midIndex = tempKeys.size() / 2;
            int midKey = tempKeys[midIndex];
            
            cursor->keys.resize(midIndex);
            std::copy(tempKeys.begin(), tempKeys.begin() + midIndex, cursor->keys.begin());
            this->display(getRoot());
            Node* newLeaf = new Node(true);
            newLeaf->keys.assign(tempKeys.begin() + midIndex, tempKeys.end());
            newLeaf->next = cursor->next;
            cursor->next = newLeaf;

            std::cout << "Split complete. New leaf node:\n";
            cursor->print_node();
            

            if (cursor == root) {
                std::cout << "Creating new root node.\n";
                Node* newRoot = new Node(false);
                newRoot->keys.push_back(midKey);
                newRoot->children.push_back(cursor);
                newRoot->children.push_back(newLeaf);
                root = newRoot;

                std::cout << "New root node:\n";
                newRoot->print_node();
            } else {
                
                std::cout << "Creating new root node.\n";
                Node* newRoot = new Node(false);
                newRoot->keys.push_back(midKey);
                newRoot->children.push_back(cursor);
                newRoot->children.push_back(newLeaf);
                padre->children[0]=newRoot;
              //  root->children[0]=newRoot;
              //  root->children.push_back(newRoot);

                std::cout << "New root node:\n";
                newRoot->print_node();
            }
        }
    }

    int findPosition(Node* parent, Node* child) {
    for (size_t i = 0; i < parent->children.size(); ++i) {
        if (parent->children[i] == child) {
            return i;
        }
    }
    return -1; // Nodo no encontrado
}


    Node* findParent(Node* root, Node* child) {
    if (root == nullptr || root->isLeaf) {
        return nullptr;
    }

    for (Node* node : root->children) {
        if (node == child) {
            return root;
        }
        Node* parent = findParent(node, child);
        if (parent != nullptr) {
            return parent;
        }
    }
    return nullptr;
}

/*
    Node* findParent(Node* cursor, Node* child) {
        if (cursor->isLeaf || cursor->children[0]->isLeaf) {
            return nullptr;
        }

        for (size_t i = 0; i < cursor->children.size(); ++i) {
            if (cursor->children[i] == child) {
                return cursor;
            } else {
                Node* parent = findParent(cursor->children[i], child);
                if (parent) {
                    return parent;
                }
            }
        }

        return nullptr;
    }*/

public:
    BPlusTree(int maxKeys) : maxKeys(maxKeys), root(new Node(true)) {}

    void insert(int key) {
        std::cout << "\nInserting key: " << key << "\n";
        Node* cursor = root;

        while (!cursor->isLeaf) {//busca donde debe insertarse
            auto it = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key);
            cursor = cursor->children[it - cursor->keys.begin()];
        }

        insertLeaf(key, cursor);
    }

    void display(Node* cursor, int level = 0) {
        if (cursor) {
            std::cout << "Level " << level << ": ";
            for (int key : cursor->keys) {
                std::cout << key << " ";
            }
            std::cout << "\n";

            for (Node* child : cursor->children) {
                display(child, level + 1);
            }
        }
    }

    Node* getRoot() {
        return root;
    }
};

int main() {
    BPlusTree bpt(4);

    bpt.insert(10);
    bpt.insert(27);
    bpt.insert(29);
    bpt.insert(17);
    bpt.insert(25);
    bpt.insert(21);
    bpt.insert(15);
    bpt.insert(31);
    bpt.insert(13); // Agregado para probar más inserciones

    std::cout << "\nFinal B+ Tree structure:\n";
    bpt.display(bpt.getRoot());

    return 0;
}
