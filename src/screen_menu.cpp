/*
 * screen_menu.cpp
 *
 *  Created on: Nov 17, 2016
 *      Author: karri
 */

#include <iostream>
#include <fstream>
#include <tuple>
#include <memory>

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "TGUI/TGUI.hpp"

#include "screen.hpp"
#include "screen_menu.hpp"
#include "screen_game.hpp"

TGUI_IMPORT_LAYOUT_BIND_FUNCTIONS

const int maxHighScoreSize = 10;

screen_menu::screen_menu(void){
	if (!std::ifstream("scores.txt")){
		std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> all;
		std::vector<std::vector<std::tuple<std::string, int>>> mid;
		std::vector<std::tuple<std::string, int>> lines;
		for (int i = 0; i < 3; i++){
			for (int j = 0; j<3; j++){
				mid.push_back(lines);
			}
			all.push_back(mid);
			mid.clear();
		}
		setScores(all);
	}
}

std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> screen_menu::getScores() {
	std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> all;
	std::vector<std::vector<std::tuple<std::string, int>>> mid;
	std::vector<std::tuple<std::string, int>> s;
	std::string line;
	std::ifstream file {"scores.txt"};

	if (file.is_open()){
		while(getline(file,line)){
			if(line[0] == '%'){
				mid.push_back(s);
				s.clear();
			}
			else if (line[0] == '?'){
				mid.push_back(s);
				all.push_back(mid);
				s.clear();
				mid.clear();
			}
			else{
				size_t pos = line.find(':');
				std::string who = line.substr(0, pos);
				std::string t = line.erase(0, pos + 1);
				int score = std::stoi(t);
				s.push_back(std::make_tuple(who, score));
			}
		}
		mid.push_back(s);
		all.push_back(mid);
		s.clear();
		mid.clear();
		file.close();
	}
	return all;
  }

bool screen_menu::set_a_score(int stage, int mode, int score){
	std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> former = getScores();
	//Get the right list
	std::vector<std::tuple<std::string, int>> list = former.at(stage).at(mode);

	for (auto i = list.begin(); i != list.end(); i++){
		if (score > std::get<1>(*(i))){
			std::string who = askForName();
			list.insert(i, std::make_tuple(who, score));
			if (list.size() > maxHighScoreSize) list.pop_back();
			former.at(stage).at(mode) = list;
			setScores(former);
			return true;
		}
	}

	if (list.size() < 10){
			std::string who = askForName();
			list.push_back(std::make_tuple(who, score));
			if (list.size() > maxHighScoreSize) list.pop_back();
			former.at(stage).at(mode) = list;
			setScores(former);
			return true;
		}
	return false;
}

void screen_menu::setScores(std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> s){
	
	if (std::ifstream("scores.txt")) remove("scores.txt");
	std::ofstream file {"scores.txt"};
	if (file.is_open()){
		for (auto ite = s.begin(); ite != s.end();++ite){
			for (auto it = (*ite).begin(); it != (*ite).end();++it){
				for (auto i = (*it).begin(); i != (*it).end();++i){
					file << std::get<0>(*(i)) + ':' + std::to_string(std::get<1>(*(i)))<<'\n';
				}
				if (it != (*ite).end() - 1) file<<"%"<<'\n';
			}
			if (ite != s.end() - 1) file <<"?"<<'\n';
		}
		file.close();
	}
	else std::cout << "Unable to open file"<<std::endl;
}

void screen_menu::updateScores(unsigned stageIndex, unsigned modeIndex, tgui::ListBox::Ptr scores){
	std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> scoreList = getScores();
	scores->removeAllItems();
	if (stageIndex >= scoreList.size()){
		if (scoreList.empty()){
			scores->addItem("No highscores");
			scores->addItem("made yet");
		}
		else scores->addItem("Corrupted save file");
	}
	else if(modeIndex >= scoreList[stageIndex].size()) {
		if (scoreList.at(stageIndex).empty()) {
			scores->addItem("No highscores");
			scores->addItem("made yet");
		}
		else scores->addItem("Corrupted save file");
	}
	else if (scoreList.at(stageIndex).at(modeIndex).empty()){
		scores->addItem("No highscores");
		scores->addItem("made yet");
	}
	else{
		std::vector<std::vector<std::tuple<std::string, int>>> chosenStage = scoreList.at(stageIndex);
		std::vector<std::tuple<std::string, int>> chosen = chosenStage.at(modeIndex);
		for (auto it = chosen.begin(); it != chosen.end(); it++){
			std::string who = std::get<0>(*(it));
			int score = std::get<1>(*(it));
			scores->addItem(who + " : " + std::to_string(score));
		}
	}
}

std::string screen_menu::askForName() {
	sf::RenderWindow window(sf::VideoMode(300,150), "Highscores");
	tgui::Gui gui(window);
	tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("widgets/Black.txt");
	gui.setFont("fonts/FreeMono.ttf");

	tgui::Label::Ptr label = theme->load("Label");
	label->setPosition(10,10);
	label->setText("You made a highscore!");
	gui.add(label);

	tgui::Label::Ptr label2 = theme->load("Label");
	label2->setPosition(30,30);
	label2->setText("Please write your name");
	gui.add(label2);

	tgui::EditBox::Ptr editBox = theme->load("EditBox");
	editBox->setPosition(10, 65);
	editBox->setSize(280, 30);
	editBox->setMaximumCharacters(15);
	editBox->setText("Unnamed player");
	gui.add(editBox);

	tgui::Button::Ptr readyButton = theme->load("Button");
	readyButton->setPosition(110, 105);
	readyButton->setSize(80, 40);
	readyButton->setText("Confirm");
	gui.add(readyButton);

	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed){
				window.close();
				return "Unnamed player";
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left){
				if (readyButton->mouseOnWidget(event.mouseButton.x, event.mouseButton.y)){
					std::string value = editBox->getText();
					window.close();
					if (value == "") return "Unnamed player";
					else return value;
				}
			}
			gui.handleEvent(event);
		}
		window.clear(sf::Color(128,128,128));
		gui.draw();
		window.display();
	}

	//Normally not reaching this
	return "";
}

//Play one
void screen_menu::playClickSound(){
	sf::SoundBuffer _click;
	sf::Sound _clickSound;

	_click.loadFromFile("audio/click.wav");
	_clickSound.setBuffer(_click);

	_clickSound.setVolume(100);
	_clickSound.play();
}


int screen_menu::Run(sf::RenderWindow &App)
{
	//Setup sounds and music

	sf::SoundBuffer _play;
	sf::Sound _playSound;

	sf::SoundBuffer _menu;
	sf::Sound _menuMusic;

	_play.loadFromFile("audio/play.wav");
	_playSound.setBuffer(_play);

	_menu.loadFromFile("audio/Caketown1_matthewPablo.wav");
	_menuMusic.setBuffer(_menu);
	_menuMusic.setLoop(true);

	//_clickSound.setVolume(100);



	tgui::Gui gui(App);
	tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("widgets/Black.txt");
	bool Running = true;

	tgui::Button::Ptr playButton = theme->load("Button");
	playButton->setPosition(150,400);
	playButton->setSize(100,50);
	playButton->setText("Play");
	gui.add(playButton);

	tgui::Panel::Ptr mainPanel = theme->load("Panel");
	mainPanel->setSize(400, 480);
	mainPanel->setPosition(0, 0);
	mainPanel->setBackgroundColor(sf::Color(128,128,128));
	mainPanel->add(playButton);
	gui.add(mainPanel);

	tgui::Panel::Ptr scorePanel = theme->load("Panel");
	scorePanel->setSize(278, 480);
	scorePanel->setPosition(402, 0);
	scorePanel->setBackgroundColor(sf::Color(128,128,128));
	gui.add(scorePanel);

	tgui::Label::Ptr scoreLabel = theme->load("Label");
	scoreLabel->setText("Highscores");
	scoreLabel->setPosition(473, 20);
	scoreLabel->setTextColor(sf::Color::White);
	gui.add(scoreLabel);

	tgui::Label::Ptr mainLabel = theme->load("Label");
	mainLabel->setText("Select your car, level and game mode. Then press the play button!");
	mainLabel->setTextSize(12);
	mainLabel->setPosition(10, 20);
	mainLabel->setTextColor(sf::Color::White);
	gui.add(mainLabel);

	tgui::Label::Ptr stageLabel = theme->load("Label");
	stageLabel->setText("Select level:");
	stageLabel->setPosition(145, 45);
	stageLabel->setTextColor(sf::Color::White);
	gui.add(stageLabel);

	tgui::Tab::Ptr stageMenu = theme->load("Tab");
	stageMenu->setPosition(53, 80);
	stageMenu->add("Basic");
	stageMenu->add("Challenge", false);
	stageMenu->add("Expert", false);
	gui.add(stageMenu);

	tgui::Label::Ptr carLabel = theme->load("Label");
	carLabel->setText("Select your car:");
	carLabel->setPosition(126, 135);
	carLabel->setTextColor(sf::Color::White);
	gui.add(carLabel);

	tgui::Tab::Ptr carMenu = theme->load("Tab");
	carMenu->setPosition(50, 170);
	carMenu->add("Truck");
	carMenu->add("Formula", false);
	carMenu->add("Toy Car", false);
	gui.add(carMenu);

	tgui::Label::Ptr modeLabel = theme->load("Label");
	modeLabel->setText("Select game mode:");
	modeLabel->setPosition(118, 225);
	modeLabel->setTextColor(sf::Color::White);
	gui.add(modeLabel);

	tgui::Tab::Ptr modeMenu = theme->load("Tab");
	modeMenu->setPosition(55,260);
	modeMenu->add("Normal");
	modeMenu->add("Freeride", false);
	modeMenu->add("Time", false);
	gui.add(modeMenu);

	tgui::ListBox::Ptr scores = theme->load("ListBox");
	scores->setPosition(407, 55);
	scores->getRenderer()->setBackgroundColor(sf::Color(90,90,90));
	scores->getRenderer()->setPadding(15);
	scores->setTextSize(16);
	scores->setItemHeight(30);
	scores->setSize(225, 420);
	scores->disable();
	gui.add(scores);

	updateScores(stageMenu->getSelectedIndex(), modeMenu->getSelectedIndex(), scores);
	stageMenu->connect("TabSelected",&screen_menu::updateScores, *this, std::bind(&tgui::Tab::getSelectedIndex, stageMenu), std::bind(&tgui::Tab::getSelectedIndex, modeMenu), scores);
	carMenu->connect("TabSelected",&screen_menu::updateScores, *this, std::bind(&tgui::Tab::getSelectedIndex, stageMenu), std::bind(&tgui::Tab::getSelectedIndex, modeMenu), scores);
	modeMenu->connect("TabSelected", &screen_menu::updateScores, *this, std::bind(&tgui::Tab::getSelectedIndex, stageMenu), std::bind(&tgui::Tab::getSelectedIndex, modeMenu), scores);
	
	stageMenu->connect("TabSelected", &screen_menu::playClickSound, *this);
	carMenu->connect("TabSelected", &screen_menu::playClickSound, *this);
	modeMenu->connect("TabSelected", &screen_menu::playClickSound, *this);

	_menuMusic.play();
	while (Running){
		sf::Event event;
		while (App.pollEvent(event)){
			if (event.type == sf::Event::Closed) return (-1);
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
				if (playButton->mouseOnWidget(event.mouseButton.x, event.mouseButton.y)){
					_menuMusic.stop();
					_playSound.play();
					screen_game game(stageMenu->getSelectedIndex(),carMenu->getSelectedIndex(),modeMenu->getSelectedIndex());
					int stats = game.Run(App);
					if (stats > 0){
						if (set_a_score(stageMenu->getSelectedIndex(), modeMenu->getSelectedIndex(), stats))
							updateScores(stageMenu->getSelectedIndex(), modeMenu->getSelectedIndex(), scores);
					}
					else if (stats < 0) return stats;
				}
			}
			gui.handleEvent(event);
		}
		App.clear();
		gui.draw();
		App.display();
    }

    //Never reaching this point normally, but just in case, exit the application
    return (-1);
}
