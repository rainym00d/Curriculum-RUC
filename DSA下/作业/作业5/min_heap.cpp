/*
 * @Description: 算法导论第6章min heap及priority queue实现
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-10-14 10:49:19
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-10-17 15:40:56
 */

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;


const int MAX_LENGTH = 128;     // 堆数组的最大长度
const int INF = -233333333;     // 定义最小的下界 
const int SUP = 233333333;      // 定义最大的上界

// 最小堆
class MinHeap
{
private:
    int *A;
    int length;     // 堆数组总长度
    int heap_size;  // 堆数组当前长度

public:
    MinHeap(int length = MAX_LENGTH);
    ~MinHeap();
    // 最小堆的有关代码实现参考算法导论第六章最大堆的实现
    // heap
    void MinHeapify(int i); // 维护最小堆
    void BuildMinHeap();    // 构建最小堆
    void HeapSort();        // 堆排序
    // priority queue
    int HeapMinimum();      // 返回最小堆顶，即最小的数
    int HeapExtractMin();   // 取出堆顶
    void HeapDecreaseKey(int index, int key);   // 减小堆中某个index的值
    void HeapInsert(int key);   // 插入堆
    // 打印堆数组
    void PrintHeap();

private:
    inline int PARENT(int i);
    inline int LEFT(int i);
    inline int RIGHT(int i);
};
// 构造函数里初始随机生成数据，方便测试，也可以注释掉自己加入数据
MinHeap::MinHeap(int length)
    : length(length), heap_size(length)
{
    this->A = new int[length];
    // 生成不同的随机数据
    char tmp[length];  
    memset(tmp, 0, sizeof(tmp));// 初始化为0
    // 随机种子随时间变化
    srand((unsigned)time(NULL));
    for (int i = 0; i < length; i ++)
    {   
        while (true) 
        {
            int num = rand() % length;
            if (tmp[num] == 0)
            {
                tmp[num] = 1;
                this->A[i] = num;
                break;
            }
        }
    }
    
}

MinHeap::~MinHeap()
{
    delete this->A;
}

void MinHeap::MinHeapify(int i)
{
    int smallest = i;
    int l = this->LEFT(i);
    int r = this->RIGHT(i);
    if (l < this->heap_size && this->A[l] < this->A[smallest]) 
    {
        smallest = l;
    }
    if (r < this->heap_size && this->A[r] < this->A[smallest]) 
    {
        smallest = r;
    }
    if (smallest != i) 
    {
        swap(this->A[i], this->A[smallest]);
        MinHeapify(smallest);
    }
}

void MinHeap::BuildMinHeap()
{
    for (int i = (this->heap_size >> 1) - 1; i >= 0; i --) 
    {
        this->MinHeapify(i);
    }
}

void MinHeap::HeapSort() 
{
    this->BuildMinHeap();
    for (int i = this->length - 1; i >= 0; i --) 
    {
        swap(this->A[0], this->A[i]);
        this->heap_size --;
        this->MinHeapify(0);
    }
}

int MinHeap::HeapMinimum()
{
    return this->A[0];
}

int MinHeap::HeapExtractMin()
{
    if (this->heap_size <= 0)
    {
        cout << "[ERROR] Underflow!" << endl;
        return -1;
    }
    int min = this->A[0];
    this->A[0] = this->A[this->heap_size - 1];
    this->heap_size --;
    this->MinHeapify(0);
    return min;
}

void MinHeap::HeapDecreaseKey(int index, int key)
{
    if (index < 0 || index >= this->heap_size)
    {
        cout << "[ERROR] Index out of bounds!" << endl;
        return;
    }
    if (this->A[index] < key)
    {
        cout << "[ERROR] New key is bigger than current key!" << endl;
        return;
    }
    this->A[index] = key;
    
    int i = index;
    while (i > 0 && this->A[this->PARENT(i)] > this->A[i])
    {
        swap(this->A[this->PARENT(i)], this->A[i]);
        i = this->PARENT(i);
    }
}

void MinHeap::HeapInsert(int key)
{
    if (this->heap_size >= this->length)
    {
        cout << "[ERROR] Overflow!" << endl;
        return;
    }
    this->heap_size ++;
    this->A[this->heap_size - 1] = SUP;
    this->HeapDecreaseKey(this->heap_size - 1, key);
}

void MinHeap::PrintHeap()
{
    cout << "length is " << this->length << "\nheap_size is " << this->heap_size << endl;
    for (int i = 0; i < this->length; i ++)
    {
        cout << this->A[i] << " ";
    }
    cout << endl;
}

inline int MinHeap::PARENT(int i)
{
    return ((i + 1) >> 1) - 1;
}

inline int MinHeap::LEFT(int i)
{
    return ((i + 1) << 1) - 1;
}

inline int MinHeap::RIGHT(int i)
{
    return ((i + 1) << 1);
}

int main(int argc, char const *argv[])
{
    MinHeap min_heap(20);
    cout << "初始：" << endl;
    min_heap.PrintHeap();
    min_heap.BuildMinHeap();
    cout << "建堆：" << endl;
    min_heap.PrintHeap();
    
    cout << "ExtractMin: " << min_heap.HeapExtractMin() << endl;
    cout << "取出：" << endl;
    min_heap.PrintHeap();

    min_heap.HeapInsert(233);
    cout << "插入：" << endl;
    min_heap.PrintHeap();
    
    min_heap.HeapDecreaseKey(19, -2);
    cout << "减少：" << endl;
    min_heap.PrintHeap();

    min_heap.HeapSort();
    cout << "排序：" << endl;
    min_heap.PrintHeap();
    return 0;
}
