#ifndef AVL_H
#define AVL_H

#include "AVLInterface.h"
#include <sstream>

/** Represents a difference in heights on left or right of a node */
enum balanceType { LEFT_HEAVY = -1, BALANCED = 0, RIGHT_HEAVY = 1 };

//The Iterator Messes Up after a Removal
template <typename T>
class AVL : AVLInterface<T> {
private:
    struct Node {
        Node() : parent(NULL), left(NULL), right(NULL), balance(BALANCED) {}
        Node(T data = T(), Node* parent = NULL, Node* left = NULL, Node* right = NULL) : data(data), parent(parent), left(left), right(right), balance(BALANCED) {}
        ~Node() {
            parent = NULL;
            left = NULL;
            right = NULL;
        }
        T data;
        Node* parent;
        Node* left;
        Node* right;
        balanceType balance;
    };
    Node* root;
    size_t treeSize;
    bool increase;
    bool decrease;
    
    
    /** Add node to tree recursively */
    bool insert(AVL<T>::Node*& current, AVL<T>::Node*& parent, const T& item);
    
    /** Output nodes at a given level */
    bool outLevel(Node* root, int level, std::stringstream& os) const;
    
    /** Recursive function to clear tree */
    void clearTree(Node*& current) {
        if (current == NULL) return;
        clearTree(current->left);
        clearTree(current->right);
        delete current;
        return;
    }
    
    /** Recursive function to find a data node in the tree*/
    std::string find(Node* node, T& data) {
        if (node == NULL) { return "not found"; }
        else if (data < node->data) { return find(node->left, data); }
        else if (data > node->data) { return find(node->right, data); }
        else { return "found"; }
    }
    
    /** Recursive function to erase a node from tree */
    bool erase(const T& data, Node*& current);
    
    /** Called by 'erase' to remove a node with two children */
    void replaceParent(Node*& oldRoot, Node*& localRoot);
    
    /** Rotates a branch to the left */
    bool rotateLeft(Node*& root);
    
    /** Rotates a branch to the right */
    bool rotateRight(Node*& root);
    
    /** Rebalances a left branch of the tree */
    void rebalanceLeft(Node*& root, char c = 'i');
    
    /** Rebalances a right branch of the tree */
    void rebalanceRight(Node*& root, char c = 'i');
    
public:
    
    AVL() : root(NULL), treeSize(0), increase(false), decrease(false) {}
    virtual ~AVL() { clearTree(); }

    class Iterator {
    private:
        Node* node;
    public:
        Iterator(Node* node = NULL) : node(node) {}
        ~Iterator() { node = NULL; };
        
        /** Compares two iterators */
        bool operator!=(const Iterator& rhs) const {
            bool notEqual;
            (node == rhs.node) ? (notEqual = false) : (notEqual = true);
            return notEqual;
        }
        
        /** Increments iterator */
        Iterator& operator++();
        
        /** Returns data referenced by iterator */
        T& operator*() const {
            return node->data;
        }
    };
    
    /** Grabs iterator at beginning of in-order chain */
    Iterator begin() {
        Node* start = root;
        if (start == NULL) {
            return AVL<T>::Iterator(NULL);
        }
        while (start->left != NULL) {
            start = start->left;
        }
        return AVL<T>::Iterator(start);
    }
    
    /** Grabs an iterator to the position just beyond last in-order node */
    Iterator end() { return AVL<T>::Iterator(NULL); }
    
    /** Uses Iterators to print an in-order reading of the tree */
    std::string treeInOrder() {
        if (root == NULL) {
            return "empty";
        }
        std::ostringstream os;
        for (Iterator iter = begin(); iter != end(); ++iter) {
            os << *iter << " ";
        }
        return os.str();
    }
    
    /** Return number of nodes in tree */
    virtual size_t sizeTree() { return treeSize; }
    
    /** Return true if node added to AVL, else false */
    virtual bool addNode(const T& item) {
        return insert(root, root->parent, item);
    }
    
    /** Return true if node removed from AVL, else false */
    virtual bool removeNode(const T& data) { return erase(data, root); }
    
    /** Find a node based off of it's data */
    std::string find(T data) {
        return find(this->root, data);
    }

    /** Return true if AVL cleared of all nodes, else false */
    virtual bool clearTree() {
        clearTree(root);
        root = NULL;
        treeSize = 0;
        return true;
    }

    /** Return a level order traversal of a AVL as a string */
    std::string toString() const;
    
    friend std::ostream& operator<<(std::ostream& os, AVL<T>* avl) {
        return os << avl->toString();
    }
};

/** Add node to tree recursively */
template <typename T>
bool AVL<T>::insert(AVL<T>::Node*& current, AVL<T>::Node*& parent, const T& item)  {
    if (root == NULL) {
        root = new Node(item);
        treeSize++;
        increase = true;
        return true;
    }
    else if (current == NULL) {
        current = new Node(item, parent);
        treeSize++;
        increase = true;
        return true;
    }
    else if (item < current->data) {
        bool returnValue = insert(current->left, current, item);
        if (increase) {
            switch(current->balance) {
                case RIGHT_HEAVY:
                    current->balance = BALANCED;
                    increase = false;
                    break;
                case BALANCED:
                    current->balance = LEFT_HEAVY;
                    increase = true;
                    break;
                case LEFT_HEAVY:
                    rebalanceLeft(current);
                    increase = false;
                    break;
            }
        }
        return returnValue;
    }
    else if (item > current->data){
        bool returnValue = insert(current->right, current, item);
        if (increase) {
            switch(current->balance) {
                case LEFT_HEAVY:
                    current->balance = BALANCED;
                    increase = false;
                    break;
                case BALANCED:
                    current->balance = RIGHT_HEAVY;
                    increase = true;
                    break;
                case RIGHT_HEAVY:
                    rebalanceRight(current);
                    increase = false;
                    break;
            }
        }
        return returnValue;
    }
    else { //data == current->data
        increase = false;
        return false;
    }
}

/** Recursive function to erase a node from tree */
template <typename T>
bool AVL<T>::erase(const T& data, Node*& current)  {
    if (current == NULL) {
        decrease = false;
        return false;
    }
    else if (data < current->data) {
        bool returnValue = erase(data, current->left);
        if (decrease) {
            switch(current->balance) {
                case LEFT_HEAVY:
                    current->balance = BALANCED;
                    decrease = true;
                    break;
                case BALANCED:
                    current->balance = RIGHT_HEAVY;
                    decrease = false;
                    break;
                case RIGHT_HEAVY:
                    rebalanceRight(current, 'e');
                    if (current->balance != BALANCED) { decrease = false; }
                    break;
            }
        }
        return returnValue;
    }
    else if (data > current->data) {
        bool returnValue = erase(data, current->right);
        if (decrease) {
            switch(current->balance) {
                case RIGHT_HEAVY:
                    current->balance = BALANCED;
                    decrease = true;
                    break;
                case BALANCED:
                    current->balance = LEFT_HEAVY;
                    decrease = false;
                    break;
                case LEFT_HEAVY:
                    rebalanceLeft(current, 'e');
                    if (current->balance != BALANCED) { decrease = false; }
                    break;
            }
        }
        return returnValue;
    }
    else {
        Node* temp = current;
        if (current->left == NULL && current->right == NULL) {
            current = current->right; //Null Pointer
            decrease = true;
        }
        else if (current->left == NULL && current->right != NULL) {
            current->right->parent = current->parent;
            current = current->right;
            decrease = true;
        }
        else if (current->right == NULL && current->left != NULL) {
            current->left->parent = current->parent;
            current = current->left;
            decrease = true;
        }
        else {
            replaceParent(temp, temp->left);
            if (decrease) {
                switch(current->balance) {
                    case LEFT_HEAVY:
                        current->balance = BALANCED;
                        decrease = true;
                        break;
                    case BALANCED:
                        current->balance = RIGHT_HEAVY;
                        decrease = false;
                        break;
                    case RIGHT_HEAVY:
                        rebalanceRight(current, 'e');
                        if (current->balance != BALANCED) { decrease = false; }
                }
            }
        }
        treeSize--;
        delete temp;
        return true;
    }
}

/** Called by 'erase' to remove a node with two children */
template <typename T>
void AVL<T>::replaceParent(Node*& oldRoot, Node*& localRoot) {
    if (localRoot->right != NULL) {
        replaceParent(oldRoot, localRoot->right);
        if (decrease) {
            switch(localRoot->balance) {
                case BALANCED:
                    localRoot->balance = LEFT_HEAVY;
                    decrease = false;
                    break;
                case RIGHT_HEAVY:
                    localRoot->balance = BALANCED;
                    decrease = true;
                    break;
                case LEFT_HEAVY:
                    rebalanceLeft(localRoot, 'e');
                    if (localRoot->balance != BALANCED) { decrease = false; }
                    break;
            }
        }
    }
    else {
        oldRoot->data = localRoot->data;
        balanceType tempBalance = oldRoot->balance;
        oldRoot = localRoot;
        localRoot->balance = tempBalance;
        if (localRoot->left != NULL) {
            localRoot->left->parent = localRoot->parent;
        }
        localRoot = localRoot->left;
        decrease = true;
    }
}

/** Rotates a branch to the right */
template <typename T>
bool AVL<T>::rotateRight(Node*& root) {
    Node* parent = root->parent;
    Node* temp = root->left;
    root->left = temp->right;
    if (root->left != NULL) { root->left->parent = root; }
    temp->right = root;
    root = temp;
    root->right->parent = root;
    root->parent = parent;
    return true;
}

/** Rotates a branch to the left */
template <typename T>
bool AVL<T>::rotateLeft(Node*& root) {
    Node* parent = root->parent;
    Node* temp = root->right;
    root->right = temp->left;
    if (root->right != NULL) {
        root->right->parent = root;
    }
    temp->left = root;
    root = temp;
    root->left->parent = root;
    root->parent = parent;
    return true;
}

/** Rebalances a left branch of the tree */
template <typename T>
void AVL<T>::rebalanceLeft(Node*& root, char c) {
    Node*& leftSubTree = root->left;
    if (leftSubTree->balance == RIGHT_HEAVY) {
        Node*& leftRightSubTree = leftSubTree->right;
        if (leftRightSubTree->balance == LEFT_HEAVY) {
            leftSubTree->balance = BALANCED;
            leftRightSubTree->balance = BALANCED;
            root->balance = RIGHT_HEAVY;
        }
        else if (leftRightSubTree->balance == RIGHT_HEAVY) {
            leftSubTree->balance = LEFT_HEAVY;
            leftRightSubTree->balance = BALANCED;
            root->balance = BALANCED;
        }
        else {
            leftSubTree->balance = BALANCED;
            leftRightSubTree->balance = BALANCED;
            root->balance = BALANCED;
        }
        rotateLeft(leftSubTree);
    }
    else if (c == 'e' && leftSubTree->balance == BALANCED) {
        leftSubTree->balance = RIGHT_HEAVY;
        root->balance = LEFT_HEAVY;
    }
    else {
        leftSubTree->balance = BALANCED;
        root->balance = BALANCED;
    }
    rotateRight(root);
}

/** Rebalances a right branch of the tree */
template <typename T>
void AVL<T>::rebalanceRight(Node*& root, char c) {
    Node*& rightSubTree = root->right;
    if (rightSubTree->balance == LEFT_HEAVY) {
        Node*& rightLeftSubTree = rightSubTree->left;
        if (rightLeftSubTree->balance == RIGHT_HEAVY) {
            rightSubTree->balance = BALANCED;
            rightLeftSubTree->balance = BALANCED;
            root->balance = LEFT_HEAVY;
        }
        else if(rightLeftSubTree->balance == LEFT_HEAVY) {
            rightSubTree->balance = RIGHT_HEAVY;
            rightLeftSubTree->balance = BALANCED;
            root->balance = BALANCED;
        }
        else {
            rightSubTree->balance = BALANCED;
            rightLeftSubTree->balance = BALANCED;
            root->balance = BALANCED;
        }
        rotateRight(rightSubTree);
    }
    else if (c == 'e' && rightSubTree->balance == BALANCED) {
        rightSubTree->balance = LEFT_HEAVY;
        root->balance = RIGHT_HEAVY;
    }
    else {
        rightSubTree->balance = BALANCED;
        root->balance = BALANCED;
    }
    rotateLeft(root);
}

/** Output nodes at a given level */
template <typename T>
bool AVL<T>::outLevel(Node* root, int level, std::stringstream& os) const {
   if (root == NULL) return false;
   if (level == 1) {
       os << " " << root->data;// << ":" << root->balance;
      if ((root->left != NULL) || (root->right != NULL)) return true;
      return false;
   }
   if ((level == 2) && !root->left && root->right) os << " _";
   bool left = outLevel(root->left, level - 1, os);
   bool right = outLevel(root->right, level - 1, os);
   if ((level == 2) && root->left && !root->right) os << " _";
   return left || right;
}

/** Return a level order traversal of a AVL as a string */
template <typename T>
std::string AVL<T>::toString() const {
   std::stringstream os;
   if (root == NULL) os << "empty";
   else {
      int level = 0;
      do {
          os << std::endl << "  " << ++level << ":";
      } while (outLevel(root, level, os));
   }
   return os.str();
}

/** Increments iterator */
template <typename T>
typename AVL<T>::Iterator& AVL<T>::Iterator::operator++() {
    if (node->right != NULL) {
        node = node->right;
        while (node->left != NULL) {
            node = node->left;
        }
    }
    else if (node->right == NULL) {
        Node* prev = node;
        node = node->parent;
        while (node != NULL && (node->data < prev->data)) {
            prev = node;
            node = node->parent;
        }
    }
    return *this;
}


#endif /* AVL_H */
