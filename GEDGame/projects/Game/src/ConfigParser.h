#pragma once
#include <string>

class ConfigParser
{
public:
	ConfigParser();
	~ConfigParser();

	struct Color{
		float r, g, b;
	};

	void load(std::string);
	
	// getters for all the private members
	float getSpinning();
	float getSpinSpeed();

	Color getBackgroundColor();
	std::string getTerrainPath();

	float getTerrainWidth();
	float getTerrainDepth();
	float getTerrainHeight();

private:
	float spinning; 
	float spinSpeed;

	Color backgroundColor;
	std::string terrainPath;

	float terrainWidth; 
	float terrainDepth; 
	float terrainHeight;
};

