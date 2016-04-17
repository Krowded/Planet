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

	mat4 AdjustModelToHeightMap(mat4 ModelToWorldMatrix, GLfloat height)
	{
		GLfloat heightDifference = (ModelToWorldMatrix.m)[7] - height;
		if (heightDifference < 0)
			(ModelToWorldMatrix.m)[7] = fmin((ModelToWorldMatrix.m)[7] - heightDifference, (ModelToWorldMatrix.m)[7] + MaxFallSpeed);
		else
			(ModelToWorldMatrix.m)[7] = fmax((ModelToWorldMatrix.m)[7] - heightDifference, (ModelToWorldMatrix.m)[7] - MaxFallSpeed);
		return ModelToWorldMatrix;
	}

	mat4 AdjustCameraToHeightMap(mat4 camBaseMatrix, GLfloat height)
	{
		if( IsGravityOn() )
		{
			height = -height - CameraDefaultHeight;
			camBaseMatrix = AdjustModelToHeightMap(camBaseMatrix, height);
		}

		return camBaseMatrix;
	}

	vec3 GetCurrentPosition(mat4 ModelToWorldMatrix)
	{
		vec3 currentPosition = SetVector((ModelToWorldMatrix.m)[3], (ModelToWorldMatrix.m)[7], (ModelToWorldMatrix.m)[11]); 
		return currentPosition;
	}
