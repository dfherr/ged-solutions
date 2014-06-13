#include "ConfigParser.h"
#include <fstream>
#include <iostream>
#include "DXUT.h"
#include "SDKmisc.h"

ConfigParser::ConfigParser()
{
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::load(std::string path)
{
	// generate a new input file stream for the give path
	std::ifstream* fs = new std::ifstream(path);

	// check if the path can be opened
	if (fs->is_open())
	{
		std::string word;

		// go through every word as long as there are words in the file
		while (*fs >> word)
		{
			// check if the read in word is one of our setting specific words
			// if it is, assign the value to the setting
			if (word.compare("TerrainPath") == 0)
			{
				*fs >> word;
				ConfigParser::terrainHeightPath = word;
				*fs >> word;
				ConfigParser::terrainColorPath = word;
				*fs >> word;
				ConfigParser::terrainNormalPath = word;
			}  else if (word.compare("TerrainWidth") == 0)
			{
				*fs >> word;
				ConfigParser::terrainWidth = (float)atof(word.c_str());
			} else if (word.compare("TerrainDepth") == 0)
			{
				*fs >> word;
				ConfigParser::terrainDepth = (float)atof(word.c_str());
			} else if (word.compare("TerrainHeight") == 0)
			{
				*fs >> word;
				ConfigParser::terrainHeight = (float)atof(word.c_str());
			} else if(word.compare("Mesh") == 0){
				*fs >> word;
				if(word.compare("Cockpit") == 0){
					*fs >> word;
					ConfigParser::cockpitMesh = word;
					*fs >> word;
					ConfigParser::cockpitDiffuse = word;
					*fs >> word;
					ConfigParser::cockpitSpecular = word;
					*fs >> word;
					ConfigParser::cockpitGlow = word;
				}else{
					// the mesh identifier isn't correct. throw a error message and go to the next one.
					std::cout << "ERROR: mesh identifier \"" << word << "\" unknown." << std::endl;
				}

			}else
			{
				// the word isn't correct. throw a error message and go to the next one.
				std::cout << "error: \"" << word << "\"" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "error opening the file";
	}

	// close and delete the input file stream
	fs->close();
	delete fs;
}

std::wstring findMediaFilePath(std::string file){
	WCHAR path[MAX_PATH];
	std::wstring wPath(file.begin(), file.end());
	DXUTFindDXSDKMediaFileCch(path, MAX_PATH, wPath.c_str());
	return std::wstring(path);
}


std::wstring ConfigParser::getTerrainHeightPath()
{
	return findMediaFilePath(this->terrainHeightPath);
}

std::wstring ConfigParser::getTerrainColorPath()
{
	return findMediaFilePath(this->terrainColorPath);
}

std::wstring ConfigParser::getTerrainNormalPath()
{
	return findMediaFilePath(this->terrainNormalPath);
}

std::wstring ConfigParser::getCockpitMesh()
{
	
	return findMediaFilePath(this->cockpitMesh);
}
std::wstring ConfigParser::getCockpitDiffuse()
{
	return findMediaFilePath(this->cockpitDiffuse);
}
std::wstring ConfigParser::getCockpitSpecular()
{
	return findMediaFilePath(this->cockpitSpecular);
}
std::wstring ConfigParser::getCockpitGlow()
{
	return findMediaFilePath(this->cockpitGlow);
}

float ConfigParser::getTerrainWidth()
{
	return this->terrainWidth;
}

float ConfigParser::getTerrainDepth()
{
	return this->terrainDepth;
}

float ConfigParser::getTerrainHeight()
{
	return this->terrainHeight;
}

