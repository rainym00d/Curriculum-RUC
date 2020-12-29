# 数据结构与算法 作业8

<center>姓名：邵宁录&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp学号：2018202195</center>

## 15.5-4

书上源代码如下所示
<img src="pic/截屏2020-11-05%2010.10.23.png">

只需将第10行

~~~basic
for r to i
~~~

变为

~~~basic
for root[i, j - 1] to root[i + 1, j]
~~~

证明如下：

可知，第2行到第4行的时间复杂度为 $\Omicron(2n)$ ，为线性。

因此主要分析的部分在第5行到第14行。

在改变第10行，它的复杂度如下，由于单条语句的复杂度为常数，因此只用 $1$ 来表示：

$$\begin{aligned}
        T(n)&=\sum^{n}_{l=1}\sum^{n-l+1}_{i=1}(root[i+1,\ j]-root[i,\ j-1]+1)
        \\&=\sum^{n}_{l=1}(root[1+1,\ 1+l-1]-root[1,\ 1+l-2]
        \\&\qquad+root[2+1,\ 2+l-1]-root[2,\ 2+l-2]
        +\cdots
        \\&\qquad+root[n-l+1+1,\ n-l+1+l-1]-root[n-l+1,\ n-l+1+l-1-1]
        \\&\qquad+n-l+1)
        \\&=\sum^{n}_{l=1}(root[2,\ l]-root[1,\ l-1]\\&\qquad+root[3,\ l+1]-root[2,\ l]+\cdots
        \\&\qquad+root[n-l+2,\ n]-root[n-l+1,\ n-1]
        \\&\qquad+n-l+1)
        \\&=\sum^{n}_{l=1}(root[n-l+2,\ n]-root[1,\ l-1]+n-l+1)
\end{aligned}$$

由于 $root[n-l+2,\ n]-root[1,\ l-1]\le n$ ，所以 $(root[n-l+2,\ n]-root[1,\ l-1]+n-l+1)$ 该部分为 $\Omicron(n)$

即上式可写为：

$$\begin{aligned}
    T(n)&=\sum^{n}_{l=1}(root[n-l+2,\ n]-root[1,\ l-1]+n-l+1)
    \\&\le \sum^{n}_{l=1}\Omicron(n)
    \\&=\Omicron(n^2)
\end{aligned}$$

即证

## 15-11

### 1. 问题分析

首先明确本题的目标：最小化库存成本

以 $cost[i,\ j]$ 表示第 $i$ 个月，累计生产了 $j$ 台设备。显然该动态规划问题的答案为求解 $cost[n,\ D]$ 的值。有状态转移方程如下：



$$\begin{aligned}
    cost[i,\ j]=\begin{cases}
        cost[i-1,\ j-k]+h(j-D_i), &k\le m
        \\cost[i-1,\ j-k]+c(j-k)+h(j-D_i),\qquad &k\gt m
    \end{cases}
\end{aligned}$$

其中，$k$ 为这个月生产的量，$D_i$ 表示 前 $i$ 个月的总需求。

根据题意，要满足以下限制条件：

- 当第 $i$ 个月时，有 $D_i\le j\le D_n$ 。
- $D_{i-1}\le j-k$ ，即 $0\le k\le j-D_{i-1}$

综上所述，有：

$$\begin{aligned}
    cost[i,\ j] = \begin{cases}
        cost[1,\ j], &i=1
        \\\min_{k}(cost[i,\ j]), \qquad&i\not ={1}
    \end{cases}
\end{aligned}$$

其中，由于第一个月一定要出完货，所以有：

$$\begin{aligned}
    cost[1,\ j] = \begin{cases}
        h(j-D_1), &j\le m
        \\c(j-m)+h(j-D_1), \qquad&j\gt m
    \end{cases}
\end{aligned}$$

### 2. 伪代码

~~~
// d 为每个月的生产要求
// m 为固定员工的生产力
// c 额外雇佣生产每件产品的花费 
F(d, m, c):
    n = d.length()
    let D[1...n] be a new array
    D[1] = d[1]
    for i = 2 to n:
        D[i] = D[i - 1] + d[i]
    let cost[1...n, 0...D[n]] and p[1...n, 0...D[n]] be new tables
    for i = 1 to n:
        for j = D[i] to D[n]:
            if i == 1:
                cost[i, j] = h(j-D[i])
                if j > m:
                    cost[i, j] += c*(j - m)
            else:
                cost[i, j] = INFINITY
                for k = 0 to (j - D[i - 1]):
                    t = cost[i - 1, j - k] + h(j - D[i])
                    if k > m:
                        t += c*(k - m)
                    if t < cost[i, j]:
                        cost[i, j] = t
                        p[i, j] = k
    return cost and p
~~~

### 3. 复杂度分析

由于一共有 $nD$ 个子问题，且求解子问题的时候，要迭代 $\Omicron(D)$ 次，因此最后的时间复杂度为 $\Omicron(nD^2)$ 。
