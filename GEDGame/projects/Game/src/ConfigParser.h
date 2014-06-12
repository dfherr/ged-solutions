#pragma once
#include <string>

class ConfigParser
{
public:
	ConfigParser();
	~ConfigParser();

	void load(std::string);
	
	std::string getTerrainHeightPath();
	std::string getTerrainColorPath();
	std::string getTerrainNormalPath();

	float getTerrainWidth();
	float getTerrainDepth();
	float getTerrainHeight();

private:


	std::string terrainHeightPath;
	std::string terrainColorPath;
	std::string terrainNormalPath;

	float terrainWidth; 
	float terrainDepth; 
	float terrainHeight;
};

extern ConfigParser g_configParser;