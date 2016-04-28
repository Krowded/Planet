#include "Terrain.h"
#include "kiss_fft/kiss_fftndr.h"

vec3 TerrainOffset = {0, 0, 0};

LOCAL GLfloat* GenerateTerrainVertexArray(TextureData *tex)
{
	GLint x, z;
	GLint vertexCount = tex->width * tex->height;
	GLfloat *vertexArray = chkmalloc(sizeof(GLfloat) * 3 * vertexCount);
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

LOCAL GLfloat* GenerateTerrainTextureCoordinateArray(TextureData *tex)
{
	GLint x, z;
	GLint vertexCount = tex->width * tex->height;
	GLfloat *texCoordArray = chkmalloc(sizeof(GLfloat) * 2 * vertexCount);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x;//(float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z;//(float)z / tex->height;
		}

	return texCoordArray;
}

LOCAL GLuint* GenerateTerrainIndexArray(TextureData *tex)
{
	GLint x, z;	
	GLint triangleCount = (tex->width-1) * (tex->height-1) * 2;
	GLuint *indexArray = chkmalloc(sizeof(GLuint) * triangleCount*3);

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

LOCAL GLfloat* GenerateTerrainNormalArray(TextureData *tex, GLfloat *vertexArray)
{
	GLint x, z;
	GLint vertexCount = tex->width * tex->height;
	GLfloat *normalArray = chkmalloc(sizeof(GLfloat) * 3 * vertexCount);
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



void GenerateProceduralTerrainTexture(GLint sideLength, TextureData* newTerrainTexture)
{
	GLint size = sideLength*sideLength;
	

	kiss_fft_scalar* image = chkmalloc(sizeof(kiss_fft_scalar)*size);
	kiss_fft_cpx* transform = chkmalloc(sizeof(kiss_fft_cpx)*(size/2+1));
	GLint dims[2] = {sideLength, sideLength};
	GLint ndims = 2;

	GLint i;
	for (i = 0; i < size; i++)
	{
		float r = (float)rand();
		image[i] = r * (1.0f/RAND_MAX);
	}
	kiss_fftndr_cfg cfg = kiss_fftndr_alloc(dims, ndims, false, NULL, NULL);

	kiss_fftndr(cfg, image, transform);


/*
	GLint j;
	GLfloat f;
	GLfloat power = 2.0;
	for(i = 0; i < sideLength/2; ++i)
		for(j = 0; j < sideLength; ++j)
		{
			GLint index = j + i * sideLength;
			f = sqrt( pow((i - sideLength/2)/(float)sideLength, 2) + pow((float)j - (float)sideLength/2.0f/(float)sideLength,2) );		
			//fprintf(stderr, "%f\n", pow(((float)j - (float)sideLength/2.0f)/(float)sideLength,2));
			f = f < 1.0f/sideLength ? 1.0f/sideLength : f;
			transform[index].i = transform[index].i * (1.0f / pow(f, power));
			transform[index].r = transform[index].r * (1.0f / pow(f, power));
			GLfloat magnitude = sqrt( pow(transform[index].i, 2) + pow(transform[index].r, 2) );
		}


			fprintf(stderr, "%f\n", pow(((float)sideLength - (float)sideLength/2.0f)/(float)sideLength,2));

*/

	//Use the transform as out for testing
	for(i = 0; i < size/2; ++i)
	{
		image[i] = sqrt( transform[i].r*transform[i].r + transform[i].i*transform[i].i );
		image[size/2+i] = image[i];
		//fprintf(stderr, "%f\n", image[i]);
	}

	free(cfg);
	cfg = kiss_fftndr_alloc(dims, ndims, true, NULL, NULL);

	//ifft
	//kiss_fftndri(cfg, transform, image);       //Uncomment this when the transform looks right!


	GLfloat max = 0;
	GLfloat min = 0;
	for(i = 0; i < size; i++)
		if( image[i] < min )
			min = image[i];

	fprintf(stderr, "Min: %f\n", min);


	for(i = 0; i < size; i++)
	{
		image[i] = image[i] - min;
		if( image[i] > max )
			max = image[i];
	}

	fprintf(stderr, "Max: %f\n", max);

	unsigned char* final = chkmalloc(sizeof(unsigned char)*size);
	for(i = 0; i < size; i++)
	{
		final[i] = (unsigned char)round(image[i]);
		//fprintf(stderr, "%d\n", final[i]);
	}

	SaveAsTGA("testTGA.tga", sideLength, sideLength, final);

	free(cfg);
	free(image);
	free(transform);
	free(final);
	newTerrainTexture->width = sideLength;
	newTerrainTexture->height = sideLength;
	newTerrainTexture->imageData = final;
	newTerrainTexture->bpp = 8;
}

LOCAL int SaveAsTGA(char* filename, short int width, short int height, unsigned char* data)
{
	FILE* file = fopen(filename, "w");
	unsigned char cGarbage = 0;
	unsigned char aone = 1;
	unsigned char pixelDepth = 8 + 16;
	char TGAuncompressedheader[12]={ 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	// write the header
	fwrite(&TGAuncompressedheader, 12, 1, file);
	fwrite(&width, sizeof(short int), 1, file);
	fwrite(&height, sizeof(short int), 1, file);
	fwrite(&pixelDepth, sizeof(unsigned char), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	int w = 1;
	while (w < width) w = w << 1;

	unsigned char buffer[width*height*3];

	int i;
	for(i = 0; i < width*height; i++)
	{
		buffer[3*i] = data[i];
		buffer[3*i+1] = data[i];
		buffer[3*i+2] = data[i];
	}

	
	for (i = 0 ; i < height; i++)
	{
		int rowIndex = i * w * 3;
		fwrite(&buffer[rowIndex], sizeof(unsigned char), width*3, file);
	}
	fclose(file);
	
	return 0;
}











Model* GenerateTerrainFromTexture(TextureData *tex)
{
	GLint vertexCount = tex->width * tex->height;
	

	GLfloat* vertexArray;
	GLfloat* normalArray;
	GLfloat* texCoordArray;
	GLuint* indexArray;

	vertexArray = GenerateTerrainVertexArray(tex);
	texCoordArray = GenerateTerrainTextureCoordinateArray(tex);
	indexArray = GenerateTerrainIndexArray(tex);
	normalArray = GenerateTerrainNormalArray(tex, vertexArray);		
	// End of terrain generation
	
	// Create Model and upload to GPU:

	GLint triangleCount = (tex->width-1) * (tex->height-1) * 2;

	return LoadDataToModel(
					vertexArray,
					normalArray,
					texCoordArray,
					NULL,
					indexArray,
					vertexCount,
					triangleCount*3);
}

vec3* GetTransformedPositions(TextureData *tex, mat4 transformationMatrix)
{
	GLint x, z;
	GLint vertexCount = tex->width * tex->height;
	vec4* startingPositions = chkmalloc(sizeof(vec4) * vertexCount);
	vec3* transformedPositions = chkmalloc(sizeof(vec3) * vertexCount);
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

LOCAL vec3 GetBezierPoint( vec3* points, int numPoints, float u ) 
{
    vec3* tempPoints = chkmalloc(sizeof(vec3) * numPoints);
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


LOCAL GLfloat RoundingFunction(GLfloat t)
{
	return pow(t,2);
}


Model* GenerateCubeTerrainSimple(struct PlanetStruct planet)
{
	Model* model;
	model = GenerateTerrainFromTexture(planet.terrainTexture[0]);

	GLint x, z, edge;	

	GLfloat currentHeight;
	GLint arrayWidth = planet.terrainTexture[0]->width;
	GLint arrayHeight = planet.terrainTexture[0]->height;

	//Set height to go towards zero at the edge
	GLfloat b = 1;
	GLfloat c = -1;


	//x near 0
	for (x = 0; x < roundingDistanceFromEdge; ++x)
		for (z = 0; z < arrayHeight; ++z)
		{
			currentHeight = model->vertexArray[(x + z * arrayWidth)*3 + 1];
			model->vertexArray[(x + z * arrayWidth)*3 + 1] = currentHeight * RoundingFunction((GLfloat)x/(GLfloat)roundingDistanceFromEdge);
		}
	//x far edge
	edge = arrayWidth;
	for (x = (edge - roundingDistanceFromEdge); x < edge; ++x)
		for (z = 0; z < arrayHeight; ++z)
		{
			currentHeight = model->vertexArray[(x + z * arrayWidth)*3 + 1];
			model->vertexArray[(x + z * arrayWidth)*3 + 1] = currentHeight * RoundingFunction((GLfloat)(edge - x - 1)/(GLfloat)roundingDistanceFromEdge);
		}

	//z near 0
	for (x = 0; x < arrayWidth; ++x)
		for (z = 0; z < roundingDistanceFromEdge; ++z)
		{
			currentHeight = model->vertexArray[(x + z * arrayWidth)*3 + 1];
			model->vertexArray[(x + z * arrayWidth)*3 + 1] = currentHeight * RoundingFunction((GLfloat)z/(GLfloat)roundingDistanceFromEdge);
		}
	//z far edge	
	edge = arrayHeight;
	for (x = 0; x < arrayWidth; ++x)
		for (z = (edge-roundingDistanceFromEdge); z < edge; ++z)
		{
			currentHeight = model->vertexArray[(x + z * arrayWidth)*3 + 1];
			model->vertexArray[(x + z * arrayWidth)*3 + 1] = currentHeight * RoundingFunction((GLfloat)(edge - z - 1)/(GLfloat)roundingDistanceFromEdge);
		}

	model->indexArray = GenerateTerrainIndexArray(planet.terrainTexture[0]);
	model->normalArray = GenerateTerrainNormalArray(planet.terrainTexture[0], model->vertexArray);


	GLint vertexCount = arrayWidth * arrayHeight;
	GLint triangleCount = (arrayWidth-1) * (arrayHeight-1) * 2;

	return LoadDataToModel(
				model->vertexArray,
				model->normalArray,
				model->texCoordArray,
				NULL,
				model->indexArray,
				vertexCount,
				triangleCount*3);
}


//Turn side of a unit cube to a unit sphere
Model* MapCubeToFlatSphere(struct PlanetStruct planet, GLint i) //i = index of side
{
	GLint x;
	for(x = 0; x < planet.terrainTexture[i]->width*planet.terrainTexture[i]->width*3; x += 3)
	{
		vec4 point = {planet.terrainModels[i]->vertexArray[x + 0],
					  planet.terrainModels[i]->vertexArray[x + 1],
					  planet.terrainModels[i]->vertexArray[x + 2],
					  1};

		point = MultVec4(planet.terrainModelToWorld[i], point);

		vec3 newPoint = Normalize(SetVector(point.x,point.y,point.z));
		newPoint = ScalarMult(newPoint, planet.radius);

		
		planet.terrainModels[i]->vertexArray[x + 0] = newPoint.x;
		planet.terrainModels[i]->vertexArray[x + 1] = newPoint.y;
		planet.terrainModels[i]->vertexArray[x + 2] = newPoint.z;
	}
		
	return LoadDataToModel(
				planet.terrainModels[i]->vertexArray,
				planet.terrainModels[i]->normalArray,
				planet.terrainModels[i]->texCoordArray,
				NULL,
				planet.terrainModels[i]->indexArray,
				planet.terrainModels[i]->numVertices,
				planet.terrainModels[i]->numIndices);
}

Model* MapCubeToSphere(struct PlanetStruct planet, GLint i) //i = index of side
{
	GLint x;
	for(x = 0; x < planet.terrainTexture[i]->width*planet.terrainTexture[i]->width*3; x += 3)
	{
		vec4 transformedPoint = {planet.terrainModels[i]->vertexArray[x + 0],
					  			 planet.terrainModels[i]->vertexArray[x + 1],
					  			 planet.terrainModels[i]->vertexArray[x + 2],
					  	 		 1};

		vec4 flatTransformedPoint = {planet.terrainModels[i]->vertexArray[x + 0],
					  				 0,
					  				 planet.terrainModels[i]->vertexArray[x + 2],
					  	 			 1};

		transformedPoint = MultVec4(planet.terrainModelToWorld[i], transformedPoint);
		flatTransformedPoint = MultVec4(planet.terrainModelToWorld[i], flatTransformedPoint);

		vec3 newPoint = Normalize(vec4tovec3(flatTransformedPoint));
		newPoint = ScalarMult(newPoint, planet.radius);

		vec3 difference = VectorSub(newPoint, vec4tovec3(flatTransformedPoint));
		
		planet.terrainModels[i]->vertexArray[x + 0] = transformedPoint.x + difference.x;
		planet.terrainModels[i]->vertexArray[x + 1] = transformedPoint.y + difference.y;
		planet.terrainModels[i]->vertexArray[x + 2] = transformedPoint.z + difference.z;
	}
		
	return LoadDataToModel(
				planet.terrainModels[i]->vertexArray,
				planet.terrainModels[i]->normalArray,
				planet.terrainModels[i]->texCoordArray,
				NULL,
				planet.terrainModels[i]->indexArray,
				planet.terrainModels[i]->numVertices,
				planet.terrainModels[i]->numIndices);
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