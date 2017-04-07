#include "boid.h"

Boid::Boid()
{
	
}
Boid::~Boid()
{
	
}
void Boid::setPos(vec3 newPos)
{
	pos = newPos;
}
vec3 Boid::getPos()
{
	return pos;
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
