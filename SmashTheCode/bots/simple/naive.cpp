#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{

    // game loop
    while (1) {
        std::vector<int> colors;
        for (int i = 0; i < 8; i++) {
            int colorA; // color of the first block
            int colorB; // color of the attached block
            cin >> colorA >> colorB; cin.ignore();

            colors.push_back(colorA);
        }
        int score1;
        cin >> score1; cin.ignore();
        for (int i = 0; i < 12; i++) {
            string row;
            cin >> row; cin.ignore();
        }
        int score2;
        cin >> score2; cin.ignore();
        for (int i = 0; i < 12; i++) {
            string row; // One line of the map ('.' = empty, '0' = skull block, '1' to '5' = colored block)
            cin >> row; cin.ignore();
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << colors[0] << " " << 1 << endl; // "x": the column in which to drop your blocks
    }
}
