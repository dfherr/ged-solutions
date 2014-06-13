#include "NormalCalculator.h"
#define IDX(x, y, w) ((x) +(y) *(w))

NormalCalculator::NormalCalculator(int resolution, float *heightfield) : normalVec(resolution*resolution) 
{
	this->heightfield = heightfield;
	this->resolution = resolution;
	calculate();
}


NormalCalculator::~NormalCalculator(void)
{
}

void NormalCalculator::calculate(void)
{
	//new
	double x_dv;
	double y_dv;
	double z;
	double normal_length;
	for (int y=0; y<resolution; ++y)
	{
		for (int x=0; x<resolution; ++x)
		{
			//boarder wraping with ((x+1) + resolution) % resolution, + resolution can be skipped for +1, only necessary for -1
			// therefor resolution maps to = 0 and -1 maps to resolution-1
			// don't forget to scale x,y according to height/width of heightfield
			if(x == 0 || x == (resolution - 1)){
				x_dv = (heightfield[IDX((x+1)%resolution, y, resolution)] - heightfield[IDX((x-1 + resolution)%resolution, y, resolution)]) / 2.0 * resolution;
			}else{
				x_dv = (heightfield[IDX(x+1, y, resolution)] - heightfield[IDX(x-1, y, resolution)]) / 2.0 * resolution;
			}
			if(y == 0 || y == (resolution - 1)){
				y_dv = (heightfield[IDX(x, (y+1)%resolution, resolution)] - heightfield[IDX(x, (y-1+resolution) % resolution, resolution)]) / 2.0 * resolution;
			}
			else{
				y_dv = (heightfield[IDX(x, y+1, resolution)] - heightfield[IDX(x, y-1, resolution)]) / 2.0 * resolution;
			}
			normal_length = sqrt((x_dv*x_dv)+(y_dv*y_dv)+1);

			z = 1.0;
			// normalize normal (-x_dv, -y_dv, z)
			x_dv = -(x_dv /normal_length);
			y_dv = -(y_dv /normal_length);
			z = (z / normal_length);

			//  map to [0,1] and save
			int arrPos = IDX(x,y,resolution);
			normalVec[arrPos].x = (float)(x_dv/2.0 + 0.5);
			normalVec[arrPos].y = (float)(y_dv/2.0 + 0.5);
			normalVec[arrPos].z = (float)(z/2.0 + 0.5);	

		}
	}


}

std::vector<NormalCalculator::Normal> NormalCalculator::get_vector(void) {
	return normalVec;
}

// Save image to path
void NormalCalculator::saveImage(std::wstring path){
	GEDUtils::SimpleImage output(resolution,resolution);
	for (int y=0; y<resolution; ++y)
	{
		for (int x=0; x<resolution; ++x)
		{
			int arrPos = IDX(x,y,resolution);
			output.setPixel(x, y, normalVec[arrPos].x, normalVec[arrPos].y, normalVec[arrPos].z);
		}
	}
	output.save(&path[0]);
}