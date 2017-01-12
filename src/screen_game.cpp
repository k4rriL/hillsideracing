/*
 * screen_game.cpp
 *
 *  Created on: Nov 17, 2016
 *      Author: karri
 */

#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <chrono>
#include <thread>

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Box2D/Box2D.h"

#include "screen.hpp"
#include "car.hpp"
#include "screen_game.hpp"
#include "pickup.hpp"

#include "SFMLDebugDraw.cpp"

const float SCALE = 32.f;
const bool DEBUG = false;
const int MAP_LENGTH = 512;

screen_game::screen_game(int level, int car, int mode) {
	this->mode = mode;
	this->carNumber = car;
	this->level = level;
	_terrain.setBox2DScale(SCALE);
	_terrain.setLevelDetails(this->level, MAP_LENGTH, 1.f, time(NULL));
}

int screen_game::Run(sf::RenderWindow &App) {
	sf::Event Event;
	bool Running = true;
	float _scale = _terrain.getScale();
	float _step = _terrain.getStep();

	//Sounds..
	sf::SoundBuffer sound1;
	if(!sound1.loadFromFile("audio/starSound.wav"))
		return -1;

	sf::SoundBuffer sound2;
	if(!sound2.loadFromFile("audio/carSound.wav"))
		return -1;

	sf::SoundBuffer music1;

	if(mode == 2){ //if mode is time mode
		if(!music1.loadFromFile("audio/CircusDilemma.wav"))
		return -1;
	} else {
		if(!music1.loadFromFile("audio/DayAndNight_edwinnington.wav"))
		return -1;
	}
	

	//Creating Pickups' Sounds outside pickups. 
	sf::Sound starSound;
	starSound.setBuffer(sound1);
	starSound.setVolume(20);
	
	sf::Sound carSound;
	carSound.setBuffer(sound2);
	carSound.setVolume(50);
	carSound.setLoop(true);

	sf::Sound music;
	music.setBuffer(music1);
	music.setVolume(100);
	music.setLoop(true);

	music.play(); //starts to play background music

	//Creating textutes used in pickups outside the actual pickups 
	//so that pickups can be created with for-loop and textures are not lost.  
	sf::Texture coinTexture;
	coinTexture.loadFromFile("textures/star1.png");

	sf::Texture energyBoostTexture;
	energyBoostTexture.loadFromFile("textures/salama1.png");

	sf::Texture speedBoostTexture;
	speedBoostTexture.loadFromFile("textures/wings1.png");

	sf::Texture finishLine;
	finishLine.loadFromFile("textures/finish_line.png");
	sf::Sprite finishLineSprite;
	if (mode == 0){
		finishLineSprite.setTexture(finishLine);
		finishLineSprite.setScale(0.4f, 0.4f);
		finishLineSprite.setOrigin(173.f / 2, 325.f / 2);
		float y = _terrain.getHeights().at(MAP_LENGTH - 10);
		finishLineSprite.setPosition((MAP_LENGTH - 10) * SCALE, y * SCALE-40);
	}

	// Make and place the pickups

	std::vector<Pickup> PickupVec; //Vector that stores all the pickups
	unsigned num = 0;

	//Loop crates pickups into random positions.

	for (float i : _terrain.getHeights()) {
		if (mode == 0 && num >= MAP_LENGTH - 40) {
			num++;
			break;
		}
		if (_terrain.random() % 5 == 0 && num > 10) { //probability for normal pickup to appear
			Pickup pickup = Pickup(&coinTexture, &starSound);
			pickup.setPos(num * _scale * _step, i * _scale - 40);
			PickupVec.push_back(pickup);
		} else if(_terrain.random() % 14 == 0 && num % 5 == 0 && num > 10) { //propability for energyBoost to appear
			std::vector<float> v = {10,1,1,1};
			Pickup pickup1 = Pickup(&energyBoostTexture, &starSound, 0.15, 150, v, 25);
			pickup1.setPos(num * _scale * _step, i * _scale - 40);
			PickupVec.push_back(pickup1);
		} else if(_terrain.random() % 22 == 0 && num % 5 == 0 && num > 10) { //probability for speedBoost to appear
			std::vector<float> v = {0,5,1,1};
			Pickup pickup1 = Pickup(&speedBoostTexture, &starSound, 0.15, 300, v, 25);
			pickup1.setPos(num * _scale * _step, i * _scale - 40);
			PickupVec.push_back(pickup1);
		}
		num++;
	}

	int score = 0;

	// Score display

	sf::Font arial;
	arial.loadFromFile("fonts/FreeMono.ttf");

	std::ostringstream ssScore;
	ssScore << "Score: " << score;

	sf::Text lblScore;
	lblScore.setCharacterSize(30);
	lblScore.setPosition(10, 5);
	lblScore.setFont(arial);
	lblScore.setString(ssScore.str());

	float aika = 120.0; // Seconds of game time

	// Time display

	std::ostringstream ssTime;

	sf::Text lblTime;
	lblTime.setCharacterSize(30);
	lblTime.setPosition(10, 35);
	lblTime.setFont(arial);
	lblTime.setString(ssTime.str());

	std::ostringstream ssFps;
	sf::Text lblFps;
	if (DEBUG) {
		lblFps.setCharacterSize(14);
		lblFps.setPosition(10, 130);
		lblFps.setFont(arial);
		lblFps.setCharacterSize(10);
		lblFps.setString(ssFps.str());
	}

	sf::Clock clock;
	std::ostringstream ssClock;
	sf::Text lblClock;

	lblClock.setCharacterSize(30);
	lblClock.setPosition(10, 35);
	lblClock.setFont(arial);
	lblClock.setString(ssClock.str());

	std::ostringstream speed;
	sf::Text velocity;
	velocity.setCharacterSize(30);
	if (mode == 1) velocity.setPosition(10, 35);
	else velocity.setPosition(10, 65);
	velocity.setFont(arial);
	velocity.setString(speed.str());


	// World, terrain and car setup

	b2Vec2 Gravity(b2Vec2(0.f, 9.81f));
	b2World World(Gravity);

	//_terrain.createMesh(&World);

	CarSpecs* specs;

	if (carNumber == 1)
		specs = new CarSpecs(4.f, 100.f, 50.f, 3.f, "textures/formula1.png", 14.f, 1); // formula
	else if (carNumber == 2)
		specs = new CarSpecs(0.8f, 120.f, 30.f, 3.f, "textures/toyCar1.png", 35.f, 2); // toy car

	else specs = new CarSpecs(0.8f, 700.f, 25.f, 5.f, "textures/van.png", 15.f, 0); // truck

	Car* car = new Car(World, 200.f, 330.f, specs);

	//Boost stuff
	bool carBoosted = false;
	sf::Clock Boostclock;

	// Camera stuff
	sf::View cam;
	cam = App.getView();
	sf::View ui(sf::FloatRect(0, 0, 640, 480));

	// Making game's background
	_backGround.loadFromFile("textures/nightSky1.png");
	_sprite.setTexture(_backGround);
	_sprite.scale(0.15, 0.18);
	_sprite.setOrigin(sf::Vector2f(_backGround.getSize().x * 0.5, _backGround.getSize().y * 0.5));


	if (DEBUG) {
		SFMLDebugDraw * dd = new SFMLDebugDraw(App);
		dd->SetFlags(0x00);
		dd->AppendFlags(0x01); // shape
		dd->AppendFlags(0x02); // joint
		//dd->AppendFlags(0x04); // aabb (axis aligned bounding box)
		//dd->AppendFlags(0x08); // pair
		dd->AppendFlags(0x10); // centerOfMass
		World.SetDebugDraw(dd);
	}


	//Time related stuff

	bool pause = false;
	sf::Clock pauseClock;
	float paused = 0.0; // Seconds paused

	sf::Time lastT = clock.getElapsedTime();
	sf::Time nowT;
	sf::Clock  gameOverClock;
	float timeLeft = aika;
	float played = 0.0;
	bool atFinish = false;

	//The main loop
	
	while (Running) {
		while (App.pollEvent(Event)) {

			// Window closed
			if (Event.type == sf::Event::Closed){
				music.stop();
				return (-1);
			}


			//Key pressed
			if (Event.type == sf::Event::KeyPressed) {

				if (pause) {
					pause = false;
					paused += pauseClock.getElapsedTime().asSeconds();
					break;
				}

				switch (Event.key.code) {
				case sf::Keyboard::Escape:
					return 0;
					break;
				case sf::Keyboard::P:
					pause = true;
					pauseClock.restart();
					break;
				case sf::Keyboard::Up:
                    car->balance(true);
					break;
				case sf::Keyboard::Down:
					car->balance(false);
					break;
				case sf::Keyboard::Left:
					car->reverse(true);
					break;
				case sf::Keyboard::Right:
					car->accelerate(true);
					break;
				default:
					break;
				}
			} else if (Event.type == sf::Event::KeyReleased) {
				switch (Event.key.code) {
				case sf::Keyboard::Left:
					car->reverse(false);
					break;
				case sf::Keyboard::Right:
					car->accelerate(false);
					break;
				default:
					break;
				}
			}
		}
		App.clear();
		App.setView(cam);

		// Moving the camera and drawing the background

		sf::Vector2f carDistanceFromCenter =
				(sf::Vector2f) App.mapCoordsToPixel(
						car->getSprite()->getPosition()) - cam.getSize() / 2.0f;
		cam.move(carDistanceFromCenter / 2.0f);

		//Draw background
		_sprite.setPosition(cam.getCenter());
		App.draw(_sprite);
		
		//Update terrain while moving
		_terrain.update(&World, cam);
		while (num < _terrain.getHeights().size()) {
			float i = _terrain.getHeights()[num];
			if (mode == 0 && num >= MAP_LENGTH - 40) {
				num++;
				break;
			}
			if (_terrain.random() % 5 == 0 && num > 10) {
				Pickup pickup = Pickup(&coinTexture, &starSound);
				pickup.setPos(num * _scale * _step, i * _scale - 40);
				PickupVec.push_back(pickup);
			} else if(_terrain.random() % 9 == 0 && num % 5 == 0 && num > 10) {
				std::vector<float> v = {10,1,1,1};
				Pickup pickup1 = Pickup(&energyBoostTexture, &starSound, 0.15, 150, v, 25);
				pickup1.setPos(num * _scale * _step, i * _scale - 40);
				PickupVec.push_back(pickup1);
			} else if(_terrain.random() % 22 == 0 && num % 5 == 0 && num > 10) {
				std::vector<float> v = {0,5,1,1};
				Pickup pickup1 = Pickup(&speedBoostTexture, &starSound, 0.15, 300, v, 25);
				pickup1.setPos(num * _scale * _step, i * _scale - 40);
				PickupVec.push_back(pickup1);
			}
			num++;
		}
		
		_terrain.drawTo(App, cam);

		// Draw pickup items
		for (auto i = PickupVec.begin(); i != PickupVec.end(); i++)
			(i)->drawTo(App);

		App.draw(finishLineSprite);

		// Draw the different bodies
		for (b2Body * b = World.GetBodyList(); b != NULL; b = b->GetNext()) {
			if (b->GetUserData() != NULL) {
				sf::Sprite* node = (sf::Sprite*) b->GetUserData();
				node->setPosition(b->GetPosition().x * SCALE,
						b->GetPosition().y * SCALE);
				node->setRotation(57.2957795130824f * (b->GetAngle()));
				App.draw(*node);
			}
		}

		//Advance world and apply effects of pressed keys
		World.Step(1 / 60.f, 10, 8);
		car->update();
		if (pause) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			continue; // Skip the rest
		}

		//Draw debug data
		if (DEBUG) World.DrawDebugData();

		//Draw UI elements and stuff that needs to be static
		App.setView(ui);

		//Advance clocks
		played = clock.getElapsedTime().asSeconds() - paused;
		if (mode == 2) timeLeft = aika - played;

		if (mode == 0 && car->getSprite()->getPosition().x/SCALE > MAP_LENGTH - 10){
			if (!atFinish) gameOverClock.restart();
			atFinish = true;
			_gameOver = true;
			car->breakCar();
		}

		//Change timeLeft clock accordingly if on TIME mode
		ssTime.str("");
		if (timeLeft <= 0.0) {
			lblTime.setCharacterSize(70);
			lblTime.setPosition(150, 200);
			lblTime.setColor(sf::Color::Red);
			ssTime << "Game Over!";
			if (!_gameOver){
				gameOverClock.restart();
				_gameOver = true;
			}
		}
		else if (timeLeft < 5.0) {
			//lblTime.setCharacterSize(70);
			//lblTime.setPosition(150, 200);
			lblTime.setColor(sf::Color::Red);
			ssTime << "Time: " << int(timeLeft * 10) / 10.0;
		}
		
		else ssTime << "Time: " << int(timeLeft * 10) / 10.0;
		lblTime.setString(ssTime.str());

		//If the driver's head has touched the ground, do game over things
		if (!_gameOver) {
			_gameOver = car->isBroken();
			if (_gameOver) gameOverClock.restart();
		}
		else {
			if (atFinish){
				lblTime.setString("Victory!");
				lblTime.setCharacterSize(70);
				lblTime.setPosition(150, 200);
				lblTime.setColor(sf::Color::Green);
			}
			else if (timeLeft > 0.0){
				lblTime.setString("Game Over!");
				lblTime.setCharacterSize(70);
				lblTime.setPosition(150, 200);
				lblTime.setColor(sf::Color::Red);
			}
			if (gameOverClock.getElapsedTime().asSeconds() > 3.0){
				if (mode == 0 && !car->isBroken()) score = score + ((MAP_LENGTH/4)-played)*30;
				else score = score + (car->getSprite()->getPosition().x*2)/SCALE;
				if (score <= 0) return 1;
				else return score;
			}
			App.draw(lblTime);
		}
		if (mode == 2) App.draw(lblTime);


		//If on normal mode, draw the elapsed time
		ssClock.str("");
		ssClock <<"Time: " << int(played * 10) / 10.0;
		lblClock.setString(ssClock.str());
		if (mode == 0) App.draw(lblClock);

		std::string fuel = std::to_string(int(car->getFuel() * 100)) + "% battery";
		while (fuel.size() < 8)
			fuel = fuel + " ";

		//Draw velocity
		speed.str("");
		speed << fuel << "\n" << std::abs(int(car->getBody()->GetLinearVelocity().x*5)) << " km/h";
		velocity.setString(speed.str());
		App.draw(velocity);

		//Draw fps
		if (DEBUG) {
			nowT = clock.getElapsedTime();
			float fps = 1 / (nowT - lastT).asSeconds();
			lastT = nowT;
			ssFps.str("");
			ssFps << fps << "fps";
			lblFps.setString(ssFps.str());
		}
		if (DEBUG) App.draw(lblFps);

		//Count and display score and use pickup effects to car
		for (unsigned int i = 0; i < PickupVec.size(); i++) {
			if (PickupVec[i].isColliding(car)) {
				PickupVec[i].setPos(500000, 500000); //changes pickups position out of screen
				score = score + PickupVec[i].getPoints();
				ssScore.str("");
				ssScore << "Score: " << score;
				lblScore.setString(ssScore.str());
				PickupVec[i].getNoise()->play();
				PickupVec[i].effectCar(car);
				Boostclock.restart();
				carBoosted = true;
			}
		}
		App.draw(lblScore);
		//counts how long pickup's effect should last
		if(carBoosted && gameOverClock.getElapsedTime().asSeconds() > car->getEffectLifetime()*1.0){
			car-> resetBoosts();
			carBoosted = false;
		}
		//plays cars drving noise when key is pressed and stops when its not pressed
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            		sf::Sound::Status soundStatus = carSound.getStatus();
           		if (soundStatus == sf::Sound::Stopped) carSound.play();
        	} else carSound.stop();

		App.display();
	}

	//Never reaching this point normally, but just in case, exit the application
	return -1;
}
