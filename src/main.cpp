/*
 * main.cpp
 *
 *  Created on: 14.11.2016
 *      Author: karri
 */

#include <iostream>
#include "SFML/Graphics.hpp"
#include "screen.hpp"
#include "screen_game.hpp"
#include "screen_menu.hpp"

int main()
{
	
    //Applications variables
    std::vector<screen*> Screens;
    int screen = 0;

    //Window creation
    sf::RenderWindow App(sf::VideoMode(640, 480, 32), "HillSide Racing 3", sf::Style::Close);
    App.setFramerateLimit(60);

    //Mouse cursor no more visible
    App.setMouseCursorVisible(true);

    //Screens preparations
    screen_menu menu;
    Screens.push_back(&menu);
    //Main loop
    while (screen >= 0)
    {
        screen = Screens[screen]->Run(App);

    }
    return EXIT_SUCCESS;
}



