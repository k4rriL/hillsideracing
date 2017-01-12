/*
 * screen.hpp
 *
 *  Created on: Nov 17, 2016
 *      Author: karri
 */

#ifndef SRC_SCREEN_HPP_
#define SRC_SCREEN_HPP_

#include "SFML/Graphics.hpp"
class screen{
public:
	virtual ~screen(){};
	virtual int Run(sf::RenderWindow &App) = 0;
};



#endif /* SRC_SCREEN_HPP_ */
