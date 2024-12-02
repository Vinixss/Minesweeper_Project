#pragma once
#include "sfml_i.h"
#include <fstream>
#include <iostream>

struct absWindow {
    sf::RenderWindow window;
    sf::Font font;

    unsigned int width;
    unsigned int height;

    static void setText(sf::Text& text, float x, float y) {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(x, y));
    }

    virtual void draw() = 0;
};

class Welcome: public absWindow {
	const unsigned short MAX_CHAR = 10;
	unsigned short current_char = 0;

	sf::Text cursor;
	sf::Text welcome;
	sf::Text prompt;
	sf::Text user_name;

    void draw();

public:
	std::string t_input;


	Welcome(unsigned int columns, unsigned int rows);

	bool operator()();


};

class Leaderboard: public absWindow {

    std::string path;
    std::vector<std::string> name_data;
    std::vector<std::string> time_data;

    sf::Text title;
    sf::Text scores;

    void setText();

public:
    Leaderboard(std::string path);
    ~Leaderboard();

    void setBounds(unsigned int x, unsigned int y);

    void read();
    void write();

    void insert(std::string time, std::string name);

    void createWindow();
    void draw();
    void close();
};