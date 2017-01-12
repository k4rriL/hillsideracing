/*
 * pickup.cpp
 *
 *  Created on: 14.11.2016
 *      Author: karri
 */
#include "pickup.hpp"
#include "car.hpp"

Pickup::Pickup( sf::Texture * _texture, sf::Sound *_noise, double scale, int _points, std::vector<float> _effects, int _duration ){
	noise = _noise;
	effects = _effects;
	duration = _duration;
	points = _points;
	texture = _texture;
	sprite.setTexture(*texture);
	sprite.setOrigin(sf::Vector2f(texture->getSize().x * 0.5, texture->getSize().y * 0.5));
	sprite.setPosition(0, 0);
	sprite.setScale(scale, scale);
	sf::CircleShape _borders(sprite.getGlobalBounds().width/3.0);
	borders = _borders;
	borders.setOrigin(sprite.getGlobalBounds().width/5.0,sprite.getGlobalBounds().width/5.0);
}

Pickup& Pickup::operator=(const Pickup& other) {
	texture = other.texture;
	sprite = other.sprite;
	points = other.points;
	effects = other.effects;
	duration = other.duration;

	return *this;
}

bool Pickup::isColliding(Car *car) {
	if (borders.getGlobalBounds().intersects(car -> getSprite()->getGlobalBounds())) return true;
	else return false;
}

