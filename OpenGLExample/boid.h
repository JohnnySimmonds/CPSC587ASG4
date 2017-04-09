#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#define PI 3.14159265359
using namespace glm;
using namespace std;
struct pos
{
	vec3 p1;
	vec3 p2;
	vec3 p3;
};
/*TODO use boid based on a single point (center and radius around it for collisions)*/
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
		void resolveForces();
		vec3 getCenter();
		void boundBoid();
		void printVec3(vec3 toPrint);
		vec3 velLim(vec3 vel);
		vec3 setBound(vec3 posBoid, float bound);
		float getRad();
		
	private:
		void center();
		pos posBoid;
		vec3 boidCenter;
		float radius;
		vec3 velocity;
		vec3 newVelocity;
		mat4 orientation;
		
};
