#include "Physics.h"
#include "VectorUtils3.h"
#include "math.h"
#include "Init.h"
#include "PhysicsGlobals.h"


bool isGravityOnBool = false;

void SetGravity(bool isGravityOn)
{
	isGravityOnBool = isGravityOn;
}

bool IsGravityOn()
{
	return isGravityOnBool;
}

/*
 *	Checks which side of a planet an object is
 */
enum planetSide GetSide(struct PlanetStruct planet, vec3 vectorFromCenter)
{

	GLint forward;
	GLint sideways;

	GLfloat angleFromPole = acos(DotProduct(Normalize(vectorFromCenter), planet.upVec));
	GLfloat angleFromFront = acos(DotProduct(Normalize(vectorFromCenter), planet.frontVec));

	if(fabs(angleFromPole) > 3*M_PI/4)
	{
		forward = 3;
	}
	else if(angleFromPole > M_PI/4)
	{
		forward = 2;
	}
	else if(angleFromPole < -M_PI/4)
	{
		forward = 1;
	}
	else
	{
		forward = 0;
	}

	if(fabs(angleFromFront) > 3*M_PI/4)
	{
		sideways = 30;
	}
	else if( angleFromFront > M_PI/4 )
	{
		sideways = 20;
	}
	else if( angleFromFront < -M_PI/4)
	{
		sideways = 10;
	}
	else
	{
		sideways = 0;
	}


	fprintf(stderr, "%d\n", sideways+forward);

	switch (sideways+forward)
	{
		case  0:
		case 10:
		case 20:
		case 30:
			return PLANET_UP;

		case 12:
		case 21:
			return PLANET_RIGHT;

		case 22:
		case 11:
			return PLANET_LEFT;

		case  2:
		case 31:
			return PLANET_FRONT;

		case  1:
		case 32:
			return PLANET_BACK;

		case  3:
		case 13:
		case 23:
		case 33:
			return PLANET_DOWN;
		default:
			fprintf(stderr, "Angle on planet unaccounted for");
			exit(-1);
	}
}


/*
 *	Gets the height of the ground at a certain position
 */
GLfloat GetGroundHeight(vec3 currentPosition, struct PlanetStruct planet)
{
	vec3 vectorFromCenter = VectorSub(currentPosition, planet.center);
	GLfloat height = Norm(vectorFromCenter);
	if (height > planet.radius + maxHeight) //If above possible terrain, skip calculation
	{
		return planet.radius;
	}
	else
	{
		GetSide(planet, vectorFromCenter);

		return planet.radius;
	}

}

/*
 *	Cross references a models position with the height map and adjusts it
 */
mat4 AdjustModelToHeightMap(mat4 ModelToWorldMatrix, vec3 currentPosition, struct PlanetStruct planet)
{
	vec3 heightVector = VectorSub(currentPosition, planet.center);
	vec3 upVec = Normalize(heightVector);
	GLfloat currentHeight = Norm(heightVector);
	GLfloat groundHeight = planet.radius;//GetGroundHeight(currentPosition, planet);

	GLfloat heightDifference = currentHeight - groundHeight;
	
	
	if(fabs(heightDifference) > 0 )
	{
		if ( fabs(heightDifference) > maxFallSpeed)
		{
			heightDifference = maxFallSpeed;

			if(currentHeight < groundHeight)
				heightDifference = -heightDifference;
		}
		


		mat4 translation = T(upVec.x*heightDifference, upVec.y*heightDifference, upVec.z*heightDifference);
		ModelToWorldMatrix = Mult(translation, ModelToWorldMatrix);
	}
	return ModelToWorldMatrix;
}


/*
 *	Get position of an object that isn't the camera
 */
vec3 GetCurrentPosition(mat4 ModelToWorldMatrix)
{
	vec3 currentPosition = SetVector((ModelToWorldMatrix.m)[3], (ModelToWorldMatrix.m)[7], (ModelToWorldMatrix.m)[11]); 
	return currentPosition;
}
