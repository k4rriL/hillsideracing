/*
 * terrain.cpp
 *
 *  Created on: 14.11.2016
 *      Author: ln
 */


#include "terrain.hpp"

#include <iostream>
// this one concatenates blocks from the given list
std::vector<float> fromSegments(float lastHeight, std::default_random_engine rng, unsigned points, std::vector<terrain_segment> segmentList){
	std::vector<float> h = std::vector<float>();
	unsigned pos = 0;
	while (pos < points){
		terrain_segment nextBlock = segmentList[(int)(rng() % segmentList.size())];
		for (unsigned i = 0; i < nextBlock.length; i++){
			h.push_back(lastHeight + nextBlock.heights[i] + nextBlock.random_magnitudes[i] * (rng() % 1000) / 1000.0f);
		}
		lastHeight += nextBlock.heights[nextBlock.length-1];
		pos += nextBlock.length;
	}
	return h;
}

// this one generates smooth rolling hills
std::vector<float> cosineDune(float lastHeight, float lastAng, std::default_random_engine rng, unsigned points, float minWL, float maxWL, float minSteep, float maxSteep, float minL, float maxL){
	std::vector<float> h = std::vector<float>();
	unsigned pos = 0;
	while (pos < points){
		float waveLength = minWL + rng()/(float)rng.max()*(maxWL-minWL); // wavelength of the wave, in points
		float waveAmp = minSteep + rng()/(float)rng.max()*(maxSteep-minSteep); // general steepness
		float length = minL + rng()/(float)rng.max()*(maxL-minL);		 // length of this segment, in points
		float startAng = rng()%3==0?lastAng:PI-lastAng;
		float p = lastHeight;
		float a;
		for (unsigned i = 1; i <= length; i++){
			a = startAng + 2*PI*i/waveLength;
			p += -sin(a)*waveAmp;
			h.push_back(p);
		}
		lastHeight = p;
		lastAng = a;
		while (lastAng > PI)
			lastAng -= PI*2;
		while (lastAng <= -PI)
			lastAng += PI*2;
		pos += length;
	}
	return h;
}


Terrain::Terrain(){
	_body = nullptr;
	_cloud.loadFromFile("textures/simpleCloud2.png");
	_cloudSprite.setTexture(_cloud);
	_cloudSprite.setOrigin(sf::Vector2f(_cloud.getSize().x*0.5,_cloud.getSize().y*0.5));
	_cloudSprite.setScale(0.003*_scale,0.003*_scale);
	_startIndex = -1;
	_endIndex = -1;
	_physInitDone = false;
	_gfxInitDone = false;

}

void Terrain::setLevelDetails(int leveltype, int initial_length, float step, unsigned random_seed){
	_levelType = leveltype;
	_step = step;
	_terrainLength = 10;

	_yOffset = 400.0f/_scale;	
	
	_heights = std::vector<float>();
	
	for (int i = 0; i < 10; i++)
		_heights.push_back(_yOffset);
	_rng.seed(random_seed);
	this->genPoints(initial_length);
}

void Terrain::drawTo(sf::RenderWindow &window, sf::View &cam) {
	float left = cam.getCenter().x - cam.getSize().x / 2;
	float right = cam.getCenter().x + cam.getSize().x / 2;		
	int num = 0;

	for(float i : _heights){
		if(left-50 < num*_scale*_step && num*_scale*_step < right+50 && num%2 == 0){
			_cloudSprite.setPosition(num*_scale*_step,i*_scale);
			_cloudSprite.setRotation(num*20.0f);
			window.draw(_cloudSprite);
		}	
		num++;
	}

	for (auto b:_blocks){
		if(left - _scale*_step-30 < b.getPoint(0).x && right + _scale*_step > b.getPoint(1).x){
			window.draw(b);		
		}
	}
}

void Terrain::update(b2World* world, sf::View &cam){
	float left = cam.getCenter().x - cam.getSize().x / 2;
	float right = cam.getCenter().x + cam.getSize().x / 2;
	int leftI = left / _scale / _step;
	int rightI = right / _scale / _step + 1;
	if ((unsigned)(rightI + _blockLength*2) > _heights.size())
		this->genPoints(rightI + _blockLength*20);
	this->buildMesh(world, leftI, rightI);
	this->buildBlocks(leftI-1, rightI);
}

void setBlockPoints(sf::ConvexShape& block, int leftIndex, std::vector<float> heights, float step, float scale){
	block.setPointCount(4);
	block.setPoint(0,sf::Vector2f((leftIndex)*step*scale, heights[leftIndex]*scale));
	block.setPoint(1,sf::Vector2f((leftIndex+1)*step*scale, heights[leftIndex+1]*scale));
	block.setPoint(2,sf::Vector2f((leftIndex+1)*step*scale, heights[leftIndex+1]*scale+1000));
	block.setPoint(3,sf::Vector2f((leftIndex)*step*scale, heights[leftIndex]*scale+1000));
}

void Terrain::genPoints(int toHere){
	unsigned points = abs(toHere - _heights.size()-1);
	std::vector<float> level;
	if (_levelType == 0){
		float lastAng = 0;
		level = cosineDune(_heights.back(), lastAng, _rng, points, 4, 32, 0.2, 0.6, 4, 16); // suitable for level 1, simple, soft and easy, increase max steepness for difficulty
	} else if (_levelType == 1)
		level = fromSegments(_heights.back(), _rng, points, _segments_normal()); // suitable for harder levels
	else {
		level = fromSegments(_heights.back(), _rng, points, _segments_hard());
	}
	
	_heights.insert(_heights.end(), level.begin(), level.end());
}

void Terrain::buildBlocks(int fromHere, int toHere){
	int leftBlockI;
	int rightBlockI;
	sf::Color lightBlue(106,136,169) ;
	
	if (!_gfxInitDone){
		leftBlockI = fromHere<0?0:fromHere;
		rightBlockI = fromHere<0?0:fromHere;
		sf::ConvexShape block;	
		setBlockPoints(block, leftBlockI, _heights, _step, _scale);
		block.setFillColor(lightBlue);
		block.setOutlineThickness(0);
		_blocks.push_back(block);
		rightBlockI++;
		_gfxInitDone = true;
	}
	else {
		leftBlockI = _blocks.front().getPoint(0).x / _scale / _step;
		rightBlockI = _blocks.back().getPoint(1).x / _scale / _step;
	}
	while (leftBlockI > 0 && leftBlockI > fromHere){
		sf::ConvexShape block;
		setBlockPoints(block, leftBlockI-1, _heights, _step, _scale);
		block.setFillColor(lightBlue);
		block.setOutlineThickness(0);
		_blocks.push_front(block);
		leftBlockI--;
	}
	while (leftBlockI < fromHere){
		_blocks.pop_front();
		leftBlockI++;
	}
	while (rightBlockI > toHere){
		_blocks.pop_back();
		rightBlockI--;
	}
	while (rightBlockI < toHere){
		sf::ConvexShape block;
		setBlockPoints(block, rightBlockI, _heights, _step, _scale);
		block.setFillColor(lightBlue);
		block.setOutlineThickness(0);
		_blocks.push_back(block);
		rightBlockI++;
	}
}

void Terrain::buildMesh(b2World* world, int fromHere, int toHere) {
	if (!_physInitDone){ // make the first block and set the start and end index correctly
		b2BodyDef groundBodyDef;
		groundBodyDef.type = b2_staticBody;
		groundBodyDef.position.Set(0.0f, 0.0f);
		_body = world->CreateBody(&groundBodyDef);
		
		_fixtures = std::deque<b2Fixture*>();
		_startIndex = fromHere - (fromHere%_blockLength);
		_endIndex = _startIndex + _blockLength-1;
		
		b2ChainShape shape;
		b2Vec2 vs[_blockLength];
		for (int i=_startIndex; i <= _endIndex; i++) {
			if (i < 0)
				vs[i-_startIndex].Set(i*_step, -100);
			else
				vs[i-_startIndex].Set(i*_step, _heights[i]);
		}
		shape.CreateChain(vs, _blockLength);
		if (_startIndex > 0)
			shape.SetPrevVertex(b2Vec2(_step * (_startIndex-1), _heights[_startIndex-1]));
		else
			shape.SetPrevVertex(b2Vec2(_step * (_startIndex-1), -100));
		if (_endIndex < (int)_heights.size()-1)
			shape.SetNextVertex(b2Vec2(_step * (_endIndex+1), _heights[_endIndex+1]));
		
		b2FixtureDef groundFixture;
		groundFixture.shape = &shape;
		groundFixture.friction = _friction;
		_fixtures.push_back(_body->CreateFixture(&groundFixture));
		_physInitDone = true;
	}
	// forward generation
	while (_startIndex+(int)_blockLength < fromHere){
		b2Fixture* toDelete = _fixtures.front();
		if (toDelete != nullptr){
			_fixtures.pop_front();
			_body->DestroyFixture(toDelete);
			toDelete = nullptr;
		}
			
		_startIndex += _blockLength-1;
	}
	while (_endIndex < toHere){
		int blockStart = _endIndex;
		int blockEnd = blockStart + _blockLength-1;
		b2ChainShape shape;
		b2Vec2 vs[_blockLength];
		for (int i=blockStart; i <= blockEnd; i++) {
			if (i < 0)
				vs[i-blockStart].Set(i*_step, -100);
			else
				vs[i-blockStart].Set(i*_step, _heights[i]);
		}
		shape.CreateChain(vs, _blockLength);
		if (blockStart > 0)
			shape.SetPrevVertex(b2Vec2(_step * (blockStart-1), _heights[blockStart-1]));
		else
			shape.SetPrevVertex(b2Vec2(_step * (blockStart-1), -100));
		if (blockEnd < (int)_heights.size()-1)
			shape.SetNextVertex(b2Vec2(_step * (blockEnd+1), _heights[blockEnd+1]));
		
		b2FixtureDef groundFixture;
		groundFixture.shape = &shape;
		groundFixture.friction = _friction;
		_fixtures.push_back(_body->CreateFixture(&groundFixture));
		_endIndex = blockEnd;
	}

	// backward generation
	while (_endIndex-(int)_blockLength > toHere){
		b2Fixture* toDelete = _fixtures.back();
		if (toDelete != nullptr){
			_fixtures.pop_back();
			_body->DestroyFixture(toDelete);
			toDelete = nullptr;
		}
			
		_endIndex -= _blockLength-1;
	}
	while (_startIndex > fromHere){
		int blockEnd = _startIndex;
		int blockStart = blockEnd - _blockLength + 1;
		b2ChainShape shape;
		b2Vec2 vs[_blockLength];
		for (int i=blockStart; i <= blockEnd; i++) {
			if (i < 0)
				vs[i-blockStart].Set(i*_step, -100);
			else
				vs[i-blockStart].Set(i*_step, _heights[i]);
		}
		shape.CreateChain(vs, _blockLength);
		if (blockStart > 0)
			shape.SetPrevVertex(b2Vec2(_step * (blockStart-1), _heights[blockStart-1]));
		else
			shape.SetPrevVertex(b2Vec2(_step * (blockStart-1), -100));
		if (blockEnd < (int)_heights.size()-1)
			shape.SetNextVertex(b2Vec2(_step * (blockEnd+1), _heights[blockEnd+1]));
		
		b2FixtureDef groundFixture;
		groundFixture.shape = &shape;
		groundFixture.friction = _friction;
		_fixtures.push_front(_body->CreateFixture(&groundFixture));
		_startIndex = blockStart;
	}
}
