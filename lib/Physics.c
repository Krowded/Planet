#include "Physics.h"

	bool isGravityOnBool = false;
	GLfloat MaxFallSpeed = 0;
	GLfloat CameraDefaultHeight = 0;

	void PhysicsInit(GLfloat maxFallSpeed, GLfloat cameraHeight)
	{
		SetFallSpeed(maxFallSpeed);
		SetCameraHeight(cameraHeight);
	}


	void SetGravity(bool isGravityOn)
	{
		isGravityOnBool = isGravityOn;
	}

	void SetFallSpeed(GLfloat maxFallSpeed)
	{
		MaxFallSpeed = maxFallSpeed;
	}

	void SetCameraHeight(GLfloat cameraHeight)
	{
		CameraDefaultHeight = cameraHeight;
	}

	bool IsGravityOn()
	{
		return isGravityOnBool;
	}

	mat4 AdjustModelToHeightMap(mat4 ModelToWorldMatrix, GLfloat groundHeight)
	{
		vec3 currentHeightVector = VectorSub(GetCurrentCameraPosition(camMatrix), middleOfPlanet);
		GLfloat currentHeight = Norm(currentHeightVector);
		GLfloat heightDifference = -(currentHeight - groundHeight);

		fprintf(stderr, "currentHeight: %f\n", currentHeight);
		fprintf(stderr, "HeightDifference: %f\n", heightDifference);
		fprintf(stderr, "groundHeight: %f\n", groundHeight);
		vec3 upVec = GetUpDirectionVec(ModelToWorldMatrix);
		
		if(abs(heightDifference) > 10 )
		{
			if ( abs(heightDifference) > MaxFallSpeed)
				heightDifference = MaxFallSpeed;
			
			if(currentHeight < groundHeight)
				heightDifference = -heightDifference;


			mat4 translation = T(upVec.x*heightDifference, upVec.y*heightDifference, upVec.z*heightDifference);
			ModelToWorldMatrix = Mult(translation, ModelToWorldMatrix);
		}
		return ModelToWorldMatrix;
	}

	mat4 AdjustCameraToHeightMap(mat4 camBaseMatrix, GLfloat height)
	{
		if( IsGravityOn() )
		{
			height = CameraDefaultHeight + height;
			camBaseMatrix = AdjustModelToHeightMap(camBaseMatrix, height);
		}

		return camBaseMatrix;
	}

	vec3 GetCurrentPosition(mat4 ModelToWorldMatrix)
	{
		vec3 currentPosition = SetVector((ModelToWorldMatrix.m)[3], (ModelToWorldMatrix.m)[7], (ModelToWorldMatrix.m)[11]); 
		return currentPosition;
	}
