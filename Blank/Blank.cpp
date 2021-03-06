#include "Blank/Blank.h"

#include <SFML/Graphics.hpp>
#include <cstring>
#include <ctime>
#include <experimental/random>
#include <fstream>
#include <memory>
#include <string>

#include "Blank/Blanknode.h"
#include "Game/Game.h"
#include "Screen/GameOverScreen.h"
#include "Screen/MenuScreen.h"

using namespace Sudoku;

const int BLANK_BEGIN[10] = {0, 12, 88, 164, 248, 324, 400, 484, 560, 636};
const int BLANK_END[10] = {0, 84, 160, 236, 320, 396, 472, 556, 632, 708};
const sf::Keyboard::Key press[2][10] = {
    {sf::Keyboard::Num0, sf::Keyboard::Num1, sf::Keyboard::Num2,
     sf::Keyboard::Num3, sf::Keyboard::Num4, sf::Keyboard::Num5,
     sf::Keyboard::Num6, sf::Keyboard::Num7, sf::Keyboard::Num8,
     sf::Keyboard::Num9},
    {sf::Keyboard::Numpad0, sf::Keyboard::Numpad1, sf::Keyboard::Numpad2,
     sf::Keyboard::Numpad3, sf::Keyboard::Numpad4, sf::Keyboard::Numpad5,
     sf::Keyboard::Numpad6, sf::Keyboard::Numpad7, sf::Keyboard::Numpad8,
     sf::Keyboard::Numpad9}};
sf::Mouse mouse;
int time_as_sec;

Blank::Blank(int difficulty) {
    std::experimental::reseed(time(NULL));
    Game::map_id =
        std::experimental::randint(0, Game::map_id_limit[difficulty]);
    std::string sudoku_address =
        "Sudoku/" + std::to_string(difficulty) + std::to_string(Game::map_id);
    std::ifstream ifp(sudoku_address);
    char t = 0;
    for (int j = 1; j < 10; j++) {
        for (int i = 1; i < 10; i++) {
            while (!(t >= '0' && t <= '9') && ifp.good())
                ifp >> t;
            if (t == '0') {
                mut_map[i][j] = 1;
                now_map[i][j] = 0;
            } else {
                mut_map[i][j] = 0;
                now_map[i][j] = t - '0';
            }
            t = 0;
        }
    }
    ifp.close();
    target_x = 0;
    target_y = 0;

    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            sf::Vector2f position_ = {1.0f * BLANK_BEGIN[i],
                                      1.0f * BLANK_BEGIN[j]};
            nodes[i][j] = BlankNode(position_);
        }
    }

    clock.restart();
    time_as_sec = clock.getElapsedTime().asSeconds();
    font_.loadFromFile("Resource/font01.ttf");
    std::string texttmp("Sudoku:" + std::to_string(Game::difficulty) +
                        std::to_string(Game::map_id) + "\n" +
                        "Time:" + std::to_string(time_as_sec / 60) + ":" +
                        std::to_string(time_as_sec % 60));

    text_.setString(texttmp);
    text_.setFont(font_);
    text_.setFillColor(sf::Color::White);
    text_.setCharacterSize(40);
    text_.setPosition({750, 6});
}

void Blank::handleInput(sf::RenderWindow& window) {
    if (mouse.isButtonPressed(mouse.Left)) {
        auto position_ = mouse.getPosition(window);
        int mouse_x = position_.x, mouse_y = position_.y;

        ////
        if (mouse_x >= 780 && mouse_x <= 1092) {
            if (mouse_y >= 130 && mouse_y <= 190) {
                reset();
                Game::Screen = std::make_shared<MenuScreen>();
                return;
            }
            if (mouse_y >= 200 && mouse_y <= 260) {
                reset();
                return;
            }
            if (mouse_y >= 270 && mouse_y <= 330) {
                if (check())
                    Game::Screen = std::make_shared<GameOverScreen>();
            }
        }
        ////

        bool exx = false, exy = false;
        int nx = 0, ny = 0;
        for (int i = 1; i <= 9; i++) {
            if (mouse_x >= BLANK_BEGIN[i] && mouse_x <= BLANK_END[i]) {
                nx = i, exx = true;
                break;
            }
        }
        for (int i = 1; i <= 9; i++) {
            if (mouse_y >= BLANK_BEGIN[i] && mouse_y <= BLANK_END[i]) {
                ny = i, exy = true;
                break;
            }
        }
        if (exx && exy) {
            if (target_x == nx && target_y == ny) {
                target_x = target_y = 0;
            } else {
                target_x = nx, target_y = ny;
            }
        } else {
            target_x = target_y = 0;
        }
    }
    if (mut_map[target_x][target_y]) {
        for (int i = 0; i <= 9; i++) {
            if (sf::Keyboard::isKeyPressed(press[0][i]) ||
                sf::Keyboard::isKeyPressed(press[1][i])) {
                now_map[target_x][target_y] = i;
            }
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if (target_x == 0)
            target_x = target_y = 1;
        else {
            target_x = (target_x + 7) % 9 + 1;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        if (target_x == 0)
            target_x = target_y = 1;
        else {
            target_x = target_x % 9 + 1;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        if (target_x == 0)
            target_x = target_y = 1;
        else {
            target_y = (target_y + 7) % 9 + 1;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        if (target_x == 0)
            target_x = target_y = 1;
        else {
            target_y = target_y % 9 + 1;
        }
    }
}

void Blank::update() {}

void Blank::render(sf::RenderWindow& window) {
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            int shape = 0;
            if (i == target_x && j == target_y && mut_map[i][j]) {
                shape = 4;
            } else {
                if (mut_map[i][j]) {
                    if (i == target_x || j == target_y)
                        shape = 3;
                    else
                        shape = 0;
                } else {
                    if (i == target_x || j == target_y)
                        shape = 2;
                    else
                        shape = 1;
                }
            }
            nodes[i][j].render(window, now_map[i][j], shape);
        }
    }
    time_as_sec = clock.getElapsedTime().asSeconds();
    std::string texttmp("Sudoku:" + std::to_string(Game::difficulty) +
                        std::to_string(Game::map_id) + "\n" +
                        "Time:" + std::to_string(time_as_sec / 60) + ":" +
                        std::to_string(time_as_sec % 60));

    text_.setString(texttmp);
    window.draw(text_);
}

void Blank::reset() {
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            if (mut_map[i][j])
                now_map[i][j] = 0;
        }
    }
    target_x = target_y = 0;
    clock.restart();
}
bool Blank::check() {
    int helper[10] = {0};
    for (int i = 1; i <= 9; i++) {
        memset(helper, 0, sizeof(helper));
        for (int j = 1; j <= 9; j++) {
            if (!now_map[i][j])
                return false;
            if (helper[now_map[i][j]])
                return false;
            helper[now_map[i][j]]++;
        }
    }
    for (int i = 1; i <= 9; i++) {
        memset(helper, 0, sizeof(helper));
        for (int j = 1; j <= 9; j++) {
            if (!now_map[j][i])
                return false;
            if (helper[now_map[j][i]])
                return false;
            helper[now_map[j][i]]++;
        }
    }
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) {
            memset(helper, 0, sizeof(helper));
            for (int p = 3 * i - 2; p <= 3 * i; p++) {
                for (int q = 3 * j - 2; q <= 3 * j; q++) {
                    if (!now_map[p][q])
                        return false;
                    if (helper[now_map[p][q]])
                        return false;
                    helper[now_map[p][q]]++;
                }
            }
        }
    }
    return true;
}