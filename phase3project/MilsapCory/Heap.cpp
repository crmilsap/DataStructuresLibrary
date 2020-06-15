#include <cstdlib>
#include "CDA.cpp"

using namespace std;

template <class keytype, class valuetype>
class Heap
{
private:
    CDA<keytype> keys;
    CDA<valuetype> values;

    int parentIndex(int index)       { return index/2; }
    int leftChildIndex(int index)    { return 2 * index + 1; }
    int rightChildIndex(int index)   { return ((2 * index) + 2); }

    void minHeapify(int index);                             // Maintains min-heap property
public:
    Heap();                                                 // Default Constructor. Makes empty heap.
    Heap(keytype k[], valuetype V[], int s);                // Constructs a heap using the arrays in a bottom up heap building method.
    Heap(const Heap& src);                                  // Copy constructor
    ~Heap();                                                // Destructor

    Heap<keytype, valuetype>& operator=(const Heap& src);   // Copy assignment operator.

    keytype peekKey();                                      // Returns minimum key in heap. No modification.
    valuetype peekValue();                                  // Returns value associated with the minimum key. No modification.
    keytype extractMin();                                   // Removes and returns minimum key

    void insert(keytype k, valuetype v);                    // Inserts key k and value v pair into heap
    void printKey();                                        // Writes keys stored in the array starting at the root.
};

template <class keytype, class valuetype>
Heap<keytype, valuetype>::Heap()
{
    return;
}

template <class keytype, class valuetype>
Heap<keytype, valuetype>::Heap(keytype k[], valuetype V[], int s)
{
    // Populate arrays.
    for (int i = 0; i < s; i++) {
        keys.AddEnd(k[i]);
        values.AddEnd(V[i]);
    }

    // Heapify
    for (int i = s / 2 - 1; i >= 0; i--)
        minHeapify(i);
}

template <class keytype, class valuetype>
Heap<keytype, valuetype>::~Heap()
{
    return;
}

// Copy constructor
template <class keytype, class valuetype>
Heap<keytype, valuetype>::Heap(const Heap& src)
{
    keys = src.keys;
    values = src.values;

    return;
}

template <class keytype, class valuetype>
Heap<keytype, valuetype>& Heap<keytype, valuetype>::operator=(const Heap& src)
{
    keys = src.keys;
    values = src.values;

    return *this;
}
template <class keytype, class valuetype>
void Heap<keytype, valuetype>::insert(keytype k, valuetype v)
{
    keys.AddEnd(k);
    values.AddEnd(v);

    int index = keys.Length() - 1;

    while (index != 0 && keys[ ( index - 1) / 2 ] > keys[index] )
    {
        swap(keys[index], keys[ (index - 1) / 2]);
        swap(values[index], values[ (index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

template <class keytype, class valuetype>
void Heap<keytype, valuetype>::minHeapify(int index)
{
    
    /*  Called to maintain the min-heap property.
     *  Assumes that the binary trees that are below the index are min-heaps but that we may
     *  have to float down the value at the index.
     */

    int smallestIndex = index;
    int leftIndex = leftChildIndex(index);
    int rightIndex = rightChildIndex(index);

    if (leftIndex < keys.Length() && keys[leftIndex] < keys[smallestIndex])
    {
        smallestIndex = leftIndex;
    }
    
    if (rightIndex < keys.Length() && keys[rightIndex] < keys[smallestIndex])
    {
        smallestIndex = rightIndex;
    }

    // If smallest is not the root 
    if (smallestIndex != index)
    {
        // Swap values at smallest index with current index.

        // Swap keys array.
        swap(keys[index], keys[smallestIndex]);

        // Swap values array.
        swap(values[index], values[smallestIndex]);

        minHeapify(smallestIndex);
    }
}

template <class keytype, class valuetype>
keytype Heap<keytype, valuetype>::peekKey()
{
    return keys[0];
}

template <class keytype, class valuetype>
valuetype Heap<keytype, valuetype>::peekValue()
{
    return values[0];
}

template <class keytype, class valuetype>
keytype Heap<keytype, valuetype>::extractMin()
{
    if (keys.Length() < 1)
        cout << "ERROR: Heap Underflow" << endl;
    
    keytype minK = keys[0];
    keys[0] = keys[keys.Length() - 1];
    values[0] = values[values.Length() - 1];
    keys.DelEnd();
    values.DelEnd();

    minHeapify(0);

    return minK;
}

template <class keytype, class valuetype>
void Heap<keytype, valuetype>::printKey()
{
    for (int i = 0; i < keys.Length(); i++)
        cout << keys[i] << " ";
    cout << endl;
}
