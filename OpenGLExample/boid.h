#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

using namespace glm;


class Boid
{
	public:
		
		Boid();
		~Boid();
		void setPos(vec3 newPos);
		vec3 getPos();
		void setVel(vec3 newVel);
		vec3 getVel();
		void setOri(mat4 newOri);
		mat4 getOri();
	private:
		vec3 pos;
		vec3 velocity;
		mat4 orientation;
		
};
