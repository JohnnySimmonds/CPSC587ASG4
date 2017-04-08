#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

using namespace glm;

struct pos
{
	vec3 p1;
	vec3 p2;
	vec3 p3;
};
class Boid
{
	public:

		Boid();
		~Boid();
		void setPos(pos newPos);
		pos getPos();
		void setVel(vec3 newVel);
		vec3 getVel();
		void setOri(mat4 newOri);
		mat4 getOri();
		void moveBoid();

		vec3 getCenter();

	private:
		void center();
		pos posBoid;
		vec3 boidCenter;
		//vec3 pos;
		vec3 velocity;
		mat4 orientation;
		
};
