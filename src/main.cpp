#include "Tiles.h" // Includes Wind.h and sfml_i.h
#include <iostream>
using namespace std;
using namespace sf;

using namespace std::chrono;
int main() {
    unsigned int columns = 0;
    unsigned int rows = 0;
    unsigned int mine_num = 0;

    try {
        std::ifstream config("./files/board_config.cfg");
        config >> columns;
        config >> rows;
        config >> mine_num;
        config.close();
    }
    catch(...) {
        cout << "Error: most likely due to paths";
    }

    columns = (columns < 22) ? 22 : columns;
    rows = (columns < 16) ? 16 : rows;
    mine_num = (mine_num >= columns * rows || mine_num == 0) ? 50 : mine_num;

    bool passed_welcome = false;

    try {
        Welcome welcome_win(columns, rows);
        passed_welcome = welcome_win();
    }
    catch(std::runtime_error& err) {
        cout << err.what() << endl;

        cout << "I have noticed a problem with paths that may cause a problem. If this persists, check the paths, they may need to be set to absolute (for some reason)" << endl;
    }

    if(passed_welcome) {
        try {
            TextureLoad texture;
            Board game_board(texture.ui_texture_map, texture.tile_texture_map, texture.numbers, columns, rows,
                             mine_num);

            game_board.game();
        }
        catch (...) {
            cout << "Error: most likely due to paths" << endl;
        }
    }

    return 0;
}