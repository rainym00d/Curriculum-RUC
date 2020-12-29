/*
 * @Description: 实现跳跃表
 * @Author: rainym00d
 * @Github: https://github.com/rainym00d
 * @Date: 2020-10-05 19:09:20
 * @LastEditors: rainym00d
 * @LastEditTime: 2020-10-10 18:09:42
 */



#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>

using namespace std;

// 跳跃表的最高层级
const int LEVEL = 8;


// 跳跃表的节点
template<typename T>
struct Node
{
    // 该节点的值
    T value;
    // 指向后边节点的指针
    vector<Node*> next;

    // 构造函数
    Node(T v);
};

template<typename T>
Node<T>::Node(T v): value(v)
{
    // 提前分配好储存指针的空间
    this->next.resize(LEVEL, NULL);
}

// 跳跃表
template<typename T>
class SkipList
{
private:
    // 头结点
    Node<T>* head;

public:
    // 构造函数
    SkipList() {
        // 初始化头节点
        this->head = new Node<T>(-1);
    }
    // 析构函数
    ~SkipList() {
        // 释放头节点内存
        delete head;
    }

    // 插入
    void insert(T v);
    // 查找
    bool search(T v);
    // 删除
    bool erase(T v);

private:
    // 辅助函数，寻找那些跳跃表中比v小的节点
    void find_less(T v, vector<Node<T>*> &pre);

};

template<typename T>
void SkipList<T>::find_less(T v, vector<Node<T>*> &pre) {
    // 从头节点开始
    Node<T> *p = this->head;
    // 从层级最高的指针开始找离目标最近的结点
    for (int i = LEVEL - 1; i >= 0; i --) {
        while (p->next[i] && p->next[i]->value < v) {
            p = p->next[i];
        }
        pre[i] = p;
    }
}

// 插入
template<typename T>
void SkipList<T>::insert(T v) {
    // 找到比v小的前一个节点
    vector<Node<T>*> pre(LEVEL);
    this->find_less(v, pre);
    Node<T> *obj = new Node<T>(v);
    
    // 遍历比v小的那些结点
    for (int i = 0; i < LEVEL; i ++) {
        // 让v的对应层级指向比v小的那些结点所指的结点
        obj->next[i] = pre[i]->next[i];
        // 比v小的那些结点指向v
        pre[i]->next[i] = obj;
        // 根据时间初始化随机种子，并根据随机数来决定是否继续往下设置更高层级的指针
        srand((unsigned)time(NULL));
        if (rand() % 2 == 0)
            break;
    }
    
}

// 查找
template<typename T>
bool SkipList<T>::search(T v) {
    // 找到比v小的前一个节点
    vector<Node<T>*> pre(LEVEL);
    this->find_less(v, pre);

    Node<T> *obj = pre[0]->next[0];
    // 目标需要存在且值相等
    return obj && obj->value == v;
}

// 删除
template<typename T>
bool SkipList<T>::erase(T v) {
    // 找到比v小的前一个节点
    vector<Node<T>*> pre(LEVEL);
    this->find_less(v, pre);

    Node<T> *obj = pre[0]->next[0];
    // 若不存在这样的结点
    if (!obj || obj->value != v) {
        return false;
    }
    // 将比v小的那些结点的对应层级指向v对应层级所指的结点
    for (int i = 0; i < LEVEL && pre[i]->next[i] == obj; i ++) {
        pre[i]->next[i] = obj->next[i];
    }
    // 删去目标结点
    delete obj;

    return true;
}


int main()
{
    SkipList<int> *skiplist = new SkipList<int>();

    skiplist->insert(1);
    skiplist->insert(3);
    skiplist->insert(2);
    cout << "Insert 1 then search 1" << endl;
    cout << (skiplist->search(1) ? "True" : "False") << endl;
    skiplist->erase(1);
    cout << "Erase 1 then search 1" << endl;
    cout << (skiplist->search(1) ? "True" : "False") << endl;
    // skiplist->insert(2);
    // skiplist->insert(3);
    // skiplist->search(0);   // 返回 false
    // skiplist->insert(4);
    // skiplist->search(1);   // 返回 true
    // skiplist->erase(0);    // 返回 false，0 不在跳表中
    // skiplist->erase(1);    // 返回 true
    // skiplist->search(1);   // 返回 false，1 已被擦除

    return 0;
}
