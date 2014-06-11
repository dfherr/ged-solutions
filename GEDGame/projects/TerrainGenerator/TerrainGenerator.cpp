// TerrainGenerator.cpp : Defines the entry point for the console application.
#include <string>
#include <exception>
#include <SimpleImage.h>
#include <TextureGenerator.h>
#include <sstream>
#include <iostream>

#include "Windows.h"
#include "stdafx.h"

#include "TextureGenerator.h"
#include "NormalCalculator.h"
#include "HeightfieldGenerator.h"

// access a 2D array of width w at position x / y
#define IDX(x, y, w) ((x) + (y) * (w))

// standard usings so we don't have to type everytime std
using std::cout;
using std::endl;

// global variables
std::wstring output_height_filename;
std::wstring output_color_filename;
std::wstring output_normal_filename;

int resolution;

bool checkArguments(const int argc, _TCHAR* argv[]);
void fillImage(float* arr, GEDUtils::SimpleImage& img);
void heightfieldDownsizing(int resolution, std::wstring path, float *heightfield);

/**
* Main function of our programm.
* Should be called with the following commands:
*
* -r 1024 -o_height heightfield.tiff -o_color color.tiff -o_normal normal.tiff
*/
int _tmain(int argc, _TCHAR* argv[])
{
	// check input and parse it to the variables. if it returns false, exit the program with code 1.
	if(!checkArguments(argc, argv))
	{
		return 1;
	}
	HeightfieldGenerator *heightGenerator = new HeightfieldGenerator(resolution, 20);


	// create vector to store heightfield.
	std::vector<float> heightVec = heightGenerator->get_vector();
	


	NormalCalculator *nc = new NormalCalculator(resolution, &heightVec[0]);
	nc->saveImage(output_normal_filename);
	std::vector<NormalCalculator::Normal>normalVec = nc->get_vector();

	TextureGenerator *tx = new TextureGenerator();
	tx->generateTexture(resolution,output_color_filename,normalVec,&heightVec[0]);

	heightGenerator->saveDownsized(output_height_filename);

	delete nc;
	delete tx;
	delete heightGenerator;
	
	return 0;
}

/**
* Checks if the arguments match the convention and parses it to the global variables.
* If some arguments were wrong the method returns false.
*/
bool checkArguments(const int argc, _TCHAR* argv[])
{
	try
	{
		// we need 8 + 1 inputs
		if (argc != 9)
		{
			throw std::exception("invalid argument count");
		}
		else 
		{
			if (_tcscmp(argv[1], TEXT("-r")) != 0) 
			{
				throw std::exception("-r missing");		
			}
			else
			{
				resolution = _tstoi(argv[2]);
				if (resolution <= 0)
				{
					throw std::exception("resolution <= 0");
				}
			}

			if (_tcscmp(argv[3], TEXT("-o_height")) != 0)
			{
				throw std::exception("-o_height missing");		
			}
			else
			{
				output_height_filename = (argv[4]);
			}

			if (_tcscmp(argv[5], TEXT("-o_color")) != 0)
			{
				throw std::exception("-o_color missing");		
			}
			else
			{
				output_color_filename = argv[6];
			}

			if (_tcscmp(argv[7], TEXT("-o_normal")) != 0)
			{
				throw std::exception("-o_normal missing");		
			}
			else
			{
				output_normal_filename = argv[8];
			}
		}
	}
	catch(std::exception e)		
	{
		// if a exception was thrown, print the message to the screen and then return false
		cout << "an exception occurred: " << e.what() << endl;
		return false;
	}

	// returns true if all passed arguments were correct
	return true;
}
