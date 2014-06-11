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
			if (word.compare("spinning") == 0)
			{
				*fs >> word;
				ConfigParser::spinning = atof(word.c_str());
			} else if (word.compare("spinSpeed") == 0)
			{
				*fs >> word;
				ConfigParser::spinSpeed = atof(word.c_str());
			} else if (word.compare("backgroundColor") == 0)
			{
				// structure color := r, g, b
				*fs >> word;
				ConfigParser::backgroundColor.r = atof(word.c_str());

				*fs >> word;
				ConfigParser::backgroundColor.g = atof(word.c_str());

				*fs >> word;
				ConfigParser::backgroundColor.b = atof(word.c_str());
			} else if (word.compare("terrainWidth") == 0)
			{
				*fs >> word;
				ConfigParser::terrainWidth = atof(word.c_str());
			} else if (word.compare("terrainDepth") == 0)
			{
				*fs >> word;
				ConfigParser::terrainDepth = atof(word.c_str());
			} else if (word.compare("terrainHeight") == 0)
			{
				*fs >> word;
				ConfigParser::terrainHeight = atof(word.c_str());
			} else if (word.compare("terrainPath") == 0)
			{
				// read the whole line
				std::getline(*fs, word);

				// get rid of unuseful spaces
				word.erase(0, word.find_first_not_of(' '));
				word.erase(word.find_last_not_of(' ')+1); 

				ConfigParser::terrainPath = word;
			} else
			{
				// the word isn't correct. throw a error message and go to the next one.
				std::cout << "error: \"" << word << "\"" << std::endl;
			}
		}

		std::cout << "finished reading" << std::endl;
	}
	else
	{
		std::cout << "error opening the file";
	}

	// close and delete the input file stream
	fs->close();
	delete fs;
}

	float ConfigParser::getSpinning()
	{
		return this->spinning;
	}

	float ConfigParser::getSpinSpeed()
	{
		return this->spinSpeed;
	}

	ConfigParser::Color ConfigParser::getBackgroundColor()
	{
		return this->backgroundColor;
	}

	std::string ConfigParser::getTerrainPath()
	{
		return this->terrainPath;
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