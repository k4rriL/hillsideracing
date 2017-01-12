#ifndef __CAR_H__
#define __CAR_H__
#include <iostream>

#include "Box2D/Box2D.h"
#include "SFML/Graphics.hpp"

const float PTM_RATIO = 32.f;

class CarSpecs {
public:
	CarSpecs(float tireFriction, float maxTorque, float targetSpeed, float bodyDensity, std::string chassisImage, float height, int carid) {
			friction = tireFriction;
			torque = maxTorque;
			speed = targetSpeed;
			density = bodyDensity;
			image = chassisImage;
			h = height;
            id = carid;
	};
	CarSpecs() {};

	void setTireFriction(float d) {friction = d;};
	void setMaxTorque(float d) {torque = d;}
	void setTargetSpeed(float d) {speed = d;};
	void setBodyDensity(float d) {density = d;};
	void setChassisImage(std::string i) {image = i;};
    void setHeight(float d) {h = d;}
    void setID(int d) {id = d;}

	double getTireFriction() {return friction;};
	double getMaxTorque() {return torque;};
	double getTargetSpeed() {return speed;};
	double getBodyDensity() {return density;};
	double getHeight() {return h;};
    int getID() {return id;};
	std::string getChassisImage() {return image;};
private:
	float friction = 0.8f;
	float torque = 80;
	float speed = 25;
	float density = 1;
	float h = 7.0f * 2.0f;
    int id = 0;
	std::string image = "textures/toyCar1.png";
};



class Car {
protected:

	CarSpecs* _specs;

    bool _accelerating;
    bool _reversing;
    float _fuel;
    const float _fuelTank = 1.0;

    b2Vec2 oldPos; // Variable to track relative changes in position
    int gameDamage = 0; // Keep track of ticks spent in a potential game-over scenario
    bool broken = false; // Is the car broken? 


    // Remember the textures so that they never disappear
    sf::Texture tireTex;
    sf::Texture driverTex;
    sf::Texture carTex;

    // Save the SFML sprites
    sf::Sprite* _bodySprite;
    sf::Sprite* _frontTireSprite;
    sf::Sprite* _driverSprite;
    sf::Sprite* _rearTireSprite;

    // Define the Box2D bodies and parts
    b2Body* _frontTireBody;
    b2Body* _rearTireBody;
    b2Body* _body;
    b2Body* _headBody;
    b2RevoluteJoint* _neckRevoluteJoint;
    b2WheelJoint* _rearTireJoint;
    b2WheelJoint* _frontTireJoint;

    // Define some modifiers that allow for temporary effects (possible future implementation)
    float velocityModifier = 1.0;
    float densityModifier = 1.0;
    float frictionModifier = 1.0;
    int effectLifetime = 0;

public:
    Car(b2World& world, float posx, float posy, CarSpecs *specs);
    ~Car() {}
    Car& operator=(const Car& other);

    // Methods to get private variables

    b2Body* getBody() {return _body;}
    b2Body* getFrontWheel() {return _frontTireBody;}
    b2Body* getRearWheel() {return _rearTireBody;}

    sf::Sprite* getSprite() {return _bodySprite;}

    float getAngle() {return _body->GetAngle();}
    float getFuel() {return _fuel / _fuelTank;}

    void fillTank() {_fuel = _fuelTank;}
    void setFuel(float fuel) {_fuel = fuel;}

    // Method to return how many ticks an effect is still valid for (if applicable)
    int getEffectLifetime() {return effectLifetime;}

    void accelerate(bool accelerating) {_accelerating = accelerating;}
    void reverse(bool reversing) {_reversing = reversing;}

    // Allow breaking/fixing the car (broken car cannot be driven)
    bool isBroken() {return broken;}
    void fix() {broken = false;}
    void breakCar() {broken = true;}

    void resetBoosts() {
    	velocityModifier = 1.0;
        densityModifier = 1.0;
        frictionModifier = 1.0;
        effectLifetime = 0;
    }

    int countGroundedTires(); // Counts how many tires are colliding with something (the ground)

    void update(); // Main method to call every time the car's status needs to be updated

    void balance(bool counterclockwise); // Method to apply a rotational balance to the car

    void velocityBoost(float coefficient, float durationInTicks);

    void pickupEffects(std::vector<float> effects, int duration);

};
#endif
