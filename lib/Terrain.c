#include "Terrain.h"

	struct offset TerrainOffset;

	const GLfloat terrainScale = 10.0;



	void SetTerrainOffset(GLfloat x, GLfloat y, GLfloat z)
	{
		TerrainOffset.x = x;
		TerrainOffset.y = y;
		TerrainOffset.z = z;
		
	}

	GLfloat* GenerateTerrainVertexArray(TextureData *tex)
	{
		GLint x, z;
		GLint vertexCount = tex->width * tex->height;
		GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
		for (x = 0; x < tex->width; x++)
			for (z = 0; z < tex->height; z++)
			{
				// Vertex array. y scales height
				vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
				vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / terrainScale;
				vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
			}

		return vertexArray;
	}

	GLfloat* GenerateTerrainTextureCoordinateArray(TextureData *tex)
	{
		GLint x, z;
		GLint vertexCount = tex->width * tex->height;
		GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
		for (x = 0; x < tex->width; x++)
			for (z = 0; z < tex->height; z++)
			{
				// Texture coordinates. You may want to scale them.
				texCoordArray[(x + z * tex->width)*2 + 0] = x;//(float)x / tex->width;
				texCoordArray[(x + z * tex->width)*2 + 1] = z;//(float)z / tex->height;
			}

		return texCoordArray;
	}

	GLuint* GenerateTerrainIndexArray(TextureData *tex)
	{
		GLint x, z;	
		GLint triangleCount = (tex->width-1) * (tex->height-1) * 2;
		GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);

		for (x = 0; x < tex->width-1; x++)
			for (z = 0; z < tex->height-1; z++)
			{
			// Triangle 1
				indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
				indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
				indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
			// Triangle 2
				indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
				indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
				indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
			}

		return indexArray;
	}

	GLfloat* GenerateTerrainNormalArray(TextureData *tex, GLfloat *vertexArray)
	{
		GLint x, z;
		GLint vertexCount = tex->width * tex->height;
		GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
		for (x = 0; x < tex->width-1; x++)
			for (z = 0; z < tex->height-1; z++)
			{
			// Normal vectors. You need to calculate these.
				if (x > 0 && x < tex->width-1 && z > 0 && z < tex->height-1 )
				{
					vec3 leftRightVector = SetVector( (vertexArray[(x - 1 + z * tex->width)*3 + 0] - vertexArray[(x + 1 + z * tex->width)*3 + 0]),
													  (vertexArray[(x - 1 + z * tex->width)*3 + 1] - vertexArray[(x + 1 + z * tex->width)*3 + 1]),
													  (vertexArray[(x - 1 + z * tex->width)*3 + 2] - vertexArray[(x + 1 + z * tex->width)*3 + 2]));

					vec3 upDownVector = SetVector( (vertexArray[(x + (z - 1) * tex->width)*3 + 0] - vertexArray[(x + (z + 1) * tex->width)*3 + 0]),
												   (vertexArray[(x + (z - 1) * tex->width)*3 + 1] - vertexArray[(x + (z + 1) * tex->width)*3 + 1]),
												   (vertexArray[(x + (z - 1) * tex->width)*3 + 2] - vertexArray[(x + (z + 1) * tex->width)*3 + 2]));
					vec3 normalVector = (CrossProduct(upDownVector, leftRightVector));
					normalArray[(x + z * tex->width)*3 + 0] = normalVector.x;
					normalArray[(x + z * tex->width)*3 + 1] = normalVector.y;
					normalArray[(x + z * tex->width)*3 + 2] = normalVector.z;
				}
				else
				{
					normalArray[(x + z * tex->width)*3 + 0] = 0.0;
					normalArray[(x + z * tex->width)*3 + 1] = 1.0;
					normalArray[(x + z * tex->width)*3 + 2] = 0.0;
				}
			}
		return normalArray;

	}

	Model* GenerateTerrain(TextureData *tex)
	{
		GLint vertexCount = tex->width * tex->height;
		

		GLfloat* vertexArray;
		GLfloat* normalArray;
		GLfloat* texCoordArray;
		GLuint* indexArray;
		
		printf("bpp %d\n", tex->bpp);

		vertexArray = GenerateTerrainVertexArray(tex);
		texCoordArray = GenerateTerrainTextureCoordinateArray(tex);
		indexArray = GenerateTerrainIndexArray(tex);
		normalArray = GenerateTerrainNormalArray(tex, vertexArray);		
		// End of terrain generation
		
		// Create Model and upload to GPU:

		GLint triangleCount = (tex->width-1) * (tex->height-1) * 2;

		Model* model = LoadDataToModel(
								vertexArray,
								normalArray,
								texCoordArray,
								NULL,
								indexArray,
								vertexCount,
								triangleCount*3);

		return model;
	}

	vec3* GetTransformedPositions(TextureData *tex, mat4 transformationMatrix)
	{
		GLint x, z;
		GLint vertexCount = tex->width * tex->height;
		vec4* startingPositions = malloc(sizeof(vec4) * vertexCount);
		vec3* transformedPositions = malloc(sizeof(vec3) * vertexCount);
		for (x = 0; x < tex->width; x++)
			for (z = 0; z < tex->height; z++)
			{
				// Code copied from generate terrain, be careful
				GLint currentIndex = (x + z * tex->width);
				startingPositions[currentIndex].x = x / 1.0;
				startingPositions[currentIndex].y = tex->imageData[currentIndex * (tex->bpp/8)] / terrainScale;
				startingPositions[currentIndex].z = z / 1.0;
				transformedPositions[currentIndex] = vec4tovec3(MultVec4(transformationMatrix, startingPositions[currentIndex]));
			}

		free(startingPositions);
		return transformedPositions;
	}

	vec3 GetBezierPoint( vec3* points, int numPoints, float u ) 
	{
	    vec3* tempPoints = malloc(sizeof(vec3) * numPoints);
	    memcpy(tempPoints, points, sizeof(vec3) * numPoints);
	    GLint i = numPoints - 1;
	    GLint k;
	    for (i; i > 0; --i) 
	    {
	        for (k = 0; k < i; k++)
	        {
	            tempPoints[k].x = tempPoints[k].x + u * ( tempPoints[k+1].x - tempPoints[k].x );
	            tempPoints[k].y = tempPoints[k].y + u * ( tempPoints[k+1].y - tempPoints[k].y );
	            tempPoints[k].z = tempPoints[k].z + u * ( tempPoints[k+1].z - tempPoints[k].z );
	        }
	    }

	    vec3 interpolatedPoint = SetVector(tempPoints[0].x, tempPoints[0].y, tempPoints[0].z);
	    free(tempPoints);
	    return interpolatedPoint;
	}

	//BROOOOKEN
	Model* GenerateCubeTerrain(TextureData *mainTexture, 
							   TextureData *leftConnectingTexture, 
							   TextureData *rightConnectingTexture, 
                       		   TextureData *upConnectingTexture, 
                       		   TextureData *downConnectingTexture,
                       		   mat4 modelMatrices[6])
	{

		Model* model = GenerateTerrain(mainTexture);

		//Overlap 10 atm
		GLint vertexCount = mainTexture->width * mainTexture->height;

		vec3* mainTransformedPosition = malloc(sizeof(vec3) * vertexCount);
		vec3* transformedPosition = malloc(sizeof(vec3) * vertexCount);
		mainTransformedPosition = GetTransformedPositions(mainTexture, modelMatrices[0]);
		

		//Left side Bezier curve
		transformedPosition = GetTransformedPositions(leftConnectingTexture, modelMatrices[1]);
		GLfloat u;
		GLint numberOfInterpolationPointsInMain = 5;
		GLint totalNumOfInterpolationPoints = numberOfInterpolationPointsInMain * 2 ;

		vec3 bezier;
		vec3 controlPoints[10];
		GLint x,z;
		
		for (z = 0; z < mainTexture->height; ++z)
		{
			controlPoints[0] = mainTransformedPosition[4 + (z*mainTexture->width)];
			controlPoints[1] = mainTransformedPosition[3 + (z*mainTexture->width)];
			controlPoints[2] = mainTransformedPosition[2 + (z*mainTexture->width)];
			controlPoints[3] = mainTransformedPosition[1 + (z*mainTexture->width)];
			controlPoints[4] = mainTransformedPosition[0 + (z*mainTexture->width)];

			controlPoints[5] = transformedPosition[((z+1)*leftConnectingTexture->width) - 5];
			controlPoints[6] = transformedPosition[((z+1)*leftConnectingTexture->width) - 4];
			controlPoints[7] = transformedPosition[((z+1)*leftConnectingTexture->width) - 3];
			controlPoints[8] = transformedPosition[((z+1)*leftConnectingTexture->width) - 2];
			controlPoints[9] = transformedPosition[((z+1)*leftConnectingTexture->width) - 1];


			
			for (x = 0; x < numberOfInterpolationPointsInMain; ++x)
			{
				GLfloat u = (GLfloat)x/(GLfloat)numberOfInterpolationPointsInMain;
				bezier = GetBezierPoint( controlPoints, totalNumOfInterpolationPoints, u);
				model->vertexArray[x + z * mainTexture->width + 0] = bezier.x;
				model->vertexArray[x + z * mainTexture->width + 1] = bezier.y;
				model->vertexArray[x + z * mainTexture->width + 2] = bezier.z;
			}	
		}


		//Bezier curves in z

		//Apply x[i]*z[j]

		free(mainTransformedPosition);
		free(transformedPosition);

		GLint triangleCount = (mainTexture->width-1) * (mainTexture->height-1) * 2;

		return LoadDataToModel(
					model->vertexArray,
					model->normalArray,
					model->texCoordArray,
					NULL,
					model->indexArray,
					vertexCount,
					triangleCount*3);
	}
	

	Model* GenerateCubeTerrainSimple(TextureData *mainTexture)
	{
		Model* model;
		model = GenerateTerrain(mainTexture);

		GLint x, z, edge;
		GLint roundingDistanceFromEdge = 10;

		GLfloat currentHeight;

		//Set height to go towards zero at the edge

		//x near 0
		for (x = 0; x < roundingDistanceFromEdge; ++x)
			for (z = 0; z < mainTexture->height; ++z)
			{
				currentHeight = model->vertexArray[(x + z * mainTexture->width)*3 + 1];
				model->vertexArray[(x + z * mainTexture->width)*3 + 1] = currentHeight * ((GLfloat)x/(GLfloat)roundingDistanceFromEdge);
			}
		//x far edge
		edge = mainTexture->width;
		for (x = (edge - roundingDistanceFromEdge); x < edge; ++x)
			for (z = 0; z < mainTexture->height; ++z)
			{
				currentHeight = model->vertexArray[(x + z * mainTexture->width)*3 + 1];
				model->vertexArray[(x + z * mainTexture->width)*3 + 1] = currentHeight * ((GLfloat)(edge - x - 1)/(GLfloat)roundingDistanceFromEdge);
			}

		//z near 0
		for (x = 0; x < mainTexture->width; ++x)
			for (z = 0; z < roundingDistanceFromEdge; ++z)
			{
				currentHeight = model->vertexArray[(x + z * mainTexture->width)*3 + 1];
				model->vertexArray[(x + z * mainTexture->width)*3 + 1] = currentHeight * ((GLfloat)z/(GLfloat)roundingDistanceFromEdge);
			}
		//z far edge	
		edge = mainTexture->height;
		for (x = 0; x < mainTexture->width; ++x)
			for (z = (edge-roundingDistanceFromEdge); z < edge; ++z)
			{
				currentHeight = model->vertexArray[(x + z * mainTexture->width)*3 + 1];
				model->vertexArray[(x + z * mainTexture->width)*3 + 1] = currentHeight * ((GLfloat)(edge - z - 1)/(GLfloat)roundingDistanceFromEdge);
			}

		model->indexArray = GenerateTerrainIndexArray(mainTexture);
		model->normalArray = GenerateTerrainNormalArray(mainTexture, model->vertexArray);


		GLint vertexCount = mainTexture->width * mainTexture->height;
		GLint triangleCount = (mainTexture->width-1) * (mainTexture->height-1) * 2;

		return LoadDataToModel(
					model->vertexArray,
					model->normalArray,
					model->texCoordArray,
					NULL,
					model->indexArray,
					vertexCount,
					triangleCount*3);
	}

	Model* MapCubeToSphere(Model* cubeModel, GLfloat radius, GLint arrayWidth, GLint arrayHeight)
	{	
		//Turn side of a unit cube to a unit sphere

		//Since corner is at 0:
		GLfloat distanceFromOriginX = cubeModel->vertexArray[(arrayWidth/2 + arrayHeight/2 * arrayWidth)*3 + 0];
		GLfloat distanceFromOriginY = cubeModel->vertexArray[(arrayWidth/2 + arrayHeight/2 * arrayWidth)*3 + 1] - radius;
		GLfloat distanceFromOriginZ = cubeModel->vertexArray[(arrayWidth/2 + arrayHeight/2 * arrayWidth)*3 + 2];


		GLint x, z;
		GLint vertexCount = arrayWidth * arrayHeight;
		GLint triangleCount = (arrayWidth-1) * (arrayHeight-1) * 2;
		GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
		for (x = 0; x < arrayWidth; x++)
			for (z = 0; z < arrayHeight; z++)
			{
				vec3 tempvec = SetVector(cubeModel->vertexArray[(x + z * arrayWidth)*3 + 0] - distanceFromOriginX, 
										 cubeModel->vertexArray[(x + z * arrayWidth)*3 + 1], 
										 cubeModel->vertexArray[(x + z * arrayWidth)*3 + 2] = z / 1.0 - distanceFromOriginZ);
				tempvec = Normalize(tempvec);
				cubeModel->vertexArray[(x + z * arrayWidth)*3 + 0] = tempvec.x * radius + distanceFromOriginX;
				cubeModel->vertexArray[(x + z * arrayWidth)*3 + 1] = tempvec.y * radius + distanceFromOriginY;
				cubeModel->vertexArray[(x + z * arrayWidth)*3 + 2] = tempvec.z * radius + distanceFromOriginZ;
			}

		return LoadDataToModel(
					cubeModel->vertexArray,					
					cubeModel->normalArray,
					cubeModel->texCoordArray,
					NULL,
					cubeModel->indexArray,
					vertexCount,
					triangleCount*3);
	}


	GLfloat GetTerrainHeight(vec3 currentPosition, Model *tm, TextureData tex)
	{
		GLint x,z;

		//Find the quad
		GLfloat x1 = floor(currentPosition.x - TerrainOffset.x);
		GLfloat x2 = floor(currentPosition.x - TerrainOffset.x + 1);

		GLfloat z1 = floor(currentPosition.z - TerrainOffset.z);
		GLfloat z2 = floor(currentPosition.z - TerrainOffset.z + 1);

		//Determine the triangle
		if( x1 < 0 || z1 < 0 || x2 > tex.width || (z2+1) > tex.height ) //Outside texture  (Add margin to z2 to stop segfault)
		{
			return 0;
		}
		else if( (x2 - currentPosition.x) > (currentPosition.z - z1)) //Left triangle
		{
			x = (GLint)x1;
			z = (GLint)z1;

		}
		else //Right triangle
		{
			x = (GLint)x2;
			z = (GLint)z2;
		}

		
		vec3 startPoint = SetVector(tm->vertexArray[(x + z * tex.width)*3 + 0], 
			                        tm->vertexArray[(x + z * tex.width)*3 + 1], 
			                        tm->vertexArray[(x + z * tex.width)*3 + 2]);

		vec3 vectorX = SetVector( tm->vertexArray[((GLint)x2 + (GLint)z1 * tex.width)*3 + 0],
								  tm->vertexArray[((GLint)x2 + (GLint)z1 * tex.width)*3 + 1],
								  tm->vertexArray[((GLint)x2 + (GLint)z1 * tex.width)*3 + 2]);
		vectorX = VectorSub(vectorX, startPoint);

		vec3 vectorZ = SetVector( tm->vertexArray[((GLint)x1 + (GLint)z2 * tex.width)*3 + 0],
								  tm->vertexArray[((GLint)x1 + (GLint)z2 * tex.width)*3 + 1],
								  tm->vertexArray[((GLint)x1 + (GLint)z2 * tex.width)*3 + 2]);
		vectorZ = VectorSub(vectorZ, startPoint);

		vec3 normal = Normalize(CrossProduct(vectorZ, vectorX));
		

		GLfloat height = (DotProduct(normal, startPoint) 
			             - (normal.x * (currentPosition.x - TerrainOffset.x)) 
			             - (normal.z * (currentPosition.z - TerrainOffset.z))) 
					     / normal.y;

		height = height + TerrainOffset.y;

		return height;
	}







