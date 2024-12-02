#pragma once
#include "Wind.h" //includes sfml_i.h
#include <vector>
#include <iterator>
#include <experimental/random>
#include <functional>
#include <chrono>

/* Move this later */
struct TextureLoad{
    std::map<std::string, sf::Texture>* ui_texture_map;
    sf::Texture *tile_texture_map;
    sf::Texture *numbers;

    TextureLoad() {
        set_ui_map();
        set_tile_map();
        set_digit_map();
    }

    ~TextureLoad() {
        delete[] tile_texture_map;
        delete ui_texture_map;
    }

    void set_ui_map() {
        ui_texture_map = new std::map<std::string, sf::Texture>;
        (*ui_texture_map)["debug"] = load_texture("./files/images/debug.png");
        (*ui_texture_map)["happy"] = load_texture("./files/images/face_happy.png");
        (*ui_texture_map)["lose"] = load_texture("./files/images/face_lose.png");
        (*ui_texture_map)["win"] = load_texture("./files/images/face_win.png");
        (*ui_texture_map)["pause"] = load_texture("./files/images/pause.png");
        (*ui_texture_map)["play"] = load_texture("./files/images/play.png");
        (*ui_texture_map)["leaderboard"] = load_texture("./files/images/leaderboard.png");
    }

    void set_digit_map() {
        numbers = new sf::Texture[11];
        sf::IntRect list_nums[11];

        for(int i = 0; i < 11; i++) {
            list_nums[i] = sf::IntRect ((i * 21), 0, 21, 32);
            numbers[i].loadFromFile("./files/images/digits.png", list_nums[i]);
        }
    }

    void set_tile_map() {
        tile_texture_map = new sf::Texture[12];

        tile_texture_map[0] = load_texture("./files/images/tile_revealed.png");
        tile_texture_map[1] = load_texture("./files/images/number_1.png");
        tile_texture_map[2] = load_texture("./files/images/number_2.png");
        tile_texture_map[3] = load_texture("./files/images/number_3.png");
        tile_texture_map[4] = load_texture("./files/images/number_4.png");
        tile_texture_map[5] = load_texture("./files/images/number_5.png");
        tile_texture_map[6] = load_texture("./files/images/number_6.png");
        tile_texture_map[7] = load_texture("./files/images/number_7.png");
        tile_texture_map[8] = load_texture("./files/images/number_8.png");
        tile_texture_map[9] = load_texture("./files/images/tile_hidden.png");
        tile_texture_map[10] = load_texture("./files/images/mine.png");
        tile_texture_map[11] = load_texture("./files/images/flag.png");
    }

private:
    static sf::Texture load_texture(const std::string& file_name) {
        sf::Texture to_return;
        to_return.create(32, 32);
        to_return.loadFromFile(file_name);

        return to_return;
    }
};

class Tile {
    sf::Texture* tile_textures;
    sf::Sprite background;
    sf::Sprite tile;
    sf::Sprite overlay;

    bool mine;
    bool m_flag = false;
    bool unveiled = false;
    bool debug_mode = false;
    int adj_mines = 0;

    std::vector<Tile*> neighboring_tiles;

public:
    Tile();
    Tile(sf::Texture* texture_list, float x, float y, bool mine = false);

    // Methods for initialization
    void setMine(bool mine);
    bool getMine() const;
    bool getFlag() const;
    bool getUnveiled() const;
    void setPos(float x, float y);
    void setTextures(sf::Texture* texture_list);


    void addNeighbor(Tile* neighbor);
    void calcAdjMines();

    bool update();
    void unveilMine();
    bool flag();
    void debug();

    void resetTexture();

    void draw(sf::RenderWindow& window);
};

class Board {
    sf::RenderWindow game_window;
    std::map<std::string, sf::Texture>* ui_texture_list;
    sf::Texture* tile_texture_list;
    sf::Texture* digit_texture_list;

    std::map<std::string, sf::Sprite> buttons;

    std::vector<sf::Sprite> mines;
    std::vector<sf::Sprite> minutes;
    std::vector<sf::Sprite> seconds;

    Tile** board_tiles = nullptr;
    sf::Sprite** paused_tiles = nullptr;

    unsigned int rows ;
    unsigned int columns ;
    unsigned int mine_num;
    int mine_count;

    std::string game_state;
    std::string time_state;

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::chrono::nanoseconds time_buffer = std::chrono::nanoseconds(0);

    void gen_mines();
    void adjacent_tile_collection();
    void updateMineCount(bool direction);
    void updateTimer();

    void pausePlay();
    void reset();
public:
    Board(std::map<std::string, sf::Texture>* ui_texture_list, sf::Texture* texture_list, sf::Texture* digit_texture_list, unsigned int columns, unsigned int rows, unsigned int mines);
    ~Board();

    void setTiles();
    void setDebug();

    void setup();
    void draw();
    void game();
    void check_lose(bool mine);
    void check_win();



};
