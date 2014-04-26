/**
 * An example program that creates a 24-bit bitmap file that is 800 x 600 and makes a blue/red checkerboard pattern.
 * Uses EasyBMP
 *
 * Cory Boatright
 * University of Pennsylvania, Fall 2011
 **/

#include "EasyBMP.h"
#include "glm\glm.hpp"

#include <iostream>
#include <utility>

using namespace glm;
using namespace std;

ostream& operator<< (ostream &out, const vec3 &v) {
	out << "(" << v.r << ", " << v.g << ", " << v.b << ")";
	return out;
}

int main(int argc, char** argv) {
	int width = 800;
	int height = 600;

	BMP output;
	output.SetSize(width, height);
	output.SetBitDepth(24);

	vec3 E(0.f), M(0.f,0.f,1.f), H(1.f*float(width)/float(height),0.f,0.f), V(0.f,1.f,0.f);

	for(int x=0; x < width; ++x) {
		for(int y=0; y < height; ++y) {
			vec3 P = M + (2*x/float(width-1)-1)*H + (2*y/float(height-1)-1)*V;
			vec3 D = normalize(P-E);
			int r = clamp(int(abs(D.r)*256), 0, 255);
			int g = clamp(int(abs(D.g)*256), 0, 255);
			int b = clamp(int(abs(D.b)*256), 0, 255);
			output(x,y)->Red = r;
			output(x,y)->Green = g;
			output(x,y)->Blue = b;
		}
	}

	output.WriteToFile("output.bmp");
	return 0;
}