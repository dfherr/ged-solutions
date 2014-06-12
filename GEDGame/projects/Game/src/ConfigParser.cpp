#include "ConfigParser.h"
#include <fstream>
#include <iostream>

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
				ConfigParser::terrainWidth = atof(word.c_str());
			} else if (word.compare("TerrainDepth") == 0)
			{
				*fs >> word;
				ConfigParser::terrainDepth = atof(word.c_str());
			} else if (word.compare("TerrainHeight") == 0)
			{
				*fs >> word;
				ConfigParser::terrainHeight = atof(word.c_str());
			} else
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

	
	std::string ConfigParser::getTerrainHeightPath()
	{
		return this->terrainHeightPath;
	}

	std::string ConfigParser::getTerrainColorPath()
	{
		return this->terrainColorPath;
	}

	std::string ConfigParser::getTerrainNormalPath()
	{
		return this->terrainNormalPath;
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