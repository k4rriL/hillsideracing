#include "car.hpp"
#include <iostream>
#include <math.h>

Car::Car(b2World& world, float posx, float posy, CarSpecs* specs) {

    this->_specs = specs;
	this->_fuel = _fuelTank;
	this->_reversing = false;
	this->_accelerating = false;

	this->_frontTireSprite = new sf::Sprite();
	this->_rearTireSprite = new sf::Sprite();
	this->_bodySprite = new sf::Sprite();
	this->_driverSprite = new sf::Sprite();

	tireTex.loadFromFile("textures/tire.png");
	_frontTireSprite->setTexture(tireTex);
	_rearTireSprite->setTexture(tireTex);

	carTex.loadFromFile(specs->getChassisImage());
	_bodySprite->setTexture(carTex);

	// Toy car antenna exception
	if (specs->getID() == 2) driverTex.loadFromFile("textures/antenna1.png");
	else driverTex.loadFromFile("textures/head1.png");
	_driverSprite->setTexture(driverTex);

	this->_frontTireSprite->setOrigin(tireTex.getSize().x / 2, tireTex.getSize().y / 2);
	this->_rearTireSprite->setOrigin(tireTex.getSize().x / 2, tireTex.getSize().y / 2);
	this->_driverSprite->setOrigin(driverTex.getSize().x / 2, (driverTex.getSize().y / 2)) ;

	b2BodyDef bodyDef;
	b2PolygonShape carShape; b2Vec2(0,0);
	b2Vec2 mass_offset = b2Vec2(0,0);
    
    // Define chassis based on car type
	if (specs->getID() == 2) { // Toy car
		b2Vec2 vertices[6];
		vertices[0].Set(-50.0f/PTM_RATIO, 38.0f/PTM_RATIO);
		vertices[1].Set(57.0f/PTM_RATIO, 38.0f/PTM_RATIO);
		vertices[2].Set(57.f/PTM_RATIO, 0.f/PTM_RATIO);
		vertices[3].Set(10.f/PTM_RATIO, -35.f/PTM_RATIO);
		vertices[4].Set(-52.f/PTM_RATIO, -35.f/PTM_RATIO);
		vertices[5].Set(-70.f/PTM_RATIO, 10.f/PTM_RATIO);
		carShape.Set(vertices, 6);
		bodyDef.position = b2Vec2(posx / PTM_RATIO, posy / PTM_RATIO);
		this->_bodySprite->setOrigin(carTex.getSize().x / 2, carTex.getSize().y  / 2);
		mass_offset = b2Vec2(0,20/PTM_RATIO);
	}
	else if (specs->getID() == 1) { // Formula
		bodyDef.position = b2Vec2(posx / PTM_RATIO, posy / PTM_RATIO);
		this->_bodySprite->setOrigin(carTex.getSize().x / 2, carTex.getSize().y  / 2);
		carShape.SetAsBox((60.0f) / PTM_RATIO, (specs->getHeight()) / PTM_RATIO);
	}
	else {
		bodyDef.position = b2Vec2(posx / PTM_RATIO, posy / PTM_RATIO);
		carShape.SetAsBox((55.0f) / PTM_RATIO, (specs->getHeight()) / PTM_RATIO);
		this->_bodySprite->setOrigin(145.f / 2, 60.0f / 2);
	}

	bodyDef.type = b2_dynamicBody;
	_body = world.CreateBody(&bodyDef);	
	_body->SetUserData(_bodySprite);

	b2FixtureDef carFixtureDef;
	carFixtureDef.shape = &carShape;
	carFixtureDef.density = specs->getBodyDensity();
	carFixtureDef.friction = 0.7;
	carFixtureDef.filter.categoryBits = 0x02;
	carFixtureDef.restitution = 0.0;
	carFixtureDef.filter.maskBits = 0x01;
	_body->CreateFixture(&carFixtureDef);
	
	b2MassData massData;
	_body->GetMassData(&massData);
	massData.center = mass_offset;
	_body->SetMassData(&massData);

	// Define driver
	b2BodyDef driverBodyDef;
	b2Vec2 position = _body->GetPosition();
    oldPos = position;
    if (specs->getID() == 2) { // Toy car
    	driverBodyDef.position.Set((position.x - 30.0f / PTM_RATIO), (position.y - 80.0f / PTM_RATIO));
    	this->_driverSprite->setOrigin(driverTex.getSize().x/2, driverTex.getSize().y / 4);
    }
    else if (specs->getID() == 1) { // Formula
    	driverBodyDef.position.Set((position.x - 32.0f / PTM_RATIO), (position.y - 15.0f / PTM_RATIO));
    }
    else driverBodyDef.position.Set((position.x + 10.0f / PTM_RATIO), (position.y - 16.0f / PTM_RATIO));

	driverBodyDef.type = b2_dynamicBody;

	_headBody = world.CreateBody(&driverBodyDef);
	_headBody->SetUserData(_driverSprite);
	b2CircleShape headShape;
	headShape.m_radius = 16.0f / PTM_RATIO;
	b2FixtureDef headFixtureDef;
	headFixtureDef.shape = &headShape;
	headFixtureDef.density = 0.01f;
	headFixtureDef.filter.categoryBits = 0x02;
	headFixtureDef.filter.maskBits = 0x01;
	headFixtureDef.restitution=0.0f;
	_headBody->CreateFixture(&headFixtureDef);

	// Attach driver to chassis
	b2RevoluteJointDef headJointDef;
	headJointDef.enableLimit = true;
	headJointDef.lowerAngle = -0.1f;
	headJointDef.upperAngle = 0.1f;
	b2Vec2 neckPos = b2Vec2(_headBody->GetWorldCenter().x, _headBody->GetWorldCenter().y);
	headJointDef.Initialize(_body, _headBody, neckPos);
	_neckRevoluteJoint = (b2RevoluteJoint*) world.CreateJoint(&headJointDef);

	// Define rear tire
	b2BodyDef rearTireBodyDef;
	b2CircleShape rearTireShape;
	if (specs->getID() == 2) { // Toy car
		rearTireBodyDef.position.Set((posx - 36.f) / PTM_RATIO, (posy + 40.0f) / PTM_RATIO);
		rearTireShape.m_radius = 20.0f / PTM_RATIO;
		this->_rearTireSprite->setScale(1.2f, 1.2f);
	}
	else if (specs->getID() == 1) { // Formula
		rearTireBodyDef.position.Set((posx - 57.5f) / PTM_RATIO, (posy + 15.0f) / PTM_RATIO);
		rearTireShape.m_radius = 13.0f / PTM_RATIO;
		this->_rearTireSprite->setScale(0.85f, 0.85f);
	}
	else {
		rearTireBodyDef.position.Set((posx - 37.5f) / PTM_RATIO, (posy + 15.0f) / PTM_RATIO);
		rearTireShape.m_radius = 16.0f / PTM_RATIO;
	}

	rearTireBodyDef.type = b2_dynamicBody;

	_rearTireBody = world.CreateBody(&rearTireBodyDef);
	_rearTireBody->SetUserData(_rearTireSprite);

	b2FixtureDef rearTireFixtureDef;
	rearTireFixtureDef.shape = &rearTireShape;
	rearTireFixtureDef.density = specs->getBodyDensity() / 10;
	rearTireFixtureDef.friction = specs->getTireFriction();
	rearTireFixtureDef.filter.categoryBits = 0x02;
	rearTireFixtureDef.restitution = 0.0;

	_rearTireBody->CreateFixture(&rearTireFixtureDef);
	
	b2WheelJointDef rearWheelJointDef;
	rearWheelJointDef.enableMotor = true;
	rearWheelJointDef.frequencyHz = 20;
	rearWheelJointDef.dampingRatio = 0.8;
	rearWheelJointDef.Initialize(_body, _rearTireBody, b2Vec2(_rearTireBody->GetWorldCenter().x,
			_rearTireBody->GetWorldCenter().y), b2Vec2(0, 0.5));
	_rearTireJoint = (b2WheelJoint*) world.CreateJoint(&rearWheelJointDef);
	
	// Define front tire
	b2BodyDef frontTireBodyDef;
	frontTireBodyDef.type = b2_dynamicBody;
	b2CircleShape frontTireShape;
	if (specs->getID() == 2) { // Toy car
		frontTireBodyDef.position.Set((posx + 38.0f) / PTM_RATIO,(posy + 38.0f) / PTM_RATIO);
		frontTireShape.m_radius = 20.0f / PTM_RATIO;
		this->_frontTireSprite->setScale(1.2f, 1.2f);
	}
	else if (specs->getID() == 1) { // Formula
		frontTireBodyDef.position.Set((posx + 44.0f) / PTM_RATIO,(posy + 16.0f) / PTM_RATIO);
		frontTireShape.m_radius = 13.0f / PTM_RATIO;
		this->_frontTireSprite->setScale(0.87f, 0.87f);
	}
	else {
		frontTireBodyDef.position.Set((posx + 55.0f) / PTM_RATIO,(posy + 12.0f) / PTM_RATIO);
		frontTireShape.m_radius = 16.0f / PTM_RATIO;
	}
	
	_frontTireBody = world.CreateBody(&frontTireBodyDef);
	_frontTireBody->SetUserData(_frontTireSprite);

	b2FixtureDef frontTireFixtureDef;
	frontTireFixtureDef.shape = &frontTireShape;
	frontTireFixtureDef.density = specs->getBodyDensity() / 10;
	frontTireFixtureDef.friction = specs->getTireFriction();
	frontTireFixtureDef.filter.categoryBits = 0x02;
	frontTireFixtureDef.restitution = 0.0;
	this->_frontTireBody->CreateFixture(&frontTireFixtureDef);

	b2WheelJointDef frontWheelJointDef;
	frontWheelJointDef.enableMotor = true;
	frontWheelJointDef.frequencyHz = 20;
	frontWheelJointDef.dampingRatio = 0.8;
	frontWheelJointDef.Initialize(_body, _frontTireBody, b2Vec2(_frontTireBody->GetWorldCenter().x,
			_frontTireBody->GetWorldCenter().y), b2Vec2(0, 0.5));
	_frontTireJoint = (b2WheelJoint*) world.CreateJoint(&frontWheelJointDef);

}

int Car::countGroundedTires() {

    bool touchedFront = false;
    bool touchedBack = false;

	    for (b2ContactEdge* edge = _frontTireBody->GetContactList(); edge; edge = edge->next) {
	        if (edge->contact->IsTouching()) {
	            touchedFront = true;
	        }
	    }
	    for (b2ContactEdge* edge = _rearTireBody->GetContactList(); edge; edge = edge->next) {
	        if (edge->contact->IsTouching()) {
	            touchedBack = true;
	        }
	    }

	    int result = touchedFront?1:0 + touchedBack?1:0;
	    return result;
    };

void Car::update() {

    float speed = _specs->getTargetSpeed() * velocityModifier;
    float torque = _specs->getMaxTorque() * velocityModifier;

    if (!broken && _fuel <= 0) {
        broken = true;
        std::cout << "Out of fuel!" << std::endl;
    }

    if (broken) {
        _rearTireJoint->SetMotorSpeed(0);
        _rearTireJoint->SetMaxMotorTorque(0);
        _frontTireJoint->SetMotorSpeed(0);
        _frontTireJoint->SetMaxMotorTorque(0);
        return; // Don't move a broken car
    }

    if (velocityModifier > 1.0f) {
    	_body->ApplyLinearImpulse(b2Vec2(1.2f*velocityModifier, 0), _body->GetWorldCenter(), true);
    }


    if (!_reversing && !_accelerating) {
        _rearTireJoint->SetMotorSpeed(0);
        _rearTireJoint->SetMaxMotorTorque(0);
        _frontTireJoint->SetMotorSpeed(0);
        _frontTireJoint->SetMaxMotorTorque(0);
    }

	if (_reversing) {
		_rearTireJoint->SetMotorSpeed(-speed);
		_rearTireJoint->SetMaxMotorTorque(torque);
		_frontTireJoint->SetMotorSpeed(-speed);
		_frontTireJoint->SetMaxMotorTorque(torque);
        _fuel -= 0.001;
	}

	if (_accelerating) {
		_rearTireJoint->SetMotorSpeed(speed);
		_rearTireJoint->SetMaxMotorTorque(torque);
		_frontTireJoint->SetMotorSpeed(speed);
		_frontTireJoint->SetMaxMotorTorque(torque);
        _fuel -= 0.001;
	}


    for (b2ContactEdge* edge = _headBody->GetContactList(); edge; edge = edge->next) {
        if (edge->contact->IsTouching()) {
            float impulse = edge->contact->GetManifold()->points[0].normalImpulse;
            if (impulse > 0.5) {
                broken = true; // Driver broke his head
            }
        }
    }
    
    if (countGroundedTires() <= 0) { 
        // Neither tire is colliding with anything (the ground)
        gameDamage += 1;
    }
    else if ((_accelerating || _reversing) && std::abs(oldPos.x - _body->GetPosition().x) < 0.0001f) {
        // We have not moved (despite trying) much since the last call -> could be stuck
        gameDamage += 5;
    }
    else gameDamage = 0;

    oldPos = _body->GetPosition();

    if (gameDamage > 1000) {
        gameDamage = 0;

        std::cout << "The car is stuck! (ending game)" << std::endl;
        broken = true;
    }

    if (effectLifetime > 0)
        effectLifetime -= 1;
    else
        resetBoosts();

}

void Car::balance(bool counterclockwise) {

        if (broken)
            return;

        // Disallow nudging if fully grounded, amplify if flying (vary based on mass)
		float nudge = pow((2-countGroundedTires()), 2)* 1.5f * _specs->getBodyDensity();

		float direction = 0; // [-1, 0, 1]
		float v0 = _body->GetAngularVelocity(); // Old rotational intertia

		if (counterclockwise)
			nudge = -nudge; // Balance direction

		if (nudge != 0)
			direction = nudge / std::abs(nudge);

		float newVelocity = v0 + direction * 0.1; // Test value

        // If already spinning like crazy, only allow a counteracting nudge
		if ((std::abs(v0) <= 3.f) || (std::abs(newVelocity) < std::abs(v0))) {
			_body->ApplyAngularImpulse(nudge, true);
		}

    }

void Car::velocityBoost(float coefficient, float durationInTicks) {
	resetBoosts();
	velocityModifier = coefficient;
	effectLifetime = durationInTicks;
}

void Car::pickupEffects(std::vector<float> effects, int duration){
	resetBoosts();
	_fuel = std::min(effects[0] + _fuel, _fuelTank);
	velocityModifier = effects[1];
	densityModifier = effects[2];
	frictionModifier = effects[3];
	effectLifetime = duration ;
}
    /*
    // These require more work to implement
    void frictionBoost(float coefficient, float durationInTicks) {
    	resetBoosts();
    	frictionModifier = coefficient;
    	effectLifetime = durationInTicks;
    }

    void densityBoost(float coefficient, float durationInTicks) {
    	resetBoosts();
    	densityModifier = coefficient;
    	effectLifetime = durationInTicks;
    }
    */

Car& Car::operator=(const Car& other){

    _specs = other._specs;

	_accelerating = other._accelerating;
	_reversing = other._reversing;
	_fuel = other._fuel;

    oldPos = other.oldPos;
    gameDamage = other.gameDamage;
    broken = other.broken;

	_bodySprite = other._bodySprite;
	_frontTireSprite = other._frontTireSprite;
	_driverSprite = other._driverSprite;
	_rearTireSprite = other._rearTireSprite;

	_frontTireBody = other._frontTireBody;
	_rearTireBody = other._rearTireBody;
	_body = other._body;
	_headBody = other._headBody;

	_neckRevoluteJoint = other._neckRevoluteJoint;

	tireTex = other.tireTex;
	driverTex = other.driverTex;
	carTex = other.carTex;

    velocityModifier = other.velocityModifier;
    densityModifier = other.densityModifier;
    frictionModifier = other.frictionModifier;
    effectLifetime = other.effectLifetime;

	return *this;
}


