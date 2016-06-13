#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <sstream>

#include <boost/python.hpp>

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

    string tostr() {
        stringstream s;
        for (int i = 0; i < 12; ++i) {
            for (int j = 0; j < 6; ++j)
                s << d[j][i];
            s << '\n';
        }
        return s.str();
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

    void emptyit() {
        for (int x = 0; x < 6; ++x) {
            for (int y = 0; y < 12; ++y) {
                d[x][y] = '.';
            }
        }
    }
};

int dw[] = {1, 0, -1, 0};
struct Game {
    GameMap map1, map2;
    int score1 = 0, score2 = 0;
    int nuisance1 = 0, nuisance2 = 0;

    int colorsA[8];
    int colorsB[8];

    bool game_is_over = false;
    int result = 0;
    int turn = 0;

    Game() {
        map1.emptyit();
        map2.emptyit();
        for (int i = 0; i < 8; ++i) {
            colorsA[i] = (rand() % 5) + 1;
            colorsB[i] = (rand() % 5) + 1;
        }
    }

    void process_turn(int p1_col, int p1_rot, int p2_col, int p2_rot) {
        bool p1_fail = false;
        bool p2_fail = false;

        ++turn;
        
        if (p1_rot != 3) {
            map1.putOne(p1_col, colorsA[0]);
            map1.putOne(p1_col + dw[p1_rot], colorsB[0]);
        } else {
            map1.putOne(p1_col, colorsB[0]);
            map1.putOne(p1_col, colorsA[0]);
        }

        if (map1.failed)
            p1_fail = true;
        else
            score1 += map1.doClear();

        if (p2_rot != 3) {
            map2.putOne(p2_col, colorsA[0]);
            map2.putOne(p2_col + dw[p2_rot], colorsB[0]);
        } else {
            map2.putOne(p2_col, colorsB[0]);
            map2.putOne(p2_col, colorsA[0]);
        }

        if (map2.failed)
            p2_fail = true;
        else
            score2 += map2.doClear();

        if (p1_fail || p2_fail || turn > 200) {
            game_is_over = true;
            if (p1_fail && p2_fail)
                result = 0;
            else if (p1_fail)
                result = 2;
            else if (p2_fail)
                result = 1;
            else {
                if (score1 == score2) result = 0;
                else
                    result = score1 < score2 ? 2 : 1;
            }

            return;
        }

        // block 0
        while (nuisance1 + 420 < score1) {
            for (int i = 0; i < 6; ++i) {
                map2.putOne(i, 0);
                map2.failed = false;
            }
            nuisance1 += 420;                 
        }

        while (nuisance2 + 420 < score2) {
            for (int i = 0; i < 6; ++i) {
                map1.putOne(i, 0);
                map1.failed = false;
            }
            nuisance2 += 420;                 
        }

        for (int i = 0; i < 7; ++i) {
            colorsA[i] = colorsA[i + 1];
            colorsB[i] = colorsB[i + 1];
        }
        colorsA[7] = rand() % 5 + 1;
        colorsB[7] = rand() % 5 + 1;
    }
    
    std::string get_map_str(int mapid) {
        if (mapid == 1)
            return map1.tostr();
        else
            return map2.tostr();
    }

    int get_color(int id, int idx) {
        if (id == 0)
            return colorsA[idx];
        else
            return colorsB[idx];
    }
};

BOOST_PYTHON_MODULE(game)
{
    boost::python::class_<Game>("Game")
        .def("process_turn", &Game::process_turn)
        .def("get_map_str", &Game::get_map_str)
        .def("get_color", &Game::get_color)
        .def_readonly("score1", &Game::score1)
        .def_readonly("score2", &Game::score2)
        .def_readonly("gameover", &Game::game_is_over)
        .def_readonly("result", &Game::result)
        ;
}
