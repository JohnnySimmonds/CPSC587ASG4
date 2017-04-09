#include "boid.h"
#include "math.h"
#include <random>
Boid::Boid()
{
	
	// Random generator for initial velocity between -1,1
	random_device rd; 
    mt19937 gen(rd());  
    uniform_real_distribution<> dist(-1,1);   
	float x,y,z;
    for (int i = 0; i < 3; ++i) {  
        if(i == 0)
			x = dist(gen);
		if(i == 1)
			y = dist(gen);
		if(i == 2)
			z = dist(gen);
    } 
	velocity = vec3(x,y,z);
	radius = 50.0f;
	up = vec3(0.0f, 1.0f, 0.0f);
	
}
Boid::~Boid()
{
}
void Boid::setNeighbours(vector<Boid> currNeighbours)
{
	neighbours = currNeighbours;
}
vec3 Boid::placeToGo()
{
	vec3 place = vec3(10.0f,10.0f,10.0f);
	return (getCenter()-place) / 1000.0f;
}
void Boid::setBound(float boundingBox)
{
	bound = boundingBox;
}
float Boid::getRad()
{
	return radius;
}
vec3 Boid::setBound(vec3 currPos, float bound)
{
	
	if(currPos.x < -bound)
	{
		velocity.x = -velocity.x;	
		//currPos.x = -bound;
	}
	else if(currPos.x > bound)
	{
		velocity.x = -velocity.x;
		//currPos.x = bound;
	}
	if(currPos.y < -bound)
	{
		velocity.y = -velocity.y;	
		//currPos.y = -bound;
	}
	else if(currPos.y > bound)
	{
		velocity.y = -velocity.y;
		//currPos.y = bound;
	}
	if(currPos.z < -bound)
	{
		velocity.z = -velocity.z;	
		//currPos.z = -bound;
	}
	else if(currPos.z > bound)
	{
		velocity.z = -velocity.z;
		//currPos.z = bound;
	}
	
	return currPos;
}
/* Bound the boids with in a 100X100X100 cube*/
void Boid::boundBoid()
{

	vec3 center = getCenter();
	
	vec3 p1 = posBoid.p1;
	vec3 p2 = posBoid.p2;
	vec3 p3 = posBoid.p3;
	//float bound = 100.0f;
	
	//center = setBound(center, bound);
	p1 = setBound(p1, bound);
	p2 = setBound(p2, bound);
	p3 = setBound(p3, bound);

	pos newPos;
	newPos.p1 = p1;
	newPos.p2 = p2;
	newPos.p3 = p3;
	
	cout << "P1 : "<< endl;
	printVec3(p1);
	cout << "P2 : " << endl;
	printVec3(p2);
	cout << "P3 : "<< endl;
	printVec3(p3);
	
	setPos(newPos);

}
void Boid::printVec3(vec3 toPrint)
{
	cout << "X: " << toPrint.x << endl;
	cout << "Y: " << toPrint.y << endl;
	cout << "Z: " << toPrint.z << endl;
}
vec3 Boid::velLim(vec3 vel)
{
	
	float velLimitUpper = 0.5f;
	if(length(vel) > velLimitUpper && length(vel) > 0)
		vel = (vel / length(vel)) * velLimitUpper;
	
	/*
	float velLimitLower = -0.1f;
	if(length(vel) < velLimitLower && length(vel) < 0)
		vel = (vel / length(vel)) * velLimitLower;
	*/
	
	
	return vel;
}
void Boid::resolveForces(float dt)
{

	velocity += newVelocity * dt;
	velocity = velLim(velocity);
	
	vec3 oldCenter = getCenter();
	
	posBoid.p1 += velocity;
	posBoid.p2 += velocity;
	posBoid.p3 += velocity;
	boundBoid();
	
	dir = (getCenter() - oldCenter);
	dir = dir / length(dir);

	
	newVelocity = vec3(0.0f,0.0f,0.0f);
	/*
	ori = orientation(dir, up);
	posBoid.p1 = (ori * vec4(posBoid.p1, 1.0f));
	posBoid.p2 = (ori * vec4(posBoid.p2, 1.0f));
	posBoid.p3 = (ori * vec4(posBoid.p3, 1.0f));
	*/
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
	newVelocity = newVel;
}
vec3 Boid::getVel()
{
	return velocity;
}
void Boid::setOri(vec3 dir, vec3 up, vec3 other)
{
	dir = dir/length(dir);
	up = up/length(up);
	other = other/length(other);

	ori[0][0] = other.x;
	ori[0][1] = other.y;
	ori[0][2] = other.z;
	
	ori[1][0] = up.x;
	ori[1][1] = up.y;
	ori[1][2] = up.z;
	
	ori[2][0] = dir.x;
	ori[2][1] = dir.y;
	ori[2][2] = dir.z;

}
mat4 Boid::getOri()
{
	return ori;
}
