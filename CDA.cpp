/*
Library started on January 27, 2020 as part of CS201 projects.
The goal is to create a library of useful DataStructures throughout the semester.
*/

#include <cstdlib>
#include <time.h>

template <class T>
class CDA
{
private:
    int size, capacity, headIndex, tailIndex;
    bool ordered;
    T *outOfBoundsReference;
    T *a;

    int GetRealIndex(int i);

    void upsize();
    void downsize();

    void InsertionSortIndices(int leftIndex, int rightIndex);

    int QuickSelect(int leftIndex, int rightIndex, int k);
    int Partition(int leftIndex, int rightIndex);
    int RPartition(int leftIndex, int rightIndex);  // Used for selecting a random index as pivot. Uses median of three approach for pivot index.

    void QuickSortHelper(int leftIndex, int rightIndex);

    int BinarySearch(T key, int leftIndex, int rightIndex);      // Returns index of t or -1 if not found
    int LinearSearch(T key);      // Returns index of t or -1 if not found
public:
    CDA();                              // Default Constructor initializes an array of capacity 1.
    CDA(int s);                         // Constructor initializes an array of capacity s.
    CDA(const CDA& src);                // Copy constructor
    ~CDA();                             // Destructor clears up memory used.

    CDA<T>& operator=(const CDA &src);  // Copy Assignment operator 
    T& operator[](int i);               // Allows for easy access to array in class

    void AddEnd(T v);
    void AddFront(T v);
    void DelEnd();
    void DelFront();

    int Length()    { return size; }
    int Capacity()  { return capacity; }
    void Clear();
    
    bool Ordered()  { return ordered; }
    int SetOrdered();

    T Select(int k);                    // Returns the kth smallest element in the array.
    void InsertionSort();
    void QuickSort();
    void CountingSort(int m);
    int Search(T key);

    // Methods are not required for the project
    void Print();
};

// Default Constructor
template <class T>
CDA<T>::CDA()
{
    size = headIndex = tailIndex = 0; 
    capacity = 1;
    ordered = false;
    outOfBoundsReference = new T;
    a = new T[capacity];
}

// Constructor to intialize an array of size s and type T
template <class T>
CDA<T>::CDA(int s)
{
    headIndex = 0;
    tailIndex = s - 1; 
    size = capacity = s;
    ordered = false;
    outOfBoundsReference = new T;
    a = new T[capacity];
}

// Constructor to ensure deep copy of array by calling copy assignment operator.
template <class T>
CDA<T>::CDA(const CDA &src)
{
    *this = src;
}

// Destructor
template <class T>
CDA<T>::~CDA()
{
    delete[] a;
}

// Assignment operator
template <class T>
CDA<T>& CDA<T>::operator=(const CDA &src)
{
    headIndex = src.headIndex;
    tailIndex = src.tailIndex;

    size = src.size;
    capacity = src.capacity;

    ordered = src.ordered;
    outOfBoundsReference = src.outOfBoundsReference;

    a = new T[capacity];
    for (int i = 0; i < capacity; i++)
        a[i] = src.a[i];

    return *this;
}
// Overloading operator so that the user can index the array easily
template <class T>
T& CDA<T>::operator[](int i)
{
    if (i < 0 || i >= size)
    {
        cout << "ERROR: Out of Bounds Reference!" << endl;
        return *outOfBoundsReference;
    }
    return a[ ((headIndex+ i) % capacity)];
}

// Doubles the size of a[]
// This is called when the current array is full
template <class T>
void CDA<T>::upsize()
{
    int newCapacity = 2 * capacity;
    T *temp = new T[newCapacity];

    for (int i = 0; i < size; i++) 
        temp[i] = a[GetRealIndex(i)];

    delete[] a;
    a = temp;

    headIndex = 0;
    tailIndex = size - 1;
    capacity = newCapacity;
}

// Function called when the array size is 1/4 of the capacity so that ammortized runtime is constant O(1)
template <class T>
void CDA<T>::downsize()
{
    int newCapacity = capacity / 4;
    T * temp = new T[newCapacity];
    
    for (int i = 0; i < size; i++)
        temp[i] = a[GetRealIndex(i)];
    delete[] a;
    a = temp;

    headIndex = 0;
    tailIndex = size - 1;
    capacity = newCapacity;
}

template <class T>
void CDA<T>::AddEnd(T v)
{
    if (size == 0) // Case 1 : Array is empty so the headIndex = tailIndex
    {
        a[0] = v;
        ordered = true;
    }
    else { // Case 2: Array is not empty
        if (size == capacity)  // Check if array is full so we can double it
            upsize();
        
        if (ordered == true && !(a[GetRealIndex(tailIndex)] <= v) )  // Check to see if increasing order is maintained.
            ordered = false;

        tailIndex++;
        a[GetRealIndex(size)] = v; // Assign v to the end of the array.
    }
    size++;
}

template <class T>
void CDA<T>::AddFront(T v)
{
    if (size == 0) // Case 1: Array is empty so the headIndex = tailIndex
    {
        a[0] = v;
        ordered = true;
    }
    else {           // Case 2: Array is not empty
        if (size == capacity) // Check if array is full/if we need to double it.
            upsize();
        
        if (ordered == true && !(a[GetRealIndex(0)] >= v) ) // Check to see if the array is still in increasing order
            ordered = false;
        
        if (headIndex == 0) headIndex = capacity - 1;
        else headIndex--;

        (*this)[0] = v;
    }
    size++;
}

template <class T>
void CDA<T>::DelEnd()
{
    if (size <= 1) // Special case to ensure size and capacity are valid numbers.
    {
        size = 0;
        return;
    }
    
    size--;
    if (tailIndex == 0) tailIndex = capacity - 1;
    else tailIndex--;

    if (size == capacity / 4)
    {
        downsize();
    }
}

template <class T>
void CDA<T>::DelFront()
{
    if (size <= 1) // Special case to ensure size and capacity are valid numbers.
    {
        size = 0;
        return;
    }

    size--;
    headIndex++;

    if (size == capacity / 4)
    {
        downsize();
    }
}

// Clears any space the array is currently using.
template <class T>
void CDA<T>::Clear()
{
    size = headIndex = tailIndex = 0;
    capacity = 1;
    ordered = false;

    delete[] a;
    a = new T[capacity];
}

template <class T>
int CDA<T>::SetOrdered()
{
    ordered = true;
    for (int i = 0; i < size - 1; i++)
    {
        if ((*this)[i] <= (*this)[i + 1])
            continue;
        else {
            ordered = false;
            return -1;
        }
    }
    return 1;
}

template <class T>
T CDA<T>::Select(int k)
{
    // Ordered is true so we have O(1) access.
    if (ordered == true)
        return (*this)[k - 1];
    
    // Not ordered so we use QuickSelect to get the kth smallest value in O(size)
    // We will use the (*this) operator to simplify things inside of the function.
    int leftIndex = 0;
    int rightIndex = size - 1; 
    
    int kIndex = QuickSelect(leftIndex, rightIndex, k);
    
    if (kIndex == -1)
        cout << "ERROR: Invalid selection! k < size in Select(int k)" << endl;

    return (*this)[kIndex];
}

// Returns the index that would be used if the array was not circular. So the start is always 0 and end is size - 1.
template <class T>
int CDA<T>::GetRealIndex(int i)
{
    return ( (headIndex+ i) % capacity);   
}

template <class T>
int CDA<T>::QuickSelect(int leftIndex, int rightIndex, int k)
{
    // If k is less than number of elements in array
    if (k > 0 && k <= rightIndex - leftIndex + 1)
    {
        // Partition the array around random element and get position of pivot element in array
        int index = RPartition(leftIndex, rightIndex);

        // If position is same as k
        if (index - leftIndex == k - 1)
            return index;

        // If position is more, recur for left subarray
        if (index - leftIndex > k - 1)
            return QuickSelect(leftIndex, index - 1, k);

        // Else recur for right subarray
        return QuickSelect(index + 1, rightIndex, k - index + leftIndex - 1);
    }   
    
    // If k is invalid
    return -1;
}

template <class T>
int CDA<T>::Partition(int leftIndex, int rightIndex)
{
    T pivot = (*this)[rightIndex];
    int i = leftIndex;

    for (int j = leftIndex; j <= rightIndex - 1; j++)
    {
        if ((*this)[j] <= pivot)
        {
            swap((*this)[i], (*this)[j] );
            i++;
        }
    }
    swap((*this)[i], (*this)[rightIndex]);
    return i;
}

template <class T>
int CDA<T>::RPartition(int leftIndex, int rightIndex)
{
    srand(time(NULL));
    if (leftIndex == rightIndex)
        return Partition(leftIndex, rightIndex);
    
    int randomIndices[3] = {0};
    for (int i = 0; i < 3; i++)
        randomIndices[i] = leftIndex + rand() % (rightIndex - leftIndex);
    
    
    // Sort the randomIndices so we can get the median.
    if (randomIndices[0] > randomIndices[1])
        swap(randomIndices[0], randomIndices[1]);
    if (randomIndices[0] > randomIndices[2])
        swap(randomIndices[0], randomIndices[2]);
    if (randomIndices[1] > randomIndices[2])
        swap(randomIndices[1], randomIndices[2]);

    swap( (*this)[randomIndices[1]], (*this)[rightIndex] );

    return Partition(leftIndex, rightIndex);
}

template <class T>
void CDA<T>::InsertionSort()
{
    InsertionSortIndices(0, size - 1);
    ordered = true;
}

template <class T>
void CDA<T>::InsertionSortIndices(int leftIndex, int rightIndex)
{
    for (int i = leftIndex + 1; i <= rightIndex; i++)
    {
        T key = (*this)[i];
        int j = i - 1;

        while (j >= 0 && (*this)[j] > key)
        {
            (*this)[j + 1] = (*this)[j];
            j--;
        }
        (*this)[j + 1] = key;
    }
    return;
}

template <class T>
void CDA<T>::QuickSort()
{
    int leftIndex = 0;
    int rightIndex = size - 1;

    QuickSortHelper(leftIndex, rightIndex);

    ordered = true;
}

template <class T>
void CDA<T>::QuickSortHelper(int leftIndex, int rightIndex)
{
    const int INSERTION_CUTOFF = 1600;       // This constant is used to switch to insertion sort once a certain size is reached to maximize efficiency.
    if (leftIndex < rightIndex)
    {
        if (rightIndex - leftIndex <= INSERTION_CUTOFF)
            InsertionSortIndices(leftIndex, rightIndex);
        else {
            int partitionIndex = RPartition(leftIndex, rightIndex);

            QuickSortHelper(leftIndex, partitionIndex - 1);
            QuickSortHelper(partitionIndex + 1, rightIndex);
        }
    }
}

template <class T>
void CDA<T>::CountingSort(int m)
{
    int *output = new int[size]();
    int *count = new int[m + 1]();

    for (int i = 0; i < size; ++i)
        ++count[(*this)[i]];
    
    for (int i = 1; i <= m; ++i)
    {
        count[i] += count[i - 1];
    }

    for (int i = 0; i < size; ++i)
    {
        output[ count[ (*this)[i] ] - 1 ] = (*this)[i];
        --count[ (*this)[i] ];
    }

    delete[] count;

    for (int i = 0; i < size; ++i)
        (*this)[i] = output[i];

    delete[] output;

    ordered = true;

    return;
}

template <class T>
int CDA<T>::Search(T key)
{
    if (ordered == true)
        return BinarySearch(key, 0, size - 1);
    
    return LinearSearch(key);
}

template <class T>
int CDA<T>::BinarySearch(T key, int leftIndex, int rightIndex)
{
    if (rightIndex >= leftIndex)
    {
        int mid = leftIndex + (rightIndex - leftIndex) / 2;

        // Check if element is the middle element
        if ( (*this)[mid] == key)
            return mid;
        
        // If element is left recur on left subarray.
        if ( (*this)[mid] > key)
            return BinarySearch(key, leftIndex, mid - 1);

        // Else recur right subarray.
        return BinarySearch(key, mid + 1, rightIndex);
    }
    return -1;
}

template <class T>
int CDA<T>::LinearSearch(T key)
{
    for (int i = 0; i < size; i++)
    {
        if ((*this)[i] == key)        // Makes sure to account for floats
            return i;
    }
    return -1;
}

template <class T>
void CDA<T>::Print()
{
    for (int i = 0; i < this->size; i++)
        {
            cout << this->a[((this->headIndex + i)  % this->capacity)] << " ";
        }
        cout << endl;
    /*for (int i = 0; i < size; i++)
    {
        cout << "(*this)[" << i << "] = " << (*this)[i] << endl;
    }
    for (int i = 0; i < capacity; i++)
        cout << "a["  << i << "] = " << (*this)[i] << endl;
        */
}