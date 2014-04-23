/**
 * Author: Zach J. Wheeler
 * 4/23/2014
 * Graphics Final Project
 * declarations for Catmull-Clark algorithm
 */


#pragma once


#include "Mesh.h"


/// Takes a mesh object and subdivides it
void catmullclark(Mesh &mesh, int iterations);