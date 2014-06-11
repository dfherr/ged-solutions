#pragma once

#include <random>
#include <time.h>
#include <SimpleImage.h>

// access a 2D array of width w at position x / y
#define IDX(x, y, w) ((x) + (y) * (w))

class HeightfieldGenerator
{
public:	
	/**
	*	@res Resolution of the heightfield
	*   @smoothness the number of smooth cycles
	*/
	HeightfieldGenerator(int res, int smoothness);
	~HeightfieldGenerator(void);

	std::vector<float> get_vector(void);
	// save downsized image
	void saveDownsized(std::wstring path);

private:	
	void generate(void);
	void initialize(void);
	float random(float min, float max);
	void diamondStep(int x, int y, int s);
    void squareStep(int x, int y, int s);
	void smoothArray();

	float *arr;
	float roughness;

	int res;
	int resIncr; // incremented resolution
	std::normal_distribution<float> distribution;
	std::default_random_engine generator;

};