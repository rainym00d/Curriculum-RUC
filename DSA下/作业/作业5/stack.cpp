/*
 * @Description: 用优先队列实现栈
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-10-14 11:55:21
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-10-17 15:49:40
 */

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

const int MAX_LENGTH = 128;     // 堆数组的最大长度
const int INF = -233333333;     // 定义最小的下界 
const int SUP = 233333333;      // 定义最大的上界

struct Node
{
    // node由键值对组成
    int key;    
    int value;  
    // 默认构造函数
    Node() = default;    
    // 自定义构造函数
    Node(int k, int v)
        :key(k), value(v)
    {
    }
};

class MaxHeap
{
private:
    Node *A;
    int length;     // 堆数组总长度
    int heap_size;  // 堆数组当前长度

public:
    MaxHeap(int length = MAX_LENGTH);
    ~MaxHeap();
    // 最小堆的有关代码实现参考算法导论第六章最大堆的实现
    // heap
    void MaxHeapify(int i); // 维护最大堆
    void BuildMaxHeap();    // 构建最大堆
    // priority queue
    Node HeapMaximum();     // 返回最大堆顶，即最大的数
    Node HeapExtractMax();  // 取出堆顶
    void HeapIncreaseKey(int index, int key);   // 增大堆中某个index的值
    void HeapInsert(int key, int value);    // 插入堆
    // 辅助函数
    void PrintHeap();   // 打印堆数组
    int GetHeapSize();  // 返回堆数组当前打下

private:
    inline int PARENT(int i);
    inline int LEFT(int i);
    inline int RIGHT(int i);
};

MaxHeap::MaxHeap(int length)
    : length(length), heap_size(0)
{
    this->A = new Node[length];
}

MaxHeap::~MaxHeap()
{
    delete this->A;
}

void MaxHeap::MaxHeapify(int i)
{
    int largest = i;
    int l = this->LEFT(i);
    int r = this->RIGHT(i);
    if (l < this->heap_size && this->A[l].key > this->A[largest].key) 
    {
        largest = l;
    }
    if (r < this->heap_size && this->A[r].key > this->A[largest].key) 
    {
        largest = r;
    }
    if (largest != i) 
    {
        swap(this->A[i], this->A[largest]);
        MaxHeapify(largest);
    }
}

void MaxHeap::BuildMaxHeap()
{
    for (int i = (this->heap_size >> 1) - 1; i >= 0; i --) 
    {
        this->MaxHeapify(i);
    }
}

Node MaxHeap::HeapMaximum()
{
    if (this->heap_size <= 0)
    {
        cout << "[ERROR] Underflow!" << endl;
        return Node(INF, INF);
    }
    
    return this->A[0];
}

Node MaxHeap::HeapExtractMax()
{
    if (this->heap_size <= 0)
    {
        cout << "[ERROR] Underflow!" << endl;
        return Node(INF, INF);
    }
    Node max = this->A[0];
    this->A[0] = this->A[this->heap_size - 1];
    this->heap_size --;
    this->MaxHeapify(0);
    return max;
}

void MaxHeap::HeapIncreaseKey(int index, int key)
{
    if (index < 0 || index >= this->heap_size)
    {
        cout << "[ERROR] Index out of bounds!" << endl;
        return;
    }
    if (this->A[index].key > key)
    {
        cout << "[ERROR] New key is smaller than current key!" << endl;
        return;
    }
    this->A[index].key = key;
    
    int i = index;
    while (i > 0 && this->A[this->PARENT(i)].value < this->A[i].value)
    {
        swap(this->A[this->PARENT(i)], this->A[i]);
        i = this->PARENT(i);
    }
}

void MaxHeap::HeapInsert(int key, int value)
{
    if (this->heap_size >= this->length)
    {
        cout << "[ERROR] Overflow!" << endl;
        return;
    }
    this->heap_size ++;
    this->A[this->heap_size - 1].key = INF;
    this->A[this->heap_size - 1].value = value;
    this->HeapIncreaseKey(this->heap_size - 1, key);
}

void MaxHeap::PrintHeap()
{
    cout << "length is " << this->length << "\nheap_size is " << this->heap_size << endl;
    for (int i = 0; i < this->length; i ++)
    {
        cout << "(" << this->A[i].key << ", " << this->A[i].value << ") ";
    }
    cout << endl;
}

int MaxHeap::GetHeapSize()
{
    return this->heap_size;
}

inline int MaxHeap::PARENT(int i)
{
    return ((i + 1) >> 1) - 1;
}

inline int MaxHeap::LEFT(int i)
{
    return ((i + 1) << 1) - 1;
}

inline int MaxHeap::RIGHT(int i)
{
    return ((i + 1) << 1);
}

// 栈
class Stack
{
private:
    MaxHeap max_heap;

public:
    Stack() = default;
    ~Stack() = default;
    // 栈的基本功能
    void Push(int v);   // 入栈
    int Pop();          // 出栈
    int Top();          // 返回栈顶元素
    int Size();         // 栈大小
    bool Empty();       // 栈是否为空
    // 输出栈
    void Print();
};

void Stack::Print()
{
    this->max_heap.PrintHeap();
}

void Stack::Push(int v)
{
    // 检查是否溢出
    int size = this->Size();
    max_heap.HeapInsert(size, v);
}

int Stack::Pop()
{
    return this->max_heap.HeapExtractMax().value;
}

int Stack::Top()
{
    return this->max_heap.HeapMaximum().value;
}

int Stack::Size()
{
    return this->max_heap.GetHeapSize();
}

bool Stack::Empty()
{
    if (this->max_heap.GetHeapSize() == 0)
    {
        return true;
    }
    return false;
}

int main(int argc, char const *argv[])
{
    Stack s;
    // 依次压入i^2
    for (int i = 0; i < 10; i ++)
    {
        s.Push(i*i);
    }
    // 输出栈大小
    cout << "size: " << s.Size() << endl;
    // 依次弹出i^2
    for (int i = 0; i < 10; i ++)
    {
        cout << s.Pop() << endl;
    }

    return 0;
}
