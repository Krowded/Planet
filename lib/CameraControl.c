	#include "CameraControl.h"

	vec3 GetCurrentCameraPosition(mat4 camPositionMatrix)
	{
		mat4 inverseMat = InvertMat4(camPositionMatrix);
		vec3 pos = SetVector((inverseMat.m)[3], (inverseMat.m)[7], (inverseMat.m)[11]);
		return pos;

	}

	vec3 GetBackDirectionVec(mat4 camRotatedMatrix)
	{
		mat4 directions = InvertMat4(camRotatedMatrix); //Taking the inverse, kinda pointless
		return Normalize(SetVector( (directions.m)[2], (directions.m)[6], (directions.m)[10]));
	}

	vec3 GetRightDirectionVec(mat4 camRotatedMatrix)
	{
		mat4 directions = InvertMat4(camRotatedMatrix); //Taking the inverse, kinda pointless
		return Normalize(SetVector( (directions.m)[0], (directions.m)[4], (directions.m)[8]));	
	}

	vec3 GetUpDirectionVec(mat4 camRotatedMatrix)
	{
		mat4 directions = InvertMat4(camRotatedMatrix); //Taking the inverse, kinda pointless
		vec3 upVec = Normalize(SetVector( (directions.m)[1], (directions.m)[5], (directions.m)[9]));
		return upVec;
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



	mat4 ChangeUpDirection(mat4 camPositionMatrix, vec3 newUpVector)
	{
		vec3 oldUpVector = GetOldUpDirectionVec(camPositionMatrix);

		GLfloat angle = acos( DotProduct(oldUpVector, newUpVector));
		if( angle > 0.0001)
		{
			vec3 axis = Normalize( CrossProduct(oldUpVector, newUpVector) );
			camPositionMatrix = Mult( ArbRotate(axis, -angle), camPositionMatrix);
		}

		return camPositionMatrix;
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

	mat4 CameraMouseUpdate(GLint mouseX, GLint mouseY, mat4 camRotatedMatrix, mat4 camPositionMatrix)
	{	
			static GLfloat x = 0;
			static GLfloat y = 0;	

			//Update mousepointer value
			if (mouseY != 0)
			{
				x = (GLfloat)mouseX;

				//Clamp Y
				if (mouseY < 512 && mouseY > 0)
				y = (GLfloat)mouseY;
			}
			
			vec3 newUp = GetNewUpDirectionVec(camPositionMatrix);
			camRotatedMatrix = ChangeUpDirection(camPositionMatrix, newUp);
			camRotatedMatrix = Mult( Ry(2*M_PI*x/512), camRotatedMatrix);
			camRotatedMatrix = Mult( Rx(2*M_PI*y/324), camRotatedMatrix);		

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

		//Can only move perpendicular to planet -> remove projection onto normal
		vec3 gravUpVec = GetNewUpDirectionVec(camPositionMatrix);

		vec3 upVec = GetUpDirectionVec(camRotatedMatrix);
		vec3 backVec = GetBackDirectionVec(camRotatedMatrix);//Normalize(SetVector(backDirectionVec.x, backDirectionVec.y, backDirectionVec.z));
		vec3 rightVec = GetRightDirectionVec(camRotatedMatrix);//Normalize(SetVector(rightDirectionVec.x, rightDirectionVec.y, rightDirectionVec.z));
		if ( IsGravityOn() )
		{
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


