#include "Wind.h" // Includes Tiles.h and sfml_i.h
#include <iostream>


Welcome::Welcome(unsigned int columns, unsigned int rows) {
	if (!(font.loadFromFile("./files/font.ttf"))) {
		throw std::runtime_error("ERROR: Invalid Font");
	}

    this->height = rows * 32 + 100;
    this->width = columns * 32;

	//Look into ways to initialize these without these big blocks
	cursor.setString("|");
	cursor.setFont(font);
	cursor.setCharacterSize(18);
	cursor.setFillColor(sf::Color::Yellow);

	welcome.setString("WELCOME TO MINESWEEPER!");
	welcome.setFont(font);
	welcome.setStyle(sf::Text::Underlined | sf::Text::Bold);
	welcome.setCharacterSize(24);

	prompt.setString("Enter your name:");
	prompt.setFont(font);
	prompt.setStyle(sf::Text::Bold);
	prompt.setCharacterSize(20);

	user_name.setString(t_input);
	user_name.setFont(font);
	user_name.setStyle(sf::Text::Bold);
	user_name.setCharacterSize(18);
	user_name.setFillColor(sf::Color::Yellow);

	absWindow::setText(welcome, 400.0f, 150.0f);
    absWindow::setText(prompt, 400.0f, 225.0f);
    absWindow::setText(cursor, 400.0f, 255.0f);
    absWindow::setText(user_name, 400.0f, 255.0f);
}

void Welcome::draw() {
    window.clear(sf::Color::Blue);
    window.draw(welcome);
    window.draw(prompt);
    window.draw(cursor);
    window.draw(user_name);

    window.display();
}

bool Welcome::operator()() {
    window.create(sf::VideoMode(this->width, this->height), "Minesweeper", sf::Style::Close);
    window.setFramerateLimit(60);

    while(true) {
        sf::Event to_check;
        window.pollEvent(to_check);

        switch (to_check.type) {
            case (sf::Event::Closed): {
                std::cout << "Closed" << std::endl;
                window.close();
                return false;
            }

            case (sf::Event::KeyPressed): {
                window.pollEvent(to_check);

                if(to_check.type == sf::Event::TextEntered) {
                    char in = static_cast<char>(to_check.text.unicode);

                    if (((to_check.text.unicode >= 65 && to_check.text.unicode <= 90) || (to_check.text.unicode >= 97 && to_check.text.unicode <= 122)) && this->current_char < this->MAX_CHAR) {
                        if (t_input.empty()) {
                            in = std::toupper(in);
                            t_input = in;
                            current_char++;
                        }
                        else {
                            in = std::tolower(in);
                            t_input.push_back(in);
                            current_char++;
                        }

                        cursor.move(6.2f, 0.0f);
                        user_name.setString(t_input);
                        break;
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace) && !(t_input.empty())) {
                        t_input.pop_back();
                        current_char--;

                        cursor.move(-6.2f, 0.0f);
                        user_name.setString(t_input);
                        break;
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !(t_input.empty())) {
                        window.close();
                        return true;
                    }
                }

                break;
            }

            default: {
            }

        }

        //Drawing the window.
        setText(user_name, 400.0f, 255.0f);

        this->draw();
    }
}


/*----------------------------------
 * Leaderboard methods
 ----------------------------------*/

Leaderboard::Leaderboard(std::string path) {
    if (!(this->font.loadFromFile("./files/font.ttf"))) {
        throw std::runtime_error("ERROR: Invalid Font");
    }

    title.setFont(this->font);
    title.setString("LEADERBOARD");
    title.setCharacterSize(20);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    absWindow::setText(title, width/2.0f, height/2.0f - 120);

    scores.setFont(this->font);
    scores.setCharacterSize(18);
    scores.setStyle(sf::Text::Bold);
    absWindow::setText(scores, width/2.0f, height/2.0f + 20);


    this->path = path;
}

Leaderboard::~Leaderboard() {
}

void Leaderboard::setBounds(unsigned int x, unsigned int y) {
    this->height = y * 16 + 50;
    this->width = x * 16;
}


void Leaderboard::createWindow() {
    this->window.create(sf::VideoMode(width, height), "Minesweeper", sf::Style::Close);
}


void Leaderboard::draw() {
    this->window.clear(sf::Color::Blue);
    this->window.draw(title);
    this->window.draw(scores);

    this->window.display();
}


void Leaderboard::read() {
    std::string token1;
    std::string token2;

    std::ifstream txt_reader(path);
    int i = 0;

    while(std::getline(txt_reader, token1, ',') && std::getline(txt_reader, token2, '\n') && i < 5) {
        time_data.push_back(token1);
        name_data.push_back(token2);
        std::cout << token1 << " " << token2 << std::endl;
        i++;
    }

    txt_reader.close();

    setText();
}

void Leaderboard::write() {
    std::ofstream txt_writer(path);

    for(int i = 0; i < name_data.size(); i++) {
        txt_writer << time_data.at(i) << ',' << name_data.at(i) << std::endl;
    }
}

void Leaderboard::insert(std::string time, std::string name) {
    if(time <= *(time_data.end())) {
        if(time_data.size() == 5) {
            time_data.pop_back();
        }

        auto time_iter = time_data.begin();
        auto name_iter = name_data.begin();

        for(int i = 0; i < time_data.size(); i++) {
            if(time <= time_data[i]) {
                time_data.insert(time_iter, time);
                name_data.insert(name_iter, name);
                setText();
                return;
            }

            time_iter++;
            name_iter++;
        }
    }
}


void Leaderboard::setText() {
    std::string to_text = "";

    for(int j = 0; j < time_data.size(); j++) {
        to_text += std::to_string(j + 1) + ".\t" + time_data[j] + "\t" + name_data[j] + "\n\n";
    }

    scores.setString(to_text);
}

void Leaderboard::close() {
    this->write();
    window.close();
}