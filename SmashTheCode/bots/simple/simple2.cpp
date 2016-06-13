// another simple bot with dfs, use different value function
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>

using namespace std;

int mx[] = {0, 0, -1, 1};
int my[] = {-1, 1, 0, 0};
const int mn = 4;

struct GameMap {
    char d[6][12];
    char v[6][12];
    bool failed = false;

    void read() {
        for (int i = 0; i < 12; ++i)
            for (int j = 0; j < 6; ++j)
                d[j][i] = read_next_char();
    }

    void loads(const char** b) {
        for (int i = 0; i < 12; ++i)
            for (int j = 0; j < 6; ++j)
                d[j][i] = b[i][j];
    }

    void print() {
        for (int i = 0; i < 12; ++i) {
            for (int j = 0; j < 6; ++j)
                putchar(d[j][i]);
            putchar('\n');
        }
    }



    int read_next_char() {
        char ch;
        do {
            cin >> ch;
        } while (ch != '.' and (ch < '0' or ch > '5'));
        return ch;
    }

    void putOne(int col, int color) {
        int x = col;
        int y = 0;

        if (d[x][y] != '.') {
            failed = true;
            return;
        }

        while (y < 11 && d[x][y+1] == '.') ++y;
        d[x][y] = color + '0';
    }


    int doClear(int chainPower = 0) { // return scores
        memset(v, 0, sizeof(v));
        int groupBonus = 0;
        int vid = 0;

        int colorsCleared[6] = {0};
        int B = 0;

        bool hasClears = false;

        // Mark Clears
        for (int x = 0; x < 6; ++x) {
            for (int y = 0; y < 12; ++y) {
                if (d[x][y] != '.' && d[x][y] != '0' && v[x][y] == 0) {
                    vid += 1;
                    int num = dfs(x, y, d[x][y], vid);
                    if (num >= 4) {
                        groupBonus += calcGB(num);
                        setClear(vid);

                        colorsCleared[d[x][y] - '0'] = 1;
                        hasClears = true;
                    }
                }
            }
        }

        if (!hasClears)
            return 0;

        int color_num = 0;
        for (auto x: colorsCleared) color_num += x;

        // Do clean
        for (int x = 0; x < 6; ++x)
            for (int y = 0; y < 12; ++y)
                if (v[x][y] == -1) {
                    B += 1;
                    d[x][y] = '.';
                } else if (d[x][y] == '0') {
                    bool rem = false;
                    for (int idx = 0; idx < mn; ++idx) {
                        int nx = x + mx[idx];
                        int ny = y + my[idx];
                        if (inboard(nx, ny) && v[nx][ny] == -1) {
                            rem = true;
                            break;
                        }
                    }

                    if (rem) {
                        B += 1;
                        d[x][y] = '.';
                    }
                }

        // Do drop
        for (int x = 0; x < 6; ++x) {
            int nid = 11;
            for (int y = 11; y >= 0; --y)
                if (d[x][y] != '.') {
                    if (y != nid) {
                        d[x][nid] = d[x][y];
                        d[x][y] = '.';
                    }
                    nid -= 1;
                }
        }

        int sum = chainPower + groupBonus + colorBonus(color_num);
        if (sum < 1) sum = 1;
        if (sum > 999) sum = 999;

        int stepScore = 10 * B * sum;

        int nextChainPoint = chainPower * 2;
        if (nextChainPoint == 0)
            nextChainPoint = 8;

        return stepScore + doClear(nextChainPoint);
    }

    int dfs(int x, int y, int color, int vid) {
        if (!inboard(x, y) || v[x][y] || d[x][y] != color) {
            return 0;
        }

        int sum = 1;
        v[x][y] = vid;
        for (int idx = 0; idx < mn; ++idx)
            sum += dfs(x + mx[idx], y + my[idx], color, vid);
        return sum;
    }

    static int inboard(int x, int y) {
        return (x > 0) && (x < 6) && (y > 0) && (y < 12);
    }

    static int calcGB(int num) {
        if (num <= 10) {
            return num - 4;
        }
        return 8;
    }

    static int colorBonus(int num) {
        if (num == 1) return 0;
        return 1 << (num - 1);
    }

    void setClear(int vid) {
        for (int x = 0; x < 6; ++x) {
            for (int y = 0; y < 12; ++y) {
                if (v[x][y] == vid)
                    v[x][y] = -1;
            }
        }
    }
};

pair<int, int> ans;

pair<int, int> first_stack;

int best_score;
int search_dep = 3;

int wmin[] = {0, 0, 1, 0};
int wmax[] = {4, 5, 5, 5};
int dw[] = {1, 0, -1, 0};

int dep_penity = 10;

float rates[] = {1, 0.8, 0.64, 0.5};

bool eu = false;

void search(int dep, const std::vector<pair<int,int>>& colors, const GameMap& map, int pre_score) {

    if (dep > 0) {
        int test_value = pre_score;
        if (test_value > best_score) {
            best_score = test_value;
            ans = first_stack;
        }
    }

    for (int rot = 0; rot < 4; ++rot) {
        if (colors[dep].first == colors[dep].second && rot >= 2)
            break;
        for (int w = wmin[rot], wm = wmax[rot]; w <= wm; ++w) {
            GameMap nm = map;
            if (rot != 3) {
                nm.putOne(w, colors[dep].first);
                nm.putOne(w + dw[rot], colors[dep].second);
            } else {
                nm.putOne(w, colors[dep].second);
                nm.putOne(w, colors[dep].first);
            }

            if (nm.failed) continue;

            if (dep == 0) {
                first_stack = make_pair(w, rot);
            }

            int score = nm.doClear();
            if (dep < search_dep - 1)
                search(dep + 1, colors, nm, score * rates[dep] + pre_score);
        }
    }
}

pair<int, int> handle(std::vector<pair<int,int>>& colors, const GameMap& self, const GameMap& enemy, int myScore, int enemyScore) {
    best_score = -999;

    search(0, colors, self, 0);
    return ans;
}

#ifndef TEST_CODE
int main()
{

    GameMap map1, map2;

    // game loop
    while (1) {
        std::vector<pair<int,int>> colors;
        for (int i = 0; i < 8; i++) {
            int colorA; // color of the first block
            int colorB; // color of the attached block
            cin >> colorA >> colorB; cin.ignore();

            colors.push_back(make_pair(colorA, colorB));
        }
        int score1;
        cin >> score1; cin.ignore();
        map1.read();

        int score2;
        cin >> score2; cin.ignore();
        map2.read();

        auto s = handle(colors, map1, map2, score1, score2);
        cout << s.first << " " << s.second << endl;
    }
}
#endif
