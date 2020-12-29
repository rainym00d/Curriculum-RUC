#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H


#include <iomanip>
#include <iostream>
using namespace std;

template <class T>
class MinHeap{
    private:
        T *mHeap;        // 数据
        int mCapacity;    // 总的容量
        int mSize;        // 实际容量

    private:
        // 最小堆的向下调整算法
        void filterdown(int start, int end);
        // 最小堆的向上调整算法(从start开始向上直到0，调整堆)
        void filterup(int start);
    public:
        MinHeap();
        MinHeap(int capacity);
        ~MinHeap();

        int getIndex(T data);// 返回data在二叉堆中的索引
        int remove(T data);// 即extract_min
        int insert(T data);// 将data插入到二叉堆中
        int decrease_key(T pos,T data);
        int empty();
        T find_min();
};

/* 
 * 构造函数
 */
template <class T>
MinHeap<T>::MinHeap()
{
    new (this)MinHeap(100000);
}

template <class T>
MinHeap<T>::MinHeap(int capacity){
    mSize = 0;
    mCapacity = capacity;
    mHeap = new T[mCapacity];
}
/* 
 * 析构函数
 */
template <class T>
MinHeap<T>::~MinHeap() 
{
    mSize = 0;
    mCapacity = 0;
    delete[] mHeap;
}

/* 
 * 返回data在二叉堆中的索引
 *
 * 返回值：
 *     存在 -- 返回data在数组中的索引
 *     不存在 -- -1
 */
template <class T>
int MinHeap<T>::getIndex(T data)
{
    for(int i=0; i<mSize; i++)
        if (data == mHeap[i])
            return i;

    return -1;
}

/* 
 * 最小堆的向下调整算法
 *
 * 注：数组实现的堆中，第N个节点的左孩子的索引值是(2N+1)，右孩子的索引是(2N+2)。
 *
 * 参数说明：
 *     start -- 被下调节点的起始位置(一般为0，表示从第1个开始)
 *     end   -- 截至范围(一般为数组中最后一个元素的索引)
 */
template <class T>
void MinHeap<T>::filterdown(int start, int end)
{
    T temp;
    int left = 2*start + 1;
    int right = 2*start + 2;
    if(left > end)return;
    if(right > end)
    {
        if(mHeap[left] < mHeap[start])
        {
            temp = mHeap[left];
            mHeap[left] = mHeap[start];
            mHeap[start] = temp;
            filterdown(left,end);
            return;
        }else{
            return;
        }
    }else{
        if(mHeap[right] <= mHeap[left]&&mHeap[right] < mHeap[start])
        {
            temp = mHeap[right];
            mHeap[right] = mHeap[start];
            mHeap[start] = temp;
            filterdown(right,end);
            return;
        }else if(mHeap[left] < mHeap[right]&&mHeap[left] < mHeap[start])
        {
            temp = mHeap[left];
            mHeap[left] = mHeap[start];
            mHeap[start] = temp;
            filterdown(left,end);
            return;
        }
        else
        {
            return;
        }
        
    } 
}
 
/*
 * 删除最小堆中的data
 *
 * 返回值：
 *      0，成功
 *     -1，失败
 */
template <class T>
int MinHeap<T>::remove(T data)
{
    if(getIndex(data) == -1)return -1;
    mHeap[0] = mHeap[--mSize];
    filterdown(0,mSize-1);
    return 0;
}

/*
 * 最小堆的向上调整算法(从start开始向上直到0，调整堆)
 * 注：数组实现的堆中，第N个节点的左孩子的索引值是(2N+1)，右孩子的索引是(2N+2)。
 * 参数说明：start -- 被上调节点的起始位置(一般为数组中最后一个元素的索引)
 */
template <class T>
void MinHeap<T>::filterup(int start){
    int father = (start - 1 ) / 2;
    while(father >= 0 && mHeap[father] > mHeap[start] )
    {
        T temp = mHeap[father];
        mHeap[father] = mHeap[start];
        mHeap[start] = temp;
        start = father;
        father = (start - 1) / 2;
    }
}
  
/* 
 * 将data插入到二叉堆中
 * 返回值：
 *     0，表示成功
 *    -1，表示失败
 */
template <class T>
int MinHeap<T>::insert(T data){
    if(mSize == mCapacity)return -1;
    mHeap[mSize] = data;
    filterup(mSize++);
    return 0;
}


template <class T>
int MinHeap<T>::decrease_key(T pos,T data){
    int index = getIndex(pos);
    if(index == -1)return -1;
    if(mHeap[index] <= data)return -1;
    mHeap[index] = data;
    filterup(index);
    return 0;
}


template <class T>
T MinHeap<T>::find_min(){
    return mHeap[0];
}


template <class T>
int MinHeap<T>::empty(){
    if(mSize == 0)return 1;
    else return 0;
}

#endif

