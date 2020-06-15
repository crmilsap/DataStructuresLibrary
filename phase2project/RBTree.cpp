#include <cstdlib>
using namespace std;

/*
    Assume that all keys are independent.
    Didn't talk about 2-3 trees or ABL trees in class.
*/

enum color { red, black };

template <typename keytype, typename valuetype>
struct Node
{
    keytype key;
    valuetype value;
    Node* left_child, *right_child, *parent;
    color c;
    int size;
};


template <class keytype, class valuetype>
class RBTree
{
private:
    int s;
    Node<keytype, valuetype>* root, *nil;

    void destroyTree(Node<keytype, valuetype>* x);                              // Destructor Helper    
    Node<keytype, valuetype>* copyTree(Node<keytype, valuetype>* copy,          // Copy Helper
                                        Node<keytype, valuetype>* parent);

    void leftRotate(Node<keytype, valuetype>* x);
    void rightRotate(Node<keytype, valuetype>* x);

    void insertFixup(Node<keytype, valuetype>* z);                      // Called after each insert to ensure tree doesn't violate red-black rules
    void deleteFixup(Node<keytype, valuetype>* x);
    void transplant(Node<keytype, valuetype>* u,                        // Helps remove function. Swaps one node with another.
                    Node<keytype, valuetype>* v);
    
    Node<keytype, valuetype>* findNode(keytype key);                    // Returns the node when given a key.
    Node<keytype, valuetype>* treeMin(Node<keytype, valuetype>* x);
    Node<keytype, valuetype>* treeMax(Node<keytype, valuetype>* x);

    keytype selectHelper(Node<keytype, valuetype>* x, int pos);

    void preorderHelper(Node<keytype, valuetype>* z);
    void inorderHelper(Node<keytype, valuetype>* z);
    void postorderHelper(Node<keytype, valuetype>* z);

    void print2DHelper(Node<keytype, valuetype>* root, int space);

public:
    RBTree();                                                           // Default constructor
    RBTree(keytype k[], valuetype V[], int s);                          // Custom constructor
    RBTree(const RBTree& src);                                          // Copy constructor
    ~RBTree();

    RBTree<keytype, valuetype>& operator=(const RBTree& src);           // Copy assignment operator 

    valuetype *search(keytype k);                                       // Traditional search. Returns pointer to the valuetype stored with key or NULL.
    void insert(keytype k, valuetype v);
    int remove(keytype k);                                              // Returns 1 if finds node – 0 otherwise.
    int rank(keytype k);
    keytype select(int pos);

    keytype *successor(keytype k);
    keytype *predecessor(keytype k);

    int size();

    void preorder();
    void inorder();
    void postorder();

    void print2D();                                                     // Useful print function to visualize tree
};

// Default Constructor
template <class keytype, class valuetype>
RBTree<keytype, valuetype>::RBTree()
{
    s = 0;
    nil = new Node<keytype, valuetype>;
    root = nil;
    nil->c = black;
    nil->parent = nil;
    nil->left_child = nil;
    nil->right_child = nil;
    nil->size = 0;
}

// Custom constructor that uses a key and value array to build the tree.
template <class keytype, class valuetype>
RBTree<keytype, valuetype>::RBTree(keytype k[], valuetype V[], int s)
{
    this->s = 0;
    nil = new Node<keytype, valuetype>;
    root = nil;
    nil->c = black;
    nil->parent = nil;
    nil->left_child = nil;
    nil->right_child = nil;
    nil->size = 0;

    for (int i = 0; i < s; i++)
        insert(k[i], V[i]);
}

// Destructor
template <class keytype, class valuetype>
RBTree<keytype, valuetype>::~RBTree()
{
    destroyTree(root);
    delete nil;
}

// Destructor Helper
template<class keytype, class valuetype>
void RBTree<keytype, valuetype>::destroyTree(Node<keytype, valuetype>* x)
{
    if (x == nil)
        return;

    destroyTree(x->left_child);
    destroyTree(x->right_child);
    delete x;
}

// Copy constructor
template <class keytype, class valuetype>
RBTree<keytype, valuetype>::RBTree(const RBTree& src)
{
    s = src.s;
    nil = new Node<keytype, valuetype>;
    nil->c = black;
    nil->parent = nil;
    nil->left_child = nil;
    nil->right_child = nil;
    nil->size = 0;

    root = copyTree(src.root, nil);
}

// Copy constructor helper
template <class keytype, class valuetype>
Node<keytype, valuetype>* RBTree<keytype, valuetype>::copyTree(Node<keytype, valuetype>* copy, Node<keytype, valuetype>* parent)
{
    if (copy->parent == copy)
        return nil;
    
    Node<keytype, valuetype>* new_node = new Node<keytype, valuetype>;

    new_node->key = copy->key;
    new_node->value = copy->value;
    new_node->c = copy->c;
    new_node->size = copy->size;
    new_node->parent = parent;

    new_node->left_child = copyTree(copy->left_child, new_node);
    new_node->right_child = copyTree(copy->right_child, new_node);

    return new_node;
}

// Copy Assignment Operator
template <class keytype, class valuetype>
RBTree<keytype, valuetype>& RBTree<keytype, valuetype>::operator=(const RBTree& src)
{
    s = src.s;
    nil = src.nil;
    nil->c = black;
    nil->parent = nil;
    nil->left_child = nil;
    nil->right_child = nil;
    nil->size = 0;

    root = copyTree(src.root, nil);

    return *this;
}
// Left Rotate
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::leftRotate(Node<keytype, valuetype>* x)
{
    if (x == nil) return;

    // Set y
    Node<keytype, valuetype>* y = x->right_child;
    // Turn y's left subtree into x's right subtree
    x->right_child = y->left_child;
    
    if (y->left_child != nil)
        y->left_child->parent = x;

    // Link x's parent to y
    y->parent = x->parent;

    if (x->parent == nil)
        root = y;
    else if (x == x->parent->left_child)
        x->parent->left_child = y;
    else
        x->parent->right_child = y;

    // Put x on y's left
    y->left_child = x;
    x->parent = y;
    y->size = x->size;
    x->size = x->left_child->size + x->right_child->size + 1;
}

// Right Rotate
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::rightRotate(Node<keytype, valuetype>* x)
{
    if (x == nil) return;

    // Set y
    Node<keytype, valuetype>* y = x->left_child;
    x->left_child = y->right_child;
    
    if (y->right_child != nil)
        y->right_child->parent = x;
    y->parent = x->parent;

    if (x->parent == nil)
        root = y;
    else if (x == x->parent->right_child)
        x->parent->right_child = y;
    else
        x->parent->left_child = y;
    
    y->right_child = x;
    x->parent = y;
    y->size = x->size;
    x->size = x->left_child->size + x->right_child->size + 1;
}

// Search
template <class keytype, class valuetype>
valuetype* RBTree<keytype, valuetype>::search(keytype k)
{
    Node<keytype, valuetype> *curr = root;
    while (curr != nil)
    {
        if (k == curr->key)
            return &(curr->value);
        else if (k > curr->key)
            curr = curr->right_child;
        else
            curr = curr->left_child;
    }
    return nullptr;
}

// Insertion
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::insert(keytype k, valuetype v)
{
    // Create new node and initialize values. We insert with the node as red.
    Node<keytype, valuetype>* z = new Node<keytype, valuetype>;
    z->key = k;
    z->value = v;
    z->c = red;
    z->parent = nil;
    z->size = 1;

    Node<keytype, valuetype>* y = nil;
    Node<keytype, valuetype>* x = root;

    // Find where to insert the node.
    while (x != nil)
    {
        y = x;
        x->size++;
        if (z->key < x->key)
            x = x->left_child;
        else
            x = x->right_child;

    }
    z->parent = y;

    // Insert node into tree
    if (y == nil)
        root = z;
    else if (z->key < y->key)
        y->left_child = z;
    else
        y->right_child = z;

    z->left_child = nil;
    z->right_child = nil;
    z->c = red;

    s++;

    // Call insert fixup to correct any violations we created
    insertFixup(z);
}

// Insertion Helper
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::insertFixup(Node<keytype, valuetype>* z)
{
    Node<keytype, valuetype>* y = nil;

    while (z->parent->c == red) {
        if (z->parent == z->parent->parent->left_child) {
            y = z->parent->parent->right_child;
            if (y->c == red) {
                z->parent->c = black;
                y->c = black;
                z->parent->parent->c = red;
                nil->c = black;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right_child) {
                    z = z->parent;
                    leftRotate(z);
                }
                z->parent->c = black;
                z->parent->parent->c = red;
                nil->c = black;
                rightRotate(z->parent->parent);
            }
        }
        else {
            y = z->parent->parent->left_child;
            if (y->c == red) {
                z->parent->c = black;
                y->c = black;
                z->parent->parent->c = red;
                nil->c = black;
                z = z->parent->parent;
            }
            else { 
                if (z == z->parent->left_child) {
                    z = z->parent;
                    rightRotate(z);
                }
                z->parent->c = black;
                z->parent->parent->c = red;
                nil->c = black;
                leftRotate(z->parent->parent);
            }
        }
        nil->c = black;
    }
    root->c = black;
}

template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::transplant(
    <keytype, valuetype>* u, Node<keytype, valuetype>* v)
{
    /*  Swaps two nodes in the tree with one another.   */

    if (u->parent == nil)
        root = v;
    else if (u == u->parent->left_child)
        u->parent->left_child = v;
    else
        u->parent->right_child = v;

    v->parent = u->parent;
}

// Removal
template <class keytype, class valuetype>
int RBTree<keytype, valuetype>::remove(keytype k)
{
    Node<keytype, valuetype>* z = findNode(k);
    if (z == nil) return 0;

    Node<keytype, valuetype>* y = z;
    Node<keytype, valuetype>* x = nil;

    color y_color = y->c;
    
    if (z->left_child == nil) {
        x = z->right_child;
        transplant(z, z->right_child);
    }
    else if (z->right_child == nil) {
        x = z->left_child;
        transplant(z, z->left_child);
    }
    else {
        y = treeMin(z->right_child);
        y_color = y->c;
        x = y->right_child;
        if (y->parent == z)
            x->parent = y;
        else {
            transplant(y, y->right_child);
            y->right_child = z->right_child;
            y->right_child->parent = y;
        }

        transplant(z, y);
        y->left_child = z->left_child;
        y->left_child->parent = y;
        y->c = z->c;
        nil->c = black;
    }
    if (y_color == black)
        deleteFixup(x);

    s--;
    return 1;
}

// Removal Helper
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::deleteFixup(Node<keytype, valuetype>* x)
{
    Node<keytype, valuetype>* w = nullptr;

    while ((x != root) && (x->c == black))
    {
        if (x == x->parent->left_child)
        {
            w = x->parent->right_child;
            if (w->c == red)
            {
                w->c = black;
                x->parent->c = red;
                nil->c = black;
                leftRotate(x->parent);
                w = x->parent->right_child;
            }
            if ((w->left_child->c == black) && (w->right_child->c == black))
            {
                w->c = red;
                x = x->parent;
                nil->c = black;
            }
            else { 
                if (w->right_child->c == black)
                {
                    w->left_child->c = black;
                    w->c = red;
                    nil->c = black;
                    rightRotate(w);
                    w = x->parent->right_child;
                }
                w->c = x->parent->c;
                nil->c = black;
                x->parent->c = black;
                w->right_child->c = black;
                leftRotate(x->parent);
                x = root;
            }
        }
        else
        {
            w = x->parent->left_child;
            if (w->c == red)
            {
                w->c = black;
                x->parent->c = red;
                nil->c = black;
                rightRotate(x->parent);
                w = x->parent->left_child;
            }
            if ((w->left_child->c == black) && (w->right_child->c == black))
            {
                w->c = red;
                nil->c = black;
                x = x->parent;
            }
            else { 
                if (w->left_child->c == black)
                {
                    w->right_child->c = black;
                    w->c = red;
                    nil->c = black;
                    leftRotate(w);
                    w = x->parent->left_child;
                }
                w->c = x->parent->c;
                nil->c = black;
                x->parent->c = black;
                w->left_child->c = black;
                rightRotate(x->parent);
                x = root;
            }
        }
    }
    x->c = black;
}

template <class keytype, class valuetype>
int RBTree<keytype, valuetype>::rank(keytype k)
{
    Node<keytype, valuetype>* x = findNode(k);
    if (x == nil) return 0;

    Node<keytype, valuetype>* y = x;

    int r = x->left_child->size + 1;
    while (y != root)
    {
        if (y == y->parent->right_child)
            r = r + y->parent->left_child->size + 1;
        y = y->parent;
    }
    return r;
}

template <class keytype, class valuetype>
keytype RBTree<keytype, valuetype>::select(int pos)
{
    return selectHelper(root, pos);
}

template <class keytype, class valuetype>
keytype RBTree<keytype, valuetype>::selectHelper(Node<keytype, valuetype>* x, int pos)
{
    int r = x->left_child->size + 1;

    if (pos == r)
        return x->key;
    else if (pos < r)
        return selectHelper(x->left_child, pos);
    else
        return selectHelper(x->right_child, pos - r);
}
template <class keytype, class valuetype>
Node<keytype, valuetype>* RBTree<keytype, valuetype>::treeMin(Node<keytype, valuetype>* x)
{
    Node<keytype, valuetype>* curr = x;
    while (curr->left_child != nil) {
        curr = curr->left_child;
    }
    return curr;
}

template <class keytype, class valuetype>
Node<keytype, valuetype>* RBTree<keytype, valuetype>::treeMax(Node<keytype, valuetype>* x)
{
    Node<keytype, valuetype>* curr = x;
    while (curr->right_child != nil) {
        curr = curr->right_child;
    }
    return curr;
}

template <class keytype, class valuetype>
keytype* RBTree<keytype, valuetype>::successor(keytype k)
{
    Node<keytype, valuetype>* x = findNode(k);
    if (x->right_child != nil)
    {
        Node <keytype, valuetype>* w = treeMin(x->right_child);
        return &(w->key);
    }

    Node<keytype, valuetype>* y = x->parent;
    while ((y != nil) && (x == y->right_child))
    {
        x = y;
        y = y->parent;
    }
    if (y == nil) return nullptr;

    return &(y->key);
}

template <class keytype, class valuetype>
keytype* RBTree<keytype, valuetype>::predecessor(keytype k)
{
    Node<keytype, valuetype>* x = findNode(k);
    if (x->left_child != nil)
    {
        Node <keytype, valuetype>* w = treeMax(x->left_child);
        return &(w->key);
    }

    Node<keytype, valuetype>* y = x->parent;
    while ((y != nil) && (x == y->left_child))
    {
        x = y;
        y = y->parent;
    }
    if (y == nil) return nullptr;

    return &(y->key);
}

template <class keytype, class valuetype>
int RBTree<keytype, valuetype>::size()
{
    return s;
}

template <class keytype, class valuetype>
Node<keytype, valuetype>* RBTree<keytype, valuetype>::findNode(keytype key)
{
    Node<keytype, valuetype> *curr = root;
    while (curr != nil)
    {
        if (key == curr->key)
            return curr;
        else if (key > curr->key)
            curr = curr->right_child;
        else
            curr = curr->left_child;
    }
    return nil;
}


////////////////////////////////////////////////////////////
////                   TREE TRAVERSAL                   ////
////////////////////////////////////////////////////////////

// Preorder
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::preorder()
{
    preorderHelper(root);
    cout << endl;
}

// Preorder Printout Helper
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::preorderHelper(Node<keytype, valuetype>* z)
{
    if (z == nil) return;

    cout << z->key << " ";
    preorderHelper(z->left_child);
    preorderHelper(z->right_child);
}

template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::inorder()
{
    inorderHelper(root);
    cout << endl;
}

// Inorder Printout Helper
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::inorderHelper(Node<keytype, valuetype>* z)
{
    if (z == nil) return;

    inorderHelper(z->left_child);
    cout << z->key << " ";
    inorderHelper(z->right_child);
}

template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::postorder()
{
    postorderHelper(root);
    cout << endl;
}

// Preorder Printout Helper
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::postorderHelper(Node<keytype, valuetype>* z)
{
    if (z == nil) return;

    postorderHelper(z->left_child);
    postorderHelper(z->right_child);
    cout << z->key << " ";
}

// 2D Print Function taken from https://www.geeksforgeeks.org/print-binary-tree-2-dimensions/
template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::print2D()
{
    print2DHelper(root, 0);
}

template <class keytype, class valuetype>
void RBTree<keytype, valuetype>::print2DHelper(Node<keytype, valuetype>* root, int space)
{
    const int COUNT = 10;

    // Base case  
    if (root == nil)  
        return;  
  
    // Increase distance between levels  
    space += COUNT;  
  
    // Process right child first  
    print2DHelper(root->right_child, space);  
  
    // Print current node after space  
    // count  
    cout<<endl;  
    for (int i = COUNT; i < space; i++)  
        cout<<" ";
    if (root->c == red) cout <<"r";
    cout<<root->key<<"\n";  
  
    // Process left child  
    print2DHelper(root->left_child, space);
}

