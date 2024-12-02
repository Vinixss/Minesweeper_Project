#include "Tiles.h" //Includes sfml_i.h


/*--------------------------------------------------------
 * Tile Methods
 *------------------------------------------------------*/

Tile::Tile() = default;

Tile::Tile(sf::Texture* texture_list, float x, float y, bool mine) {
    this->mine = mine;
    setTextures(texture_list);
    setPos(x, y);
}

// Methods for initialization
void Tile::setMine(bool mine) {
    this->mine = mine;
}

void Tile::setPos(float x, float y) {
    background.setPosition(x, y);
    tile.setPosition(x, y);
    overlay.setPosition(x, y);
}

void Tile::setTextures(sf::Texture *texture_list) {
    this->tile_textures = texture_list;

    background.setTexture(tile_textures[0]);
    tile.setTexture(tile_textures[9]);
    overlay.setTexture(tile_textures[9]);
}

bool Tile::getMine() const {
    return mine;
}

bool Tile::getFlag() const {
    return m_flag;
}

bool Tile::getUnveiled() const {
    return unveiled;
}

void Tile::addNeighbor(Tile* neighbor) {
    neighboring_tiles.push_back(neighbor);
}

void Tile::calcAdjMines() {
    if(!(this->mine)) {
        for(auto & neighboring_tile : neighboring_tiles) {
            if(neighboring_tile->mine)
                this->adj_mines++;
        }
    }
}


bool Tile::update() {
    if(m_flag)
        return false;
    else if (mine) {
        tile.setTexture(tile_textures[10]);
    }
    else {
        tile.setTexture(tile_textures[this->adj_mines]);
    }
    this->unveiled = true;

    if(this->adj_mines == 0 && !this->mine) {
        for(auto & neighboring_tile : neighboring_tiles) {
            if(!neighboring_tile->unveiled)
                neighboring_tile->update();
        }
    }

    overlay.setColor(sf::Color::Transparent);
    return mine;
}

void Tile::unveilMine() {
    if(mine) {
        if(m_flag) {
            this->background.setTexture(tile_textures[9]);
            this->tile.setTexture(tile_textures[11]);
        }
        this->overlay.setTexture(tile_textures[10]);
    }
}

bool Tile::flag() {
    if(m_flag) {
        this->resetTexture();
        this->m_flag = false;
    }
    else {
        if(debug_mode && mine) {
            this->tile.setTexture(tile_textures[11]);
            this->background.setTexture(tile_textures[9]);
        }
        else
            this->overlay.setTexture(tile_textures[11]);
        this->m_flag = true;
    }

    return m_flag;
}

void Tile::debug() {
    if(!debug_mode) {
        this->debug_mode = true;
        this->unveilMine();
    }
    else {
        if (mine) {
            if (m_flag)
                overlay.setTexture(tile_textures[11]);
            else
                overlay.setTexture(tile_textures[9]);
            tile.setTexture(tile_textures[9]);
            background.setTexture(tile_textures[0]);
            this->debug_mode = false;
        }
        this->debug_mode = false;
    }
}

void Tile::resetTexture() {
    if(debug_mode && mine) {
        background.setTexture(tile_textures[0]);
        tile.setTexture(tile_textures[9]);
        overlay.setTexture(tile_textures[10]);
    }
    else {
        background.setTexture(tile_textures[0]);
        tile.setTexture(tile_textures[9]);
        overlay.setTexture(tile_textures[9]);
    }
}

void Tile::draw(sf::RenderWindow &window) {
    window.draw(this->background);
    window.draw(this->tile);
    window.draw(this->overlay);
}


/* ----------------------------------
 *  Board Methods
   --------------------------------*/

Board::Board(std::map<std::string, sf::Texture>* ui_texture_list, sf::Texture* texture_list, sf::Texture* digit_texture_list, unsigned int columns, unsigned int rows, unsigned int mines){
    this->ui_texture_list = ui_texture_list;
    this->tile_texture_list = texture_list;
    this->digit_texture_list = digit_texture_list;

    this->columns = columns;
    this->rows = rows;
    this->mine_num = mines;

    this->setTiles();
    this->setup();

}

Board::~Board() {
    for(unsigned int i = 0; i < this->rows; i++) {
        delete[] board_tiles[i];
        delete[] paused_tiles[i];
    }
    delete[] board_tiles;
    delete[] paused_tiles;
    board_tiles = nullptr;

}


void Board::setTiles() {
    if(board_tiles != nullptr) {
        for(int i = 0; i < this->rows; i++) {
            delete[] board_tiles[i];
        }
        delete[] board_tiles;
    }

    board_tiles = new Tile*[this->rows];
    paused_tiles = new sf::Sprite*[this->rows];
    for(unsigned int i = 0; i < this->rows; i++) {
        board_tiles[i] = new Tile[this->columns];
        paused_tiles[i] = new sf::Sprite[this->columns];
        for(unsigned int j = 0; j < this->columns; j++) {
            board_tiles[i][j].setTextures(this->tile_texture_list);
            paused_tiles[i][j].setTexture(tile_texture_list[0]);
            board_tiles[i][j].setPos(32.0f * j, 32.0f * i);
            paused_tiles[i][j].setPosition(32.0f * j, 32.0f * i);
        }
    }

    this->gen_mines();
    this->adjacent_tile_collection();
    for(unsigned int i = 0; i < this->rows; i++) {
        for(unsigned int j = 0; j < this->columns; j++) {
            this->board_tiles[i][j].calcAdjMines();
            this->board_tiles[i][j].resetTexture();
        }
    }

    this->mine_count = this->mine_num;
}


void Board::setDebug() {
    for (unsigned int i = 0; i < this->rows; i++) {
        for (unsigned int j = 0; j < this->columns; j++) {
            board_tiles[i][j].debug();
        }
    }
}


void Board::setup() {
    buttons["face"].setTexture((*ui_texture_list)["happy"]);
    buttons["face"].setPosition((this->columns / 2.0f) * 32 - 32, ((this->rows) + 0.5f) * 32);

    buttons["debug"].setTexture((*ui_texture_list)["debug"]);
    buttons["debug"].setPosition((this->columns * 32) - 304, (this->rows + 0.5f) * 32);

    buttons["pause"].setTexture((*ui_texture_list)["pause"]);
    buttons["pause"].setPosition((this->columns * 32) - 240, (this->rows + 0.5f) * 32);

    buttons["leaderboard"].setTexture((*ui_texture_list)["leaderboard"]);
    buttons["leaderboard"].setPosition((this->columns * 32) - 176, (this->rows + 0.5f) * 32);

    this->mines.push_back(sf::Sprite(this->digit_texture_list[mine_count / 100]));
    this->mines[0].setPosition(33, (this->rows + 0.5f) * 32 + 16);
    this->mines.push_back(sf::Sprite(this->digit_texture_list[(mine_count % 100) / 10]));
    this->mines[1].setPosition(54, (this->rows + 0.5f) * 32 + 16);
    this->mines.push_back(sf::Sprite(this->digit_texture_list[mine_count % 10]));
    this->mines[2].setPosition(75, (this->rows + 0.5f) * 32 + 16);
    this->mines.push_back(sf::Sprite(this->digit_texture_list[10]));
    this->mines[3].setPosition(12, (this->rows + 0.5f) * 32 + 16);



    for(int i = 0; i < 2; i++) {
        this->minutes.push_back(sf::Sprite(this->digit_texture_list[0]));
        this->minutes[i].setPosition(this->columns * 32 - 97 + (21 * i), (this->rows + 0.5f) * 32 + 16);
        this->seconds.push_back(sf::Sprite(this->digit_texture_list[0]));
        this->seconds[i].setPosition(this->columns * 32 - 54 + (21 * i), (this->rows + 0.5f) * 32 + 16);
    }
}


void Board::draw() {
    game_window.clear(sf::Color::White);
    if(time_state == "play") {
        for (int i = 0; i < this->rows; i++) {
            for (int j = 0; j < this->columns; j++) {
                board_tiles[i][j].draw(game_window);
            }
        }
    }
    else {
        for (int i = 0; i < this->rows; i++) {
            for (int j = 0; j < this->columns; j++) {
                game_window.draw(paused_tiles[i][j]);
            }
        }
    }

    game_window.draw(buttons["face"]);
    game_window.draw(buttons["debug"]);
    game_window.draw(buttons["pause"]);
    game_window.draw(buttons["leaderboard"]);

    game_window.draw(mines[2]);
    for(int i = 0; i < 2; i++) {
        game_window.draw(minutes[i]);
        game_window.draw(seconds[i]);
        game_window.draw(mines[i]);
    }

    if(mine_count < 0) {
        game_window.draw(mines[3]);
    }

    game_window.display();
}


void Board::game() {
    game_window.create(sf::VideoMode(columns * 32, (rows * 32 + 100)), "Minesweeper", sf::Style::Close);
    game_window.setFramerateLimit(60);

    game_state = "normal";
    time_state = "play";
    start = std::chrono::high_resolution_clock::now();

    while(game_window.isOpen()) {
        sf::Event to_check;

        while(game_window.pollEvent(to_check)) {
            switch (to_check.type) {
                case (sf::Event::Closed): {
                    game_window.close();
                    return;
                }
                case (sf::Event::MouseButtonPressed): {
                    int x = sf::Mouse::getPosition(game_window).x;
                    int y = sf::Mouse::getPosition(game_window).y;

                    try {
                        if (y <= 32 * this->rows && game_state == "normal" && time_state == "play") {
                            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                                this->check_lose(board_tiles[y / 32][x / 32].update());
                                this->check_win();
                            } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !(board_tiles[y / 32][x / 32].getUnveiled())) {
                                this->updateMineCount(board_tiles[y / 32][x / 32].flag());
                                this->check_win();
                            }
                        }
                        else {
                            if(buttons["face"].getGlobalBounds().contains(x, y)) {
                                this->reset();
                            }
                            else if(buttons["debug"].getGlobalBounds().contains(x, y) && game_state == "normal" && time_state == "play") {
                                this->setDebug();
                            }
                            else if(buttons["pause"].getGlobalBounds().contains(x, y) && game_state == "normal") {
                                this->pausePlay();
                            }
                        }
                    }
                    catch (...) {}
                }
                default: {}
            }
        }

        if(time_state == "play" && game_state == "normal") {
            updateTimer();
        }

        if(game_window.isOpen())
            this->draw();
    }
}


void Board::check_lose(bool mine) {
    if(mine) {
        buttons["face"].setTexture((*ui_texture_list)["lose"]);
        game_state = "lose";
        for(unsigned int i = 0; i < this->rows; i++) {
            for(unsigned int j = 0; j < this->columns; j++) {
                board_tiles[i][j].unveilMine();
            }
        }
    }
}


void Board::check_win() {
    if(mine_count == 0) {
        for(unsigned int i = 0; i < this->rows; i++) {
            for(unsigned int j = 0; j < this->columns; j++) {
                if(!board_tiles[i][j].getUnveiled() && (!board_tiles[i][j].getMine() || !board_tiles[i][j].getFlag()))
                    return;
            }
        }

        buttons["face"].setTexture((*ui_texture_list)["win"]);
        game_state = "win";
    }
}


// I hate this function. It's so ugly
void Board::gen_mines() {
    int chance = (this->rows * this->columns)/this->mine_num;
    unsigned int current_mines = 0;

    /// Look, I know this looks really bad will run bad, but this is the best way I could think of at the moment
    while(current_mines < this->mine_num) {
        for (unsigned int i = 0; i < this->rows; i++) {
            for (unsigned int j = 0; j < this->columns; j++) {
                int rand_num = std::experimental::randint(1, chance);

                if (current_mines < this->mine_num && rand_num == 8 && !(board_tiles[i][j].getMine())) {
                    board_tiles[i][j].setMine(true);
                    current_mines++;
                }
            }
        }
    }
}


void Board::adjacent_tile_collection() {
    for(unsigned int i = 0; i < this->rows; i++) {
        for (unsigned int j = 0; j < this->columns; ++j) {
            if (i == 0) {
                board_tiles[i][j].addNeighbor(&(board_tiles[i+1][j]));

                if(j != 0) {
                    board_tiles[i][j].addNeighbor(&(board_tiles[i][j-1]));
                    board_tiles[i][j].addNeighbor(&(board_tiles[i+1][j-1]));
                }
                if (j != (columns - 1)) {
                    board_tiles[i][j].addNeighbor(&(board_tiles[i][j+1]));
                    board_tiles[i][j].addNeighbor(&(board_tiles[i+1][j+1]));
                }
            }
            else if (i == (rows - 1)) {
                board_tiles[i][j].addNeighbor(&(board_tiles[i-1][j]));

                if(j != 0) {
                    board_tiles[i][j].addNeighbor(&(board_tiles[i-1][j-1]));
                    board_tiles[i][j].addNeighbor(&(board_tiles[i][j-1]));
                }
                if (j != (columns - 1)) {
                    board_tiles[i][j].addNeighbor(&(board_tiles[i-1][j+1]));
                    board_tiles[i][j].addNeighbor(&(board_tiles[i][j+1]));
                }
            }
            else {
                board_tiles[i][j].addNeighbor(&(board_tiles[i-1][j]));
                board_tiles[i][j].addNeighbor(&(board_tiles[i+1][j]));

                if(j != 0) {
                    board_tiles[i][j].addNeighbor(&(board_tiles[i-1][j-1]));
                    board_tiles[i][j].addNeighbor(&(board_tiles[i][j-1]));
                    board_tiles[i][j].addNeighbor(&(board_tiles[i+1][j-1]));
                }
                if (j != (columns - 1)) {
                    board_tiles[i][j].addNeighbor(&(board_tiles[i-1][j+1]));
                    board_tiles[i][j].addNeighbor(&(board_tiles[i][j+1]));
                    board_tiles[i][j].addNeighbor(&(board_tiles[i+1][j+1]));
                }
            }
        }
    }
}

void Board::updateMineCount(bool direction) {
    if(direction)
        mine_count--;
    else
        mine_count++;

    if(mine_count >= 0) {
        this->mines[0].setTexture(digit_texture_list[mine_count/100]);
        this->mines[1].setTexture(digit_texture_list[(mine_count % 100) / 10]);
        this->mines[2].setTexture(digit_texture_list[mine_count % 10]);
    }
    else {
        this->mines[0].setTexture(digit_texture_list[(-1 * mine_count)/100]);
        this->mines[1].setTexture(digit_texture_list[((-1 * mine_count) % 100) / 10]);
        this->mines[2].setTexture(digit_texture_list[(-1 * mine_count) % 10]);
    }
}

void Board::updateTimer() {
    std::chrono::nanoseconds current_time = std::chrono::high_resolution_clock::now() - start + time_buffer;

    this->seconds[1].setTexture(digit_texture_list[std::chrono::duration_cast<std::chrono::seconds>(current_time).count() % 10]);
    this->seconds[0].setTexture(digit_texture_list[(std::chrono::duration_cast<std::chrono::seconds>(current_time).count() / 10) % 6]);

    this->minutes[1].setTexture(digit_texture_list[std::chrono::duration_cast<std::chrono::minutes>(current_time).count() % 10]);
    this->minutes[0].setTexture(digit_texture_list[(std::chrono::duration_cast<std::chrono::minutes>(current_time).count() / 10) % 6]);
}

void Board::reset() {
    this->setTiles();
    buttons["face"].setTexture((*ui_texture_list)["happy"]);
    buttons["pause"].setTexture((*ui_texture_list)["pause"]);

    time_buffer = std::chrono::nanoseconds(0);
    start = std::chrono::high_resolution_clock::now();

    game_state = "play";
    game_state = "normal";
}

void Board::pausePlay() {
    if(time_state == "play") {
        time_buffer += (std::chrono::high_resolution_clock::now() - start);
        buttons["pause"].setTexture((*ui_texture_list)["play"]);
        time_state = "pause";
    }
    else {
        buttons["pause"].setTexture((*ui_texture_list)["pause"]);
        time_state = "play";
        start = std::chrono::high_resolution_clock::now();
    }
}