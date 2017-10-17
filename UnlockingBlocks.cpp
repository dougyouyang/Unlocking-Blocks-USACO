//  Copyright © 2017 Dougy Ouyang. All rights reserved.

#include <iostream>
#include <set>
#include <queue>
#include <stdlib.h>
#include <cmath>

using namespace std;

struct point {
    int x, y;
};
struct block {
    point loc, dim;
    int body[20][20];
} blocks[3];
void initBlocks()
{
    for (int i=0; i<3; i++)
        for (int j=0; j<20; j++)
            for (int k=0; k<20; k++)
                blocks[i].body[j][k] = 0;
}
void processBlocks()
{
    int i, j, k, bx, ex, by, ey;
    for (i=0; i<3; i++) {
        bx = 20;
        ex = 0;
        by = 20;
        ey = 0;
        for (j=0; j<10; j++) {
            for (k=0; k<10; k++) {
                if (blocks[i].body[j][k]) {
                    if (bx > j) bx = j;
                    if (ex < j) ex = j;
                    if (by > k) by = k;
                    if (ey < k) ey = k;
                }
            }
        }
        blocks[i].loc.x = bx;
        blocks[i].loc.y = by;
        blocks[i].dim.x = ex - bx + 1;
        blocks[i].dim.y = ey - by + 1;
        for (j=bx; j<20; j++)
            for (k=by; k<20; k++)
                blocks[i].body[j-bx][k-by] = blocks[i].body[j][k];
    }
}
bool conflicts[4][30][30];
void initConflicts(int b1, int b2)
{
    int i, j, m, n, add[2][30][30], sum[30][30];
    for (i=0; i<30; i++)
        for (j=0; j<30; j++)
            conflicts[b1+b2][i][j] = false;
    for (i=0; i<30; i++) {
        for (j=0; j<30; j++) {
            add[0][i][j] = 0;
            add[1][i][j] = 0;
        }
    }
    for (i=0; i<10; i++) {
        for (j=0; j<10; j++) {
            add[0][10+i][10+j] = blocks[b1].body[i][j];
        }
    }
    for (m=-10; m<10; m++) {
        for (n=-10; n<10; n++) {
            for (i=0; i<30; i++) {
                for (j=0; j<30; j++) {
                    add[1][i][j] = 0;
                    sum[i][j] = 0;
                }
            }
            for (i=0; i<10; i++) {
                for (j=0; j<10; j++) {
                    add[1][10+m+i][10+n+j] = blocks[b2].body[i][j];
                }
            }
            for (i=0; i<30; i++) {
                for (j=0; j<30; j++) {
                    sum[i][j] = add[0][i][j] + add[1][i][j];
                    if (sum[i][j] > 1) {
                        conflicts[b1+b2][10+m][10+n] = true;
                        // cout << b1 << b2 << " conflit at " << m << " " << n << endl;
                    }
                }
            }
        }
    }
}
void inputData()
{
    int i, j, x, y, nums[3];
    cin >> nums[0] >> nums[1] >> nums[2];
    for (i=0; i<3; i++) {
        for (j=0; j<nums[i]; j++) {
            cin >> x >> y;
            blocks[i].body[x][y] = 1;
        }
    }
}
struct config {
    int steps;
    point pt[3];
};
set<long long int> visited;
queue<config> q;
long long int index(config cf)
{
    long long int iii=0, base = 1;
    iii += base * (cf.pt[1].x - cf.pt[0].x + 20);
    base *= 40;
    iii += base * (cf.pt[1].y - cf.pt[0].y + 20);
    base *= 40;
    iii += base * (cf.pt[2].x - cf.pt[0].x + 20);
    base *= 40;
    iii += base * (cf.pt[2].y - cf.pt[0].y + 20);
    base *= 40;
    iii += base * (cf.pt[2].x - cf.pt[1].x + 20);
    base *= 40;
    iii += base * (cf.pt[2].y - cf.pt[1].y + 20);
    return iii;
}
bool checkConfig(config cf)
{
    int i, j;
    bool result[4] = {false, false, false, false};
    for (i=0; i<2; i++) {
        for (j=i+1; j<3; j++) {
            if (cf.pt[i].x+blocks[i].dim.x <= cf.pt[j].x) {
                result[i+j] = true;
            }
            if (cf.pt[i].y+blocks[i].dim.y <= cf.pt[j].y) {
                result[i+j] = true;
            }
            if (cf.pt[j].x+blocks[j].dim.x <= cf.pt[i].x) {
                result[i+j] = true;
            }
            if (cf.pt[j].y+blocks[j].dim.y <= cf.pt[i].y) {
                result[i+j] = true;
            }
        }
    }
    return (result[1] && result[2] && result[3]);
}
void blockMove(config cf0, config &cf, int dir)
{
    int i, j, k, d;
    cf = cf0;
    i = dir / 4;
    d = dir % 4;
    j = 0;
    k = 0;
    switch (d) {
    case 0:
        j = -1;
        break;
    case 1:
        j = 1;
        break;
    case 2:
        k = -1;
        break;
    case 3:
        k = 1;
        break;
    }
    cf.pt[i].x += j;
    cf.pt[i].y += k;
}
bool isValid(config cf)
{
    int i, j;
    for (i=0; i<2; i++) {
        for (j=i+1; j<3; j++) {
            if (abs(cf.pt[i].x - cf.pt[j].x > 20))
                return false;
            if (abs(cf.pt[i].y - cf.pt[j].y > 20))
                return false;
        }
    }
    for (i=0; i<2; i++) {
        for (j=i+1; j<3; j++) {
            if (abs(cf.pt[i].x-cf.pt[j].x)<10 && abs(cf.pt[i].y-cf.pt[j].y)<10) {
                if (conflicts[i+j][cf.pt[j].x-cf.pt[i].x+10][cf.pt[j].y-cf.pt[i].y+10])
                    return false;
            }
        }
    }
    return true;
}
int main()
{
    long long int ilong;
    int i, result = -1;
    config cf0, cf;
    initBlocks();
    inputData();
    processBlocks();
    initConflicts(0, 1);
    initConflicts(0, 2);
    initConflicts(1, 2);
    for (i=0; i<3; i++) {
       cf.pt[i].x = blocks[i].loc.x;
       cf.pt[i].y = blocks[i].loc.y;
    }
    cf.steps = 0;
    q.push(cf);
    ilong = index(cf);
    visited.insert(ilong);
    int cnt = 0;
    while (!q.empty()) {
        cf0 = q.front();
        q.pop();
        if (checkConfig(cf0)) {
            result = cf0.steps;
            break;
        }
        for (i=0; i<12; i++) {
            blockMove(cf0, cf, i);
            ilong = index(cf);
            if (visited.find(ilong) == visited.end()) {
                visited.insert(ilong);
                if (isValid(cf)) {
                    cf.steps = cf0.steps + 1;
                    q.push(cf);
                }
            }
        }
    }
    
    cout << result << endl;
    
    return 0;
}
