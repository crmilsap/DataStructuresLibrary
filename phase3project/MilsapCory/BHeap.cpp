#include <cstdlib>

using namespace std;

template <typename keytype, typename valuetype>
struct BNode
{
    keytype key;
    valuetype value;
    int degree;

    BNode<keytype, valuetype> *child, *sibling, *parent;
};

template <class keytype, class valuetype>
class BHeap  
{
private:
    BNode<keytype, valuetype> *head;

    BNode<keytype, valuetype> *newNode(keytype k, valuetype v);
    void mergeTree(BNode<keytype, valuetype> *root1, BNode<keytype, valuetype> *root2); // Merge two trees of same degree together
    void preorder(BNode<keytype, valuetype> *root);
    void deleteTree(BNode <keytype, valuetype> *root);
    BNode<keytype, valuetype> copyTree(BNode<keytype, valuetype> *root);

public:
    BHeap() {
        head = nullptr;
    }
    BHeap(keytype k[], valuetype V[], int s);
    BHeap(const BHeap &src);
    ~BHeap();

    BHeap<keytype, valuetype> &operator=(const BHeap &src); // Copy assignment operator.

    BNode<keytype, valuetype> *getHead() { return head; }
    void setHead(BNode<keytype, valuetype> *head) { this->head = head; }

    keytype peekKey();
    valuetype peekValue();
    keytype extractMin();

    void insert(keytype k, valuetype v);
    void merge(BHeap<keytype, valuetype> &H2);

    void printKey();
};


template <class keytype, class valuetype>
BHeap<keytype, valuetype>::BHeap(keytype k[], valuetype V[], int s)
{
    for (int i = 0; i < s; i++)
    {
        insert(k[i], V[i]);
    }
}
template <class keytype, class valuetype>
BHeap<keytype, valuetype>::~BHeap()
{
    deleteTree(head);
}

template <class keytype, class valuetype>
void BHeap<keytype, valuetype>::deleteTree(BNode <keytype, valuetype> *root)
{
    if (root == nullptr) return;

    deleteTree(root->child);
    deleteTree(root->sibling);
    return;
}

template <class keytype, class valuetype>
keytype BHeap<keytype, valuetype>::extractMin()
{
    BNode<keytype, valuetype> *ptr = head;
    keytype min;
    while (ptr != nullptr)
    {
        if (ptr->key < min)
            min = ptr->key;
        ptr = ptr->sibling;
    }
    return min;
}
template <class keytype, class valuetype>
BHeap<keytype, valuetype>::BHeap(const BHeap &src)
{
    *this = src;
}
template <class keytype, class valuetype>
BHeap<keytype, valuetype>& BHeap<keytype, valuetype>::operator=(const BHeap& src)
{
    BNode<keytype, valuetype> *temp = src.head;
    copyTree(temp);

    setHead(temp);
    return *this;
}
template <class keytype, class valuetype>
BNode<keytype, valuetype> BHeap<keytype, valuetype>::copyTree(BNode<keytype, valuetype> *root)
{
    if (root == nullptr) return nullptr;
    BNode<keytype, valuetype> *child = copyTree(root->child);
    BNode<keytype, valuetype> *sibling = copyTree(root->sibilng);
    BNode<keytype, valuetype> *temp = new BNode<keytype, valuetype>;

    temp->key = root->key;
    temp->value = root->value;
    temp->degree = root->degree;
    temp->child = child;
    temp->sibling = sibling;

    return temp;

}

template <class keytype, class valuetype>
BNode<keytype, valuetype> *BHeap<keytype, valuetype>::newNode(keytype k, valuetype v)
{
    BNode<keytype, valuetype> *temp = new BNode<keytype, valuetype>;
    temp->key = k;
    temp->value = v;
    temp->degree = 0;
    temp->child = temp->sibling = temp->parent = nullptr;

    return temp;
}

template <class keytype, class valuetype>
void BHeap<keytype, valuetype>::insert(keytype k, valuetype v)
{
    BHeap<keytype, valuetype> h;
    BNode<keytype, valuetype> *x = newNode(k, v);
    h.setHead(x);
    merge(h);
    return;
}

template <class keytype, class valuetype>
void BHeap<keytype, valuetype>::mergeTree(BNode<keytype, valuetype> *p, BNode<keytype, valuetype> *q)
{

    if (p->key <= q->key)
    {
        // Make q a child of p
        q->sibling = p->child;
        p->child = q;
        p->degree = p->degree + 1;
    }
    else
    {
        // Make p a child of q
        p->sibling = q->child;
        q->child = p;
        q->degree = q->degree + 1;
    }
}

template <class keytype, class valuetype>
void BHeap<keytype, valuetype>::merge(BHeap<keytype, valuetype> &H2)
{
    BNode<keytype, valuetype> *curr1 = getHead(); // Tracks where we are in this heap
    BNode<keytype, valuetype> *curr2 = H2.getHead(); // Tracks where we are in the heap we are merging
    BNode<keytype, valuetype> *curr3 = nullptr;       // Tracks where we are in the heap we are building
    BNode<keytype, valuetype> *temp = nullptr;        // Acts as our head for the merged heap


    // If the smallest tree exists in this heap
    if ((curr1 != nullptr) && curr1->degree <= curr2->degree)
    {
        curr3 = curr1;
        curr1 = curr1->sibling;     // Move onto the next one
    }
    else {
        curr3 = curr2;
        curr2 = curr2->sibling;     // Move onto next tree in other heap
    }

    temp = curr3;                   // Set temp head.

    // We now merge the heaps. 
    // For now we disregard the trees that have the same degee
    while (curr1 != nullptr && curr2 != nullptr)
    {
        // If the next smallest degree comes from this heap
        if (curr1->degree <= curr2->degree)
        {
            curr3->sibling = curr1; // Add to the root list we are building
            curr1 = curr1->sibling;
        }
        else        // Do the same except for heap2
        {
            curr3->sibling = curr2;
            curr2 = curr2->sibling;
        }
        curr3 = curr3->sibling;         // Move forward with the new list
    }

    // If we have remaining elements in heap1
    if (curr1 != nullptr)
    {
        while (curr1 != nullptr)
        {
            curr3->sibling = curr1;
            curr1 = curr1->sibling;
            curr3 = curr3->sibling;
        }
    }

    // If we have remaining elements in heap2
    if (curr2 != nullptr)
    {
        while(curr2 != nullptr)
        {
            curr3->sibling = curr2;
            curr2 = curr2->sibling;
            curr3 = curr3->sibling;
        }
    }

    // Scan and deal with the same degrees
    curr3 = temp;   // Start at beggining of merged list
    BNode<keytype, valuetype> *prev = nullptr;
    BNode<keytype, valuetype> *next = curr3->sibling;

    while (next != nullptr)
    {
        // We will ignore the tree if two adjacent roots have different degrees
        // OR 3 consecutive trees have the same degree
        if ( ( curr3->degree != next->degree) || (next->sibling != nullptr && curr3->degree == next->sibling->degree))
        {
            prev = curr3;
            curr3 = next;
        }
        else {
            // Merging the trees with same degree
            if (curr3->key <= next->key)
                curr3->sibling = next->sibling;
            else {
                if (prev == nullptr)
                    temp = next;
                else
                    prev->sibling = next;

                curr3 = next;
            }
            mergeTree(next, curr3);
        }
        next = curr3->sibling;
    }

    // Update the head and we are done merging.
    setHead(temp);
}

template <class keytype, class valuetype>
void BHeap<keytype, valuetype>::printKey()
{
    BNode<keytype, valuetype> *curr;


    for (curr = getHead(); curr != nullptr; curr = curr->sibling)
    {
        cout << "B" << curr->degree << endl;
        preorder(curr);
        cout << endl;
    }

}

template <class keytype, class valuetype>
void BHeap<keytype, valuetype>::preorder(BNode<keytype, valuetype> *root)
{
    if (root == nullptr)
        return;

    cout << root->key << " ";
    preorder(root->child);
    preorder(root->sibling);
}