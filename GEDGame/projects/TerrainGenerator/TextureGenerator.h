#include "stdafx.h"
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include "SimpleImage.h"
#include "NormalCalculator.h"


class TextureGenerator
{
	public:
		TextureGenerator(void);
		~TextureGenerator(void);

		
	private:
		// access a 2D array of width w at position x / y
		#define IDX(x, y, w) ((x) +(y) *(w))
		void calcAlphas(float height, float slope, float& alpha1, float& alpha2, float& alpha3);
		float blendColor(float alpha1, float alpha2, float alpha3, float c0, float c1, float c2, float c3);

	public:
		void generateTexture(int resolution, std::wstring savepath, std::vector<NormalCalculator::Normal>normal_map, float* height_map);
};
