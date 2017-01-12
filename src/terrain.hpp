/*
 * terrain.hpp
 *
 *  Created on: 14.11.2016
 *      Author: ln
 */

#ifndef SRC_TERRAIN_HPP_
#define SRC_TERRAIN_HPP_

#include <deque>
#include <vector>
#include <random>

#include <math.h>

#include "SFML/Graphics.hpp"
#include "Box2D/Box2D.h"

#define PI 3.14159265

typedef struct {
	unsigned length;
	float* heights;
	float* random_magnitudes;
} terrain_segment;

static terrain_segment flat10 = {.length = 10, .heights = (float[]){0,0,0,0,0,0,0,0,0,0}, .random_magnitudes = (float[]){0,0,0,0,0,0,0,0,0,0}};
static terrain_segment bumpy10 = {.length = 10, .heights = (float[]){0.1,0.15,0.2,0.15,0.05,-0.05,-0.15,-0.2,-0.15,-0.1}, .random_magnitudes = (float[]){0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1}};
static terrain_segment random1 = {.length = 1, .heights = (float[]){0}, .random_magnitudes = (float[]){0.15}};
static terrain_segment hole4 = {.length = 4, .heights = (float[]){-0.1, 3, 3, -0.1}, .random_magnitudes = (float[]){0.1,0.4,0.4,0.1}};
static terrain_segment rift20 = {.length = 20, .heights = (float[]){-0.015,-0.03,-0.06,-0.12,-0.25, -0.5, -1, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0}, .random_magnitudes = (float[]){0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,3,3,3,0}};

static terrain_segment sine10(float start, float step, float magnitude, float random_mg){
	terrain_segment seg;
	seg.length = 10;
	seg.heights = new float[10];
	seg.random_magnitudes = new float[10];
	float base = sin(start);
	for (int i = 1; i <= 10; i++){
		seg.heights[i-1] = (sin(start+step*i)-base)*magnitude;
		seg.random_magnitudes[i-1] = random_mg;
	}
	return seg;
}

class Terrain
{
public:
	Terrain();

	std::vector<float> getHeights() {return _heights;}
	float getStep() {return _step;}
	float getScale() {return _scale;}
	int random() {return int(_rng());}
	void setBox2DScale(float sc) {_scale = sc;};

	void setLevelDetails(int leveltype, int minLen, float step, unsigned random_seed);
	void genPoints(int toHere);
	void drawTo(sf::RenderWindow &window, sf::View & cam);
	void update(b2World* w, sf::View & cam);
	void buildMesh(b2World* w, int i0, int i1);
	void buildBlocks(int i0, int i1);

	void setFriction(float f){
		this->_friction = f;
	}
	
private:
	static const std::vector<terrain_segment> _segments_normal() { return {flat10, bumpy10, random1, sine10(-PI/2, PI/10, 3.5, 0.3), sine10(PI/2, PI/10, 1.5, 0.3), sine10(PI/2, PI/20, 6, 0)}; }
	static const std::vector<terrain_segment> _segments_hard() { return {hole4, rift20, bumpy10, random1, sine10(-PI/2, PI/10, 3.5, 0.4), sine10(PI/2, PI/10, 1.5, 0.4), sine10(PI/2, PI/20, 6, 0.1)}; }
		
	std::default_random_engine _rng;
	
	int _levelType;
	float _yOffset;
	float _scale = 32; // ratio of box2d meters to pixels
	unsigned _terrainLength = 512;
	unsigned _blockLength = 16;
	float _step = 1.0f;
	std::vector<float> _heights;
	std::deque<sf::ConvexShape> _blocks;
	
	std::deque<b2Fixture*> _fixtures;
	int _startIndex; // the first index in the current mesh
	int _endIndex; // the last index in the current mesh

	bool _physInitDone;
	bool _gfxInitDone;
	
	sf::Texture _cloud;
	sf::Sprite _cloudSprite;
	
	float _friction = 0.5f;
	
	//this body will contain all physics for the track
	b2Body* _body;
};

#endif /* SRC_TERRAIN_HPP_ */
