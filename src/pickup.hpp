/*
 * pickup.hpp
 *
 *  Created on: 14.11.2016
 *      Author: karri
 */

#ifndef SRC_PICKUP_HPP_
#define SRC_PICKUP_HPP_

#include "SFML/Graphics.hpp"
#include "car.hpp"
#include "SFML/Audio.hpp"

class Pickup {
public:

    /*
    Takes parameters 2-6 parameters. 
    _texture is pointer to a texture used in pickup. 
    _noise is the sound pickup makes when collected.
    _points is number of points player gets when pickup is collected.
    _effects a vector that MUST contain 4 floats. They are used in effectCar-method. Meanings of of floats are:
        0. how much player gets "gasoline" from pickup (can be <0)
        1. hox much effects velocity (1 is neutral)
	2. how much effects density
	3. how much effects friction
    _duration is the time pickups effects are effective
    */
    Pickup( sf::Texture * _texture, sf::Sound *_noise, double scale = 0.15,
			int _points = 100, std::vector<float> _effects = {0,1,1,1}, int _duration = 0 );
    
    Pickup& operator=(const Pickup& other);
    ~Pickup() {}
    
    sf::FloatRect getGlobalBounds() {return borders.getGlobalBounds();}
    std::vector<float> getEffects() {return effects;}
    sf::Sound* getNoise() {return noise;}
    int getPoints() {return points;}
    int getDuration() {return duration;}

    void effectCar(Car *car){car->pickupEffects(effects,duration);}
    void drawTo(sf::RenderWindow &window) {window.draw(sprite);}

    //sets position. pickups origin is its center. 
    void setPos(int x, int y) {
		sf::Vector2f newPos(x, y);
		sprite.setPosition(newPos);
		borders.setPosition(newPos);
    }

    //checks if this pickup is colliding whith a car
    bool isColliding(Car *car);



private:
    sf::Texture* texture;
    sf::Sound* noise;
    sf::Sprite sprite;
    int points;
    std::vector<float> effects;
    int duration;
    sf::CircleShape borders; //smaller shape than actual pickup so car must touch pickup to collect it.
    
};

#endif /* SRC_PICKUP_HPP_ */
