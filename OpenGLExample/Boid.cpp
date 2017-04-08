#include "boid.h"

Boid::Boid()
{
	velocity = vec3(0.1f, 0.1f, 0.0f);
	//velocity = vec3(random(-1.0, 1.0), random(-1.0, 1.0), random(-1.0, 1.0));
}
Boid::~Boid()
{
}


void Boid::center()
{
	vec3 p1 = posBoid.p1;
	vec3 p2 = posBoid.p2;
	vec3 p3 = posBoid.p3;
	float mid = 0.5f;
	boidCenter.x = mid*p1.x + mid*p2.x + mid*p3.x;
	boidCenter.y = mid*p1.y + mid*p2.y + mid*p3.y;
	boidCenter.z = mid*p1.z + mid*p2.z + mid*p3.z;
	
}
vec3 Boid::getCenter()
{
	center();
	return boidCenter;
}
void Boid::setPos(pos newPos)
{
	posBoid = newPos;
}

pos Boid::getPos()
{
	return posBoid;
}
void Boid::setVel(vec3 newVel)
{
	velocity = newVel;
}
vec3 Boid::getVel()
{
	return velocity;
}
void Boid::setOri(mat4 newOri)
{
	orientation = newOri;
}
mat4 Boid::getOri()
{
	return orientation;
}
