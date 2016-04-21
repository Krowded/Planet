#include "CameraControl.h"

mat4 mouseRotationMatrix = {
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1	};

vec3 GetCurrentCameraPosition(mat4 camPositionMatrix)
{
	mat4 inverseMat = InvertMat4(camPositionMatrix); 
	return SetVector((inverseMat.m)[3], (inverseMat.m)[7], (inverseMat.m)[11]);
}

vec3 GetBackDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix); //Only needed if there is scaling?
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[2], (directions.m)[6], (directions.m)[10]));
}

vec3 GetRightDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix);
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[0], (directions.m)[4], (directions.m)[8]));	
}

vec3 GetUpDirectionVec(mat4 camRotatedMatrix)
{
	//mat4 directions = InvertMat4(camRotatedMatrix);
	mat4 directions = Transpose(camRotatedMatrix);
	return Normalize(SetVector( (directions.m)[1], (directions.m)[5], (directions.m)[9]));
}

vec3 GetOldUpDirectionVec(mat4 camRotatedMatrix)
{
	static vec3 upVec = {0, 1, 0};
	if( IsGravityOn() )
	{
		upVec = GetUpDirectionVec(camRotatedMatrix);
	}
	return upVec;
}

vec3 GetNewUpDirectionVec(mat4 camPositionMatrix)
{
	static vec3 upvec = {0, 1, 0};
	if(IsGravityOn())
	{
		upvec = Normalize(VectorSub(GetCurrentCameraPosition(camPositionMatrix), middleOfPlanet));	
	}
	return upvec;
}

static vec3 axis = {0, 1, 0};
mat4 ChangeUpDirection(mat4 camPositionMatrix, vec3 newUpVector)
{
	vec3 oldUpVector = GetOldUpDirectionVec(camPositionMatrix);
	
	GLfloat angle = acos( DotProduct(oldUpVector, newUpVector));
	if( angle > 0.0001 && angle < M_PI - 0.0001)
	{
		axis = Normalize( CrossProduct(oldUpVector, newUpVector) );
	}
	camPositionMatrix = Mult( ArbRotate(axis, -angle), camPositionMatrix);
	fprintf(stderr, "Axis x %f y %f z %f\n", axis.x, axis.y, axis.z);
	//fprintf(stderr, "%f\n", DotProduct(axis));
	//fprintf(stderr, "Angle %f\n", angle);
	//fprintf(stderr, "Old x %f y %f z %f\n", oldUpVector.x, oldUpVector.y, oldUpVector.z);
	//fprintf(stderr, "new x %f y %f z %f\n", newUpVector.x, newUpVector.y, newUpVector.z);

	return camPositionMatrix;
}


void CameraMouseUpdate(GLint mouseX, GLint mouseY)
{	
		static GLfloat x = 0;
		static GLfloat y = 0;	

		//Update mousepointer value
		if (mouseY != 0)
		{
			x = (GLfloat)mouseX;
			y = (GLfloat)mouseY;
		}
		
		mouseRotationMatrix = Mult( Rx(2*M_PI*y/(windowHeight*0.5)), Ry(2*M_PI*x/(windowWidth*0.5)));		
}

mat4 UpdateCamera(mat4 camPositionMatrix)
{
	vec3 newUp = GetNewUpDirectionVec(camPositionMatrix);
	mat4 camRotatedMatrix = ChangeUpDirection(camPositionMatrix, newUp);
	camRotatedMatrix = Mult(mouseRotationMatrix, camRotatedMatrix);
	return camRotatedMatrix;
}


//Read keyboard input and update camera position
mat4 CameraControl(GLint t, mat4 camRotatedMatrix, mat4 camPositionMatrix)
{
	static GLfloat averageSpeed;
	if (averageSpeed <= 0)
	{
		averageSpeed = standardSpeed;
	}
	static GLint tlast = 0;
	GLint passedTime = t - tlast;
	tlast = t;	
	
	GLfloat speed = (GLfloat)passedTime * averageSpeed;

	vec3 upVec = GetUpDirectionVec(camRotatedMatrix);
	vec3 backVec = GetBackDirectionVec(camRotatedMatrix);
	vec3 rightVec = GetRightDirectionVec(camRotatedMatrix);

	vec3 pos = GetCurrentCameraPosition(camPositionMatrix);
	fprintf(stderr, "Position: x %f y %f z %f\n", pos.x, pos.y, pos.z);

	//Can only move perpendicular to planet -> remove projection onto normal
	if ( IsGravityOn() )
	{
		vec3 gravUpVec = GetNewUpDirectionVec(camPositionMatrix);

		vec3 backVecProj = ScalarMult(gravUpVec, DotProduct(backVec, gravUpVec));
		backVec = Normalize(VectorSub( backVec, backVecProj));
		
		vec3 rightVecProj = ScalarMult(gravUpVec, DotProduct(rightVec, gravUpVec));
		rightVec = Normalize(VectorSub( rightVec, rightVecProj));
	}
	backVec = ScalarMult(backVec, speed);		
	rightVec = ScalarMult(rightVec, speed);
	upVec = ScalarMult(upVec, speed);
	
	if (glutKeyIsDown('w') || glutKeyIsDown('W'))
		camPositionMatrix = Mult( T( backVec.x, backVec.y, backVec.z), camPositionMatrix);
	if (glutKeyIsDown('a') || glutKeyIsDown('A')) 
		camPositionMatrix = Mult( T( rightVec.x, rightVec.y, rightVec.z), camPositionMatrix);
	if (glutKeyIsDown('s') || glutKeyIsDown('S'))
		camPositionMatrix = Mult( T( -backVec.x, -backVec.y, -backVec.z), camPositionMatrix);
	if (glutKeyIsDown('d') || glutKeyIsDown('D'))
		camPositionMatrix = Mult( T( -rightVec.x, -rightVec.y, -rightVec.z), camPositionMatrix);
	if (glutKeyIsDown('q') || glutKeyIsDown('Q'))
		camPositionMatrix = Mult( T( upVec.x, upVec.y, upVec.z), camPositionMatrix);
	if (glutKeyIsDown('e') || glutKeyIsDown('E'))
		camPositionMatrix = Mult( T( -upVec.x, -upVec.y, -upVec.z), camPositionMatrix);


	{ //static scope limiter
		static bool spacePressed = false;
		if (glutKeyIsDown(' '))
		{
			if (!spacePressed)
			{
				SetGravity(!IsGravityOn());
				spacePressed = true;
			}
		}
		else
		{
			spacePressed = false;
		}
	}

	{	//static scope limiter
		static bool zPressed = false;
		if (glutKeyIsDown('z') || glutKeyIsDown('Z'))
		{
			if(!zPressed)
			{
				if (averageSpeed == standardSpeed)
				{
					averageSpeed = runSpeed;
				}
				else
				{
					averageSpeed = standardSpeed;
				}
				zPressed = true;
			}
		}
		else
		{
			zPressed = false;
		}
	}

	return camPositionMatrix;
}


mat4 AdjustCameraToHeightMap(mat4 camPositionMatrix, GLfloat height)
{
	if( IsGravityOn() )
	{
		height = cameraHeight + height;
		camPositionMatrix = AdjustModelToHeightMap(camPositionMatrix, GetCurrentCameraPosition(camPositionMatrix), height);
	}

	return camPositionMatrix;
}


