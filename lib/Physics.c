#include "Physics.h"
#include "VectorUtils3.h"

	bool isGravityOnBool = false;

	void SetGravity(bool isGravityOn)
	{
		isGravityOnBool = isGravityOn;
	}

	bool IsGravityOn()
	{
		return isGravityOnBool;
	}

	mat4 AdjustModelToHeightMap(mat4 camBaseMatrix, vec3 currentPosition, GLfloat groundHeight)
	{
		vec3 currentHeightVector = VectorSub(currentPosition, middleOfPlanet);
		GLfloat currentHeight = Norm(currentHeightVector);
		GLfloat heightDifference = currentHeight - groundHeight;
		vec3 upVec = Normalize(currentHeightVector);//GetUpDirectionVec(ModelToWorldMatrix);

		fprintf(stderr, "currentHeight: %f\n", currentHeight);
		fprintf(stderr, "HeightDifference: %f\n", heightDifference);
		fprintf(stderr, "UpVec: X %f, Y %f, Z %f\n", upVec.x, upVec.y, upVec.z);
		
		if(abs(heightDifference) > 0.0 )
		{
			if ( abs(heightDifference) > maxFallSpeed)
			{
				heightDifference = maxFallSpeed;

				if(currentHeight < groundHeight)
					heightDifference = -heightDifference;
			}
			


			mat4 translation = T(upVec.x*heightDifference, upVec.y*heightDifference, upVec.z*heightDifference);
			camBaseMatrix = Mult(translation, camBaseMatrix);
		}
		return camBaseMatrix;
	}



	vec3 GetCurrentPosition(mat4 ModelToWorldMatrix)
	{
		vec3 currentPosition = SetVector((ModelToWorldMatrix.m)[3], (ModelToWorldMatrix.m)[7], (ModelToWorldMatrix.m)[11]); 
		return currentPosition;
	}
