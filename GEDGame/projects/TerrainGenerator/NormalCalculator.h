#pragma once

#include "stdafx.h"
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include "SimpleImage.h"

class NormalCalculator
{

	
public:
		
	struct Normal {
		float x;
		float y;
		float z;
	};

	NormalCalculator(int resolution, float *heightfield);
	~NormalCalculator(void);
	std::vector<Normal> get_vector(void);
	void saveImage(std::wstring path);

private:
	void calculate();


	std::vector<Normal> normalVec;
	int resolution; //normal map resolution
	float* heightfield;


};

