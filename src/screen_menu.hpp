/*
 * screen_menu.hpp
 *
 *  Created on: Nov 17, 2016
 *      Author: karri
 */

#ifndef SRC_SCREEN_MENU_HPP_
#define SRC_SCREEN_MENU_HPP_

#include <iostream>
#include "screen.hpp"
#include "TGUI/TGUI.hpp"

#include "SFML/Graphics.hpp"

class screen_menu : public screen
{
private:

public:
    screen_menu(void);

    /*
     * Reads the high scores from file scores.txt
     */
    std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> getScores();

    /*
     * Writes the scores into a file
     */
    void setScores(std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> s);

    /*
     * Checks if score is enough to get to highscores and if it is, then adds it
     */
    bool set_a_score (int stage, int mode, int toAdd);

    /*
     * Updates the scores displayed accroding to which modes and stages are chosen
     */
    void updateScores(unsigned stageIndex, unsigned modeIndex, tgui::ListBox::Ptr scores);

    /*
     * Asks user for his/her name if the play made it to the highscores
     */
    std::string askForName();

    //Plays a click sound when tab is selected
    void playClickSound();

    /*
     *The main function
     */
    virtual int Run(sf::RenderWindow &App);
};


#endif /* SRC_SCREEN_MENU_HPP_ */
