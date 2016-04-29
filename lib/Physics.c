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
		GLfloat angleFromPole = acos(DotProduct(Normalize(vectorFromCenter), planet.upVec));
		if(abs(angleFromPole) > 3*maxAngleOfTerrain)
		{
			return 0;
		}
		else if(abs(angleFromPole) > maxAngleOfTerrain)
		{
			GLfloat angleFromFront = acos(DotProduct(Normalize(vectorFromCenter), planet.frontVec));
		}
		else
		{
			return 0;
		}
	}

}

mat4 AdjustModelToHeightMap(mat4 ModelToWorldMatrix, vec3 currentPosition, struct PlanetStruct planet)
{
	vec3 heightVector = VectorSub(currentPosition, planet.center);
	vec3 upVec = Normalize(heightVector);
	GLfloat currentHeight = Norm(heightVector);
	GLfloat groundHeight = planet.radius;//GetGroundHeight(currentPosition, planet);

	GLfloat heightDifference = currentHeight - groundHeight;
	
	
	if(abs(heightDifference) > 0.0 )
	{
		if ( abs(heightDifference) > maxFallSpeed)
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



vec3 GetCurrentPosition(mat4 ModelToWorldMatrix)
{
	vec3 currentPosition = SetVector((ModelToWorldMatrix.m)[3], (ModelToWorldMatrix.m)[7], (ModelToWorldMatrix.m)[11]); 
	return currentPosition;
}
