#pragma once
#include <string>

class ConfigParser
{
public:
	ConfigParser();
	~ConfigParser();

	void load(std::string);
	
	std::wstring getTerrainHeightPath();
	std::wstring getTerrainColorPath();
	std::wstring getTerrainNormalPath();

	std::wstring getCockpitMesh();
	std::wstring getCockpitDiffuse();
	std::wstring getCockpitSpecular();
	std::wstring getCockpitGlow();

	float getTerrainWidth();
	float getTerrainDepth();
	float getTerrainHeight();

private:


	std::string terrainHeightPath;
	std::string terrainColorPath;
	std::string terrainNormalPath;

	std::string cockpitMesh;
	std::string cockpitDiffuse;
	std::string cockpitSpecular;
	std::string cockpitGlow;

	float terrainWidth; 
	float terrainDepth; 
	float terrainHeight;
};

extern ConfigParser g_configParser;