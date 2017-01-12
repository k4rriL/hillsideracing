/*
 * screen_game.hpp
 *
 *  Created on: Nov 17, 2016
 *      Author: karri
 */

#ifndef SRC_SCREEN_GAME_HPP_
#define SRC_SCREEN_GAME_HPP_

#include <iostream>
#include "screen.hpp"
#include "terrain.hpp"

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

class screen_game : public screen
{
public:
    screen_game(int level, int car, int mode);
    bool isGameOver() {return _gameOver;};
    virtual int Run(sf::RenderWindow &App);

private:
	int mode;
	int carNumber;
	int level;
	Terrain _terrain;
	sf::Texture _backGround;
	sf::Sprite _sprite;
	bool _gameOver = false;
};

#endif /* SRC_SCREEN_GAME_HPP_ */
