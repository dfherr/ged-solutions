#include "TextureGenerator.h"
#include <iostream>
#include <string>

TextureGenerator::TextureGenerator()
{
}

TextureGenerator::~TextureGenerator()
{
}

void TextureGenerator::calcAlphas(float height, float slope, float& alpha1, float& alpha2, float& alpha3)
{
	if(height < 0.15f){
		alpha1 = 0.5f * slope;
		alpha2 = 0.0f;
		alpha3 = 0.0f;
	}
	else if(height < 0.35f){
		alpha1 = (1.15f - height)*slope;
		alpha2 = (height - 0.15f);
		alpha3 = (height - 0.15f) * slope;
	}else if(height < 0.6f){
		alpha1 = (1.0f-height)*slope;
		alpha2 = (height);
		alpha3 = (height) * slope;
	}else{
		alpha1 = 0.1f;
		alpha2 = height-0.1f;
		alpha3 = height * slope;
	}

}

float TextureGenerator::blendColor(float alpha1, float alpha2, float alpha3, float c0, float c1, float c2, float c3)
{
	// see slides03 slide 19 for this
	return alpha3*c3 + (1-alpha3) * (alpha2*c2+(1-alpha2)*(alpha1*c1+(1-alpha1)*c0));
}

void TextureGenerator::generateTexture(int resolution, std::wstring path, std::vector<NormalCalculator::Normal> normal_map, float* height_map)
{

	wchar_t buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	std::wstring w_dir(buffer);
	std::string s_dir(w_dir.begin(), w_dir.end());
	//			Low		High
	// Flat		tex1	tex3
	// Steep	tex2	tex4
	GEDUtils::SimpleImage tex1((s_dir + "\\..\\..\\..\\..\\external\\textures\\gras15.jpg").c_str());
	GEDUtils::SimpleImage tex2((s_dir + "\\..\\..\\..\\..\\external\\textures\\ground02.jpg").c_str());
	GEDUtils::SimpleImage tex3((s_dir + "\\..\\..\\..\\..\\external\\textures\\rock4.jpg").c_str());
	GEDUtils::SimpleImage tex4((s_dir + "\\..\\..\\..\\..\\external\\textures\\rock7.jpg").c_str());

	GEDUtils::SimpleImage output(resolution,resolution);

	float alpha1,alpha2,alpha3;
	float r1,r2,r3,r4;
	float g1,g2,g3,g4;
	float b1,b2,b3,b4;
	for(int y = 0; y < resolution; ++y)
	{
		for(int x = 0; x < resolution; ++x)
		{			
			calcAlphas(height_map[IDX(x,y,resolution)], 1-normal_map[IDX(x,y,resolution)].z, alpha1, alpha2, alpha3);

			tex1.getPixel(x % tex1.getWidth(), y % tex1.getHeight(), r1, g1, b1);
			tex2.getPixel(x % tex2.getWidth(), y % tex2.getHeight(), r2, g2, b2);
			tex3.getPixel(x % tex3.getWidth(), y % tex3.getHeight(), r3, g3, b3);
			tex4.getPixel(x % tex4.getWidth(), y % tex4.getHeight(), r4, g4, b4);
			
			output.setPixel(x,y,blendColor(alpha1,alpha2,alpha3,r1,r2,r3,r4),blendColor(alpha1,alpha2,alpha3,g1,g2,g3,g4),blendColor(alpha1,alpha2,alpha3,b1,b2,b3,b4));

		}
	}
		output.save(&path[0]);
}
