#include "HeightfieldGenerator.h"
#include <iostream>

HeightfieldGenerator::HeightfieldGenerator(int res, int smoothness) : generator(3), distribution(0.0f, 1.0f)
{
	// set the given parameter to the object attribute.
	this->res = res;
	this->resIncr = res+1; //incremented res

	// initialize the array of (resolution+1)*(resolution+1)
	// don't forget to delete it later!
	arr = new float[resIncr*resIncr];

	// generate the heightfield.
	generate();

	// smooth the calculated heightfield array.
	for(int i = 0; i < smoothness; ++i)
	{
		smoothArray();
	}
}

HeightfieldGenerator::~HeightfieldGenerator(void)
{
	// don't forget to delete the array!
	delete[] arr;
}

/*
* Initialize and then generate the heightfield with the diamond square algorithm.
*/
void HeightfieldGenerator::generate(void)
{
	// initialize the heightfieldArray
	initialize();

	// set the roughness for the diamond square algorithm
	roughness = 0.60f;

	// do the diamond square algorithm
	// s is the stepsize
	for(int s = res/2; s >= 1; s /= 2) 
	{
		// iterate diamond step
		for(int y = s; y < res; y += (s*2)) 
		{
			for(int x = s; x < res; x += (s*2)) 
			{
				diamondStep(x, y, s);
			}
		}

		// iterate square step
		for(int y = 0; y < res; y += (s*2)) 
		{
			for(int x = 0; x < res; x += (s*2))
			{
				squareStep(x, y, s);
			}
		}

		// decrease the roughness
		roughness *= 0.55f;
	}
}

/*
* Initializes the heightfieldArray with four random values for the corners between 0.0 and 1.0.
*/
void HeightfieldGenerator::initialize()
{
	float vertex = random(0.3f, 0.7f);
	// left top corner
	arr[IDX(0, 0, resIncr)] = vertex;

	// right top corner
	arr[IDX(res, 0, resIncr)] = vertex;

	// left bottom corner
	arr[IDX(0, res, resIncr)] = vertex;

	// right bottom corner
	arr[IDX(res, res, resIncr)] = vertex;
}

/*
* Returns a random number between min and max.
* Random numbers are normaly distributed with mean at min+max / 2.0 and derivation of max-mean / 3.0.
*/
float HeightfieldGenerator::random(float min, float max)
{
	// make sure roughness didn't get too small.
	if (max < 0.001f || (min < 0.0f && min > -0.001f)) 
	{
		max = 0.001f;
		min = -0.001f;
	}

	// generate random numbers as long as the result isn't in the range between min and max.
	while(true)
	{
		float number = (float)distribution(generator);
		if(number >= min && number <= max) 
		{
			return number;
		}
	}

	return 0;
}

void HeightfieldGenerator::diamondStep(int x, int y, int s)
{
	// take the four values around (x,y) and calculate the average.
	float avg = (arr[IDX(x-s, y-s, resIncr)] + arr[IDX(x+s, y-s, resIncr)] + arr[IDX(x-s, y+s, resIncr)] + arr[IDX(x+s, y+s, resIncr)]) / 4.0f;

	// add a random value to the average in dependence of the roughness.
	avg += (random(-1, 1) * roughness);

	// don't let the average get to small or to big.
	if(avg > 1.0)
	{
		avg = 1.0f;
	} 
	else if(avg < 0.0)
	{
		avg = 0.0f;
	}

	// set the value for that point.
	arr[IDX(x, y, resIncr)] = avg;
}

void HeightfieldGenerator::squareStep(int x, int y, int s)
{
	// left(a1)/top(b1) corner
	// starting point can't be out of bounds
	float corner_1 = arr[IDX(x, y, resIncr)];
	float corner_a2;
	float corner_a3;
	float corner_a4;
	float corner_b2;
	float corner_b3;
	float corner_b4;

	// bottom(a2)/right(b2) corner
	corner_a2 = arr[IDX((x+s), (y+s), resIncr)];
	corner_b2 = arr[IDX((x+s), (y+s), resIncr)]; 

	// right(a3)/bottom(b3) corner
	corner_a3 = arr[IDX((x+(2*s)), y, resIncr)];
	corner_b3 = arr[IDX(x, (y+(2*s)), resIncr)];

	// top(a4)/left(b4) corner
	if(y == 0)
	{
		corner_a4 = arr[IDX((x+s), (res-s), resIncr)];
	}
	else
	{
		corner_a4 = arr[IDX((x+s), (y-s), resIncr)];
	}

	if(x == 0)
	{
		corner_b4 = arr[IDX((res-s), (y+s), resIncr)];
	}
	else
	{
		corner_b4 = arr[IDX((x-s), (y+s), resIncr)];
	}


	// fill new averaged positions
	float avg_a = (corner_1 + corner_a2 + corner_a3 + corner_a4) / 4.0f;
	avg_a = avg_a + (random(-1, 1) * roughness);

	if(avg_a > 1.0)
	{
		avg_a = 1.0f;
	}
	else if(avg_a < 0.0)
	{
		avg_a = 0.0f;
	}
	arr[IDX((x+s), y, resIncr)] = avg_a;

	//seamless mapping
	if(y == 0){
		arr[IDX((x+s), res, resIncr)] = avg_a;
	}




	float avg_b = (corner_1 + corner_b2 + corner_b3 + corner_b4)  / 4.0f;
	avg_b = avg_b + (random(-1, 1) * roughness);

	if(avg_b > 1.0) 
	{
		avg_b = 1.0f;
	}
	else if(avg_b < 0.0)
	{ 
		avg_b = 0.0f;
	}

	arr[IDX(x, (y+s), resIncr)] = avg_b;
	//seamless mapping
	if(x == 0){
		arr[IDX(res, (y+s), resIncr)] = avg_b;
	}

}


void HeightfieldGenerator::smoothArray()
{
	float* temp = new float[resIncr*resIncr];
	float avg = 0.0f;

	for(int y = 0; y < resIncr; ++y){
		for(int x = 0; x < resIncr; ++x){
			// AVG 3x3 with center at E = (x,y)
			// A B C
			// D E F
			// G H I
			// on boarders -1 = res and res+1 = 0 (x + resIncr)% resIncr, need to add res+1 as c++ does not support negative modulo arithmetics
			// this can be done because heightfield is generated seamless
			// for speed optimazation we do that only if x = 0 or x = res or y = 0 or y = res

			if(x == 0 || x == res || y == 0 || y == res){
				//A (x-1, y-1)
				avg += arr[IDX(((x-1) + resIncr) % resIncr, ((y-1) + resIncr) % resIncr, resIncr)];
				//B (x, y-1)
				avg += arr[IDX(x, ((y-1) + resIncr) % resIncr, resIncr)];
				//C (x+1, y-1)
				avg += arr[IDX((x+1) % resIncr, ((y-1) + resIncr) % resIncr, resIncr)];
				//D (x-1, y)
				avg += arr[IDX(((x-1) + resIncr) % resIncr, y, resIncr)];
				//E (x,y)
				avg += arr[IDX(x, y, resIncr)];
				//F (x+1, y)
				avg += arr[IDX((x+1) % resIncr, y, resIncr)];
				//G (x-1, y+1)
				avg += arr[IDX(((x-1) + resIncr) % resIncr, (y+1) % resIncr, resIncr)];
				//H (x, y+1)
				avg += arr[IDX((x), (y+1) % resIncr, resIncr)];
				//I (x+1, y+1)
				avg += arr[IDX((x+1) % resIncr, (y+1) % resIncr, resIncr)];
			}else{
				//A (x-1, y-1)
				avg += arr[IDX((x-1), (y-1), resIncr)];
				//B (x, y-1)
				avg += arr[IDX(x, (y-1), resIncr)];
				//C (x+1, y-1)
				avg += arr[IDX((x+1), (y-1), resIncr)];
				//D (x-1, y)
				avg += arr[IDX((x-1), y, resIncr)];
				//E (x,y)
				avg += arr[IDX(x, y, resIncr)];
				//F (x+1, y)
				avg += arr[IDX((x+1), y, resIncr)];
				//G (x-1, y+1)
				avg += arr[IDX((x-1), (y+1), resIncr)];
				//H (x, y+1)
				avg += arr[IDX((x), (y+1), resIncr)];
				//I (x+1, y+1)
				avg += arr[IDX((x+1), (y+1), resIncr)];
			}
			temp[IDX(x,y,resIncr)] = avg / 9.0f;
			avg = 0.0f;
		}
	}

	//copy array
	memcpy(arr, temp,(resIncr*resIncr)*sizeof(float));
	delete[] temp;

}
// get the heightarray as vector with cut down to original resolution * resolution
std::vector<float> HeightfieldGenerator::get_vector()
{
	std::vector<float> vec(res * res);

	for(int x = 0; x < res; ++x) 
	{
		for(int y = 0; y < res; ++y) 
		{
			// set grey values at each pixel
			vec[IDX(x, y, res)] = arr[IDX(x, y, resIncr)];
		}
	}

	return vec;
}

void HeightfieldGenerator::saveDownsized(std::wstring path){

	// new resoultion
	int resDown = res/4;
	GEDUtils::SimpleImage heightImageDownsized(resDown,resDown);

	//average of 4x4
	float avg=0.0f;

	// downsize array, go through downsized version (y, x)  
	for(int y = 0; y < resDown; ++y){
		for(int x = 0; x < resDown; ++x){
			// map 4 values to the right and to the bottom to (x,y)
			for(int b = y*4; b < (y*4) +4; ++b){
				for(int a = x*4; a < (x*4) +4; ++a){
					avg += arr[IDX(a,b, resIncr)]; 
				}
			}
			heightImageDownsized.setPixel(x, y, (avg / 16.0f));
			avg = 0.0f;
		}
	}

	heightImageDownsized.save(&path[0]);

}
