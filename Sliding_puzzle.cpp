#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <queue>
#include <deque>
#include <functional>
#include <random>
#include <chrono>

unsigned int C;
unsigned int D;

using namespace std;

int puzzle[5][5] = {
    {1,2,3,4,5},
    {6,7,8,9,10},
    {11,12,13,14,15},
    {16,17,18,19,20},
    {21,22,23,24,0}
};

struct puzzle_node {
    int puzzle[5][5]; //puzzle의 상태
    int empty[2]; //0의 위치
    int value; //A*알고리즘 적용을 위한 값
    char from; //이전 0의 위치
    deque<char> order;//solution 저장용 queue
    int num; //이때까지 이동한 횟수
    int cmpnum;
} typedef pn;

struct cmp {
    bool operator()(pn a, pn b) {
        return a.cmpnum > b.cmpnum;
    }
};//value값이 작은 것이 우선순위 큐의 top에 오도록

pn solve(pn); //문제 푸는 함수
pn find_value(pn); //value값을 찾는 함수
pn find_value2(pn, char); //value값을 조금 더 빠르게 찾기 위해 변형한 함수
void print_puzzle(pn); //puzzle 출력함수
pn add_node(pn, char); //우선순위 큐에 다음 노드를 넣기 위한 함수
bool solvable(pn); //solvable여부 확인 함수

int main()
{
    pn node;

    for (int j = 0; j < 5; j++) {
        for (int k = 0; k < 5; k++) {
            node.puzzle[j][k] = puzzle[j][k];
            //scanf("%d", &node.puzzle[j][k]);
        }
    }

    chrono::system_clock::time_point startTime = chrono::system_clock::now();
    node = solve(node);
    chrono::system_clock::time_point endTime = chrono::system_clock::now();

    if (node.num != 0) { print_puzzle(node); }
    chrono::milliseconds millisecond = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    std::cout << "Time Cost: " << millisecond.count() << " ms\n";
}

pn solve(pn tnode) {
    pn node = find_value(tnode);
    node.from = 'N';
    node.num = 0;
    priority_queue<puzzle_node, vector<puzzle_node>, cmp>pq;

    if (!solvable(node)) {
        printf("-1\n");
        return node;
    }
    int t = node.value + 100;
    unsigned int cnt = 0;
    int temp = 0;
    C = t + 10;
    D = t;

    do {
        cnt++;
        temp++;

        if (node.value == 0) {
            return node;
        }// solution 찾았으면 탈출

        if (temp == 100000) {

            if ((cnt >> 15) <= t) {
                C = t + 10 + (cnt >> 16);
                D = t - (cnt >> 16);
                temp = 0;
            }
        }

        if (node.empty[0] != 4 && node.from != 'D') {
            node.order.push_back('D');
            pq.push(add_node(node, 'D'));
            node.order.pop_back();
        }
        if (node.empty[0] != 0 && node.from != 'U') {
            node.order.push_back('U');
            pq.push(add_node(node, 'U'));
            node.order.pop_back();
        }
        if (node.empty[1] != 4 && node.from != 'R') {
            node.order.push_back('R');
            pq.push(add_node(node, 'R'));
            node.order.pop_back();
        }
        if (node.empty[1] != 0 && node.from != 'L') {
            node.order.push_back('L');
            pq.push(add_node(node, 'L'));
            node.order.pop_back();
        }

        node = pq.top();
        pq.pop();

    } while (!pq.empty());

    printf("-1\n");
    return node;
}
pn find_value(pn node) {
    int pre_value = node.value;
    node.value = 0;
    int val;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j <5; j++) {
            val = node.puzzle[i][j];
            if (val == 0) {
                node.value += 8 - i - j;
                node.empty[0] = i;
                node.empty[1] = j;

            }
            else {
                node.value += abs((val - 1) /5 - i) + abs((val - 1) % 5 - j);
            }
        }
    } //맨하탄거리 이용
    node.cmpnum = node.value * C + node.num * D;// 적게 남았으면 공격적으로 탐색

    return node;
}

pn find_value2(pn node, char from) {
    int pre_value = node.value;
    node.value = 0;
    int val;

    switch (from) {
    case 'L':
        val = node.puzzle[node.empty[0]][node.empty[1]];
        node.value = pre_value + abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.empty[1]++;
        node.value -= abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) %5 - node.empty[1]);
        node.value--;
        break;
    case 'R':
        val = node.puzzle[node.empty[0]][node.empty[1]];
        node.value = pre_value + abs((val - 1) /5- node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.empty[1]--;
        node.value -= abs((val - 1) / 5- node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.value++;
        break;
    case 'U':
        val = node.puzzle[node.empty[0]][node.empty[1]];
        node.value = pre_value + abs((val - 1) /5 - node.empty[0]) + abs((val - 1) %5 - node.empty[1]);
        node.empty[0]++;
        node.value -= abs((val - 1) / 5- node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.value--;
        break;
    case 'D':
        val = node.puzzle[node.empty[0]][node.empty[1]];
        node.value = pre_value + abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.empty[0]--;
        node.value -= abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.value++;
        break;
    } // swap에서 변경된 두 부분의 맨하탄 거리만 다시 구함

    node.cmpnum = node.value * C + node.num * D;// 적게 남았으면 공격적으로 탐색
    return node;
}

void print_puzzle(pn node) {
    int num = node.order.size();

    printf("%d회\n", num);
    for (int i = 0; i < num; i++) {
        printf("%c", node.order.front());
        node.order.pop_front();
    }
    printf("\n");
}
pn add_node(pn node, char to) {
    node.num++;
    switch (to) {
    case 'U':
        node.from = 'D';
        swap(node.puzzle[node.empty[0]][node.empty[1]], node.puzzle[node.empty[0] - 1][node.empty[1]]);
        break;
    case 'D':
        node.from = 'U';
        swap(node.puzzle[node.empty[0]][node.empty[1]], node.puzzle[node.empty[0] + 1][node.empty[1]]);
        break;
    case 'L':
        node.from = 'R';
        swap(node.puzzle[node.empty[0]][node.empty[1]], node.puzzle[node.empty[0]][node.empty[1] - 1]);
        break;
    case 'R':
        node.from = 'L';
        swap(node.puzzle[node.empty[0]][node.empty[1]], node.puzzle[node.empty[0]][node.empty[1] + 1]);
        break;
    }
    node = find_value2(node, node.from);
    return node;
}

bool solvable(pn node) {
    int inv = 0;
    for (int i = 0; i <5; i++) {
        for (int j = 0; j < 5; j++) {
            if (node.puzzle[i][j] == 0 && i % 2 == 1) {
                inv++;
                break;
            }
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = i + 1; j <25; j++) {
            if (node.puzzle[i /5][i % 5] > node.puzzle[j / 5][j % 5] && node.puzzle[j / 5][j % 5] != 0) {
                inv++;
            }
        }
    }
    if (inv % 2 == 0) {
        return true;
    }
    return false;
}//풀 수있는 퍼즐인지 미리체크