#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <queue>
#include <deque>
#include <functional>
#include <random>
#include <chrono>
#include <fstream>
#include <string>

#define RANNUMMIN 10 //랜덤으로 섞을 최소 횟수
#define RANNUMMAX 20 //랜덤으로 섞을 최대 횟수
#define MAXMOVE 60
unsigned int C = 100;
unsigned int D = 100;
int sol_num;//엑셀 출력할때 쓸 전역변수
int vtemp;//

using namespace std;


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
pn ranpuzzle(pn, int); //퍼즐을 int 만큼 랜덤으로 섞는 함수
pn ranpuzzle2(pn); // 랜덤으로 조금 더 정확하게 섞기 위해서 수정한 함수
bool solvable(pn); //solvable여부 확인 함수
void Tst(); //테스트용 함수

int main()
{
    pn node;
    char tst;
    printf("random값으로 테스트 하시겠습니까?(맞으면 Y)\n");
    scanf("%c", &tst);
    if (tst == 'Y') {
        node = ranpuzzle2(node);
    }
    else {
        for (int j = 0; j < 5; j++) {
            for (int k = 0; k < 5; k++) {
                //node.puzzle[j][k] = puzzle[j][k];
                scanf("%d", &node.puzzle[j][k]);
            }
        }
    }
    chrono::system_clock::time_point startTime = chrono::system_clock::now();
    node = solve(node);
    chrono::system_clock::time_point endTime = chrono::system_clock::now();
    print_puzzle(node);
    chrono::milliseconds millisecond = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    std::cout << "Time Cost: " << millisecond.count() << " ms\n";
    //Tst();
}

pn solve(pn tnode) {
    pn node = find_value(tnode);
    vtemp = node.value;
    node.from = 'N';
    node.num = 0;
    priority_queue<puzzle_node, vector<puzzle_node>, cmp>pq;
    if (!solvable(node)) {
        printf("-1\n"); return node;
    }
    //C = node.value *5;//
   // chrono::system_clock::time_point startTime = chrono::system_clock::now();
    int t = node.value + 100;
    unsigned int cnt = 0;
    int temp = 0;
    C = t + 10;
    D = t;
    do {
        cnt++;
        temp++;
        //chrono::system_clock::time_point endTime = chrono::system_clock::now();
        //chrono::milliseconds millisecond = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
        /*if (millisecond.count() > 60000) {
            return node;
        }*/


        if (node.value == 0) {
            return node;
        }// solution 찾았으면 탈출

        if (temp == 100000) {
            if ((cnt >> 16) <= t) {
                C = t + 10 + (cnt >> 16);
                D = t - (cnt >> 16);
                temp = 0;
            }
        }


        /* if (node.num > MAXMOVE) {
             node = pq.top();
             pq.pop();
             continue;
         }*/ //MAXMOVE보다 많은 이동을한경우 solution아님(최적의 solution과 잘못 빠지는 경우 방지)

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
        for (int j = 0; j < 5; j++) {
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

    /*for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            val = node.puzzle[i][j];
            if (i == 4 && j ==4) {
                if (val != 0) {
                    node.value++;
                }
            }
            else if (val != (i * 5 + j)+1) {
                node.value++;
            }
            if (val == 0) {
                node.empty[0] = i;
                node.empty[1] = j;
            }
        }
    }*/ //제 위치들에 있지 않는 숫자들의 상태를 value값에 집어 넣음
    node.cmpnum = node.value * C + node.num * D;// 적게 남았으면 공격적으로 탐색

    //node.cmpnum = node.value+ node.num;
    /*if (pre_value < node.value) {
        node.cmpnum *= 1.5;
    }*/
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
        node.value -= abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.value--;
        break;
    case 'R':
        val = node.puzzle[node.empty[0]][node.empty[1]];
        node.value = pre_value + abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.empty[1]--;
        node.value -= abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.value++;
        break;
    case 'U':
        val = node.puzzle[node.empty[0]][node.empty[1]];
        node.value = pre_value + abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.empty[0]++;
        node.value -= abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.value--;
        break;
    case 'D':
        val = node.puzzle[node.empty[0]][node.empty[1]];
        node.value = pre_value + abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) % 5 - node.empty[1]);
        node.empty[0]--;
        node.value -= abs((val - 1) / 5 - node.empty[0]) + abs((val - 1) %5 - node.empty[1]);
        node.value++;
        break;
    } // swap에서 변경된 두 부분의 맨하탄 거리만 다시 구함

    node.cmpnum = node.value * C + node.num * D;// 적게 남았으면 공격적으로 탐색
    return node;
}

void print_puzzle(pn node) {
    int num = node.order.size();
    sol_num = num;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", node.puzzle[i][j]);
        }
        printf("%d\n", node.puzzle[i][4]);
    }
    printf("%d회\n", num);
    /*for (int i = 0; i < num; i++) {
        printf("%c", node.order.front());
        node.order.pop_front();
    }
    printf("\n");*/
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
pn ranpuzzle(pn node, int rannum) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 3);

    for (int i = 0; i <5; i++) {
        for (int j = 0; j < 5; j++) {
            node.puzzle[i][j] = i * 5 + j + 1;
        }
    }
    node.puzzle[4][4] = 0;
    node.empty[0] = 4;
    node.empty[1] = 4;

    int k = 0;
    int i = 0;
    int prev = -1;
    while (i < rannum) {
        k = dis(gen);
        if (prev == -1) {

        }
        else if (prev % 2 == 0) {
            if (k == prev + 1) {
                continue;
            }
        }
        else {
            if (k == prev - 1) {
                continue;
            }
        }
        switch (k) {
        case 0:
            if (node.empty[0] != 0) {
                swap(node.puzzle[node.empty[0]][node.empty[1]], node.puzzle[node.empty[0] - 1][node.empty[1]]);
                node.empty[0]--;
                i++;
            }
            break;
        case 1:
            if (node.empty[0] !=4) {
                swap(node.puzzle[node.empty[0]][node.empty[1]], node.puzzle[node.empty[0] + 1][node.empty[1]]);
                node.empty[0]++;
                i++;
            }
            break;
        case 2:
            if (node.empty[1] != 0) {
                swap(node.puzzle[node.empty[0]][node.empty[1]], node.puzzle[node.empty[0]][node.empty[1] - 1]);
                node.empty[1]--;
                i++;
            }
            break;
        case 3:
            if (node.empty[1] != 4) {
                swap(node.puzzle[node.empty[0]][node.empty[1]], node.puzzle[node.empty[0]][node.empty[1] + 1]);
                node.empty[1]++;
                i++;
            }
            break;
        }
        prev = k;
    }

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", node.puzzle[i][j]);
        }
        printf("%d\n", node.puzzle[i][4]);
    }
    printf("\n");

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
        for (int j = i + 1; j < 25; j++) {
            if (node.puzzle[i / 5][i % 5] > node.puzzle[j / 5][j % 5] && node.puzzle[j / 5][j % 5] != 0) {
                inv++;
            }
        }

    }
    if (inv % 2 == 0) {
        return true;
    }
    return false;
}//풀 수있는 퍼즐인지 미리체크

pn ranpuzzle2(pn node) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(RANNUMMIN, RANNUMMAX);
    return ranpuzzle(node, dis(gen));

}

int findmin(float arr[11]) {
    int m = 0;
    for (int i = 1; i <= 10; i++) {
        if (arr[i] < arr[m]) {
            m = i;
        }
    }
    return m;
}

void Tst() {
    bool issolve;
    C = 100;
    pn tnode;
    pn temp;
    ofstream fout("Test_data.csv", ios::app);//같은 이름의 파일 존재시 이어쓰기함
    //ofstream fout(file_name, ios::out); //같은 이름의 파일 존재시 이전 파일 지우고 새로 만듬
    fout << "puzzle,msec,sol_num,C,D,value,solution" << endl;
    fout.close();
    float arr[6][11];
    float numavg = 0;
    float secavg = 0;
    while (true) {


        pn node = ranpuzzle2(tnode);
        ofstream fout("Test_data.csv", ios::app);
        C = 100;
        D = 100;
        chrono::system_clock::time_point startTime = chrono::system_clock::now();
        temp = solve(node);
        if (temp.value == 0) issolve = true;
        else issolve = false;
        chrono::system_clock::time_point endTime = chrono::system_clock::now();
        chrono::milliseconds millisecond = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
        std::cout << "Time Cost: " << millisecond.count() << " ms\n";
        if (issolve) print_puzzle(temp);

        if (issolve == false) {
            sol_num = -1;
        }

        for (int i = 0; i <5; i++) {
            for (int j = 0; j < 5; j++) {
                fout << node.puzzle[i][j] << " ";
            }
        }

        int num = temp.order.size();
        string sol = "";
        for (int i = 0; i < num; i++) {
            printf("%c", temp.order.front());
            sol += temp.order.front();
            temp.order.pop_front();
        }
        printf("\n");
        fout << "," << millisecond.count() << "," << sol_num << "," << C << "," << D << "," << vtemp << "," << sol << endl;
        fout.close();
    }



}