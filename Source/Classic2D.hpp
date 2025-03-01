#pragma once

class Classic2D
{
public:
	float xOffset;
	float yOffset;
	const int chunkSize = 100;
	int NW, NH;
	glm::ivec2 playerChunk;

	Classic2D()
	{
		xOffset = 50.0f;
		yOffset = 50.0f;
		NW = chunkSize;
		NH = chunkSize;
		output = new float[NW * NH];
		seed = 0;
		playerChunk = { 0,0 };
	}

	~Classic2D()
	{
		delete[] output;
	}

	void NewSeed()
	{
		seed = std::clock() + time(NULL);
	}

	void GenNoise(float xOff, float yOff)
	{
		for (int x = 0; x < NW; x++)
		{
			for (int y = 0; y < NH; y++)
			{
				float px = ((float)x + xOff) / (float)NW;
				float py = ((float)y + yOff) / (float)NH;

				//Standard
				//output[x + y * NW] = Noise(px * 5.0f, py * 5.0f);

				//Hills and valleys (steep)
				//output[x + y * NW] = (
				//	Noise(px * 5.0f, py * 5.0f) +
				//	Noise(px * 0.5f, py * 0.5f) ) / 1.3f;

				//Hills and valleys (moderate)
				output[x + y * NW] = (
					Noise(px * 3.0f, py * 3.0f) +
					Noise(px * 2.0f, py * 2.0f) +
					Noise(px * 0.5f, py * 0.5f)) / 1.8f;
			}
		}
	}

	void Draw2DNoise(Shader* shader, int resId, int offId)
	{
		shader->Bind();

		Vertex v[6] = { 
			{-1.0f,-1.0f,0.0f, 0,0,0}, {-1.0f,1.0f,0.0f, 0,0,0}, {1.0f,1.0f,0.0f, 0,0,0},
			{-1.0f,-1.0f,0.0f, 0,0,0}, {1.0f,-1.0f,0.0f, 0,0,0}, {1.0f,1.0f,0.0f, 0,0,0} };
		VertexBuffer vbo(v, 6, 0, 0);

		vbo.Bind();

		glUniform2f(resId, SW, SH);
		glUniform2f(offId, xOffset, yOffset);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		vbo.Unbind();
		shader->Unbind();
	}

	void Store3DChunk(std::vector<Vertex>& v, std::vector<Uint32>& i, float xOff, float yOff)
	{
		v.clear();
		i.clear();
		for (Uint32 y = 0; y < chunkSize; y++)
		{
			for (Uint32 x = 0; x < chunkSize; x++)
			{
				v.push_back({
					((float)x + xOff) / 25.0f, output[x + y * chunkSize] * 2.0f, ((float)y + yOff) / 25.0f,
					output[x + y * chunkSize], output[x + y * chunkSize], output[x + y * chunkSize] });

				if (y < chunkSize - 1 && x < chunkSize - 1)
				{
					i.push_back(x + y * chunkSize); //First Triangle
					i.push_back((x + 1) + y * chunkSize);
					i.push_back(x + (y + 1) * chunkSize);

					i.push_back((x + 1) + y * chunkSize); //Second Triangle
					i.push_back(x + (y + 1) * chunkSize);
					i.push_back((x + 1) + (y + 1) * chunkSize);
				}
			}
		}

	}

	void Draw3DNoise(VertexBuffer* vbo, IndexBuffer* ibo, size_t iSize,
		int mvpId, glm::mat4 mvp, Shader* shader) 
	{
		shader->Bind();
		vbo->Bind();
		ibo->Bind();

		glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);
		glDrawElements(GL_TRIANGLES, iSize, GL_UNSIGNED_INT, 0);

		ibo->Unbind();
		vbo->Unbind();
		shader->Unbind();
	}

private:
	float* output;
	int seed;

	float Noise(float x, float y)
	{
		float ix = floorf(x);
		float iy = floorf(y);
		float fx = x - ix;
		float fy = y - iy;

		// Four corners in 2D of a tile
		float a = Random(ix, iy);
		float b = Random(ix + 1.0f, iy + 0.0f);
		float c = Random(ix + 0.0f, iy + 1.0f);
		float d = Random(ix + 1.0f, iy + 1.0f);

		// Cubic Hermine Curve.  Same as SmoothStep()
		float sx = fx * fx * (3.0f - 2.0f * fx);
		float sy = fy * fy * (3.0f - 2.0f * fy);

		// Mix 4 coorners percentages
		return Lerp(a, b, sx) +
			(c - a) * sy * (1.0f - sx) +
			(d - b) * sx * sy;
	}

	float inline Random(float x, float y) 
	{
		//float orig = sin(Dot(x, y, 12.9898f, 78.233f)) * 43758.5453123f;
		float orig = sin(Dot(x, y, 15.0f, 80.0f)) * 43758.5453123f;
		return orig - floorf(orig);
	}

	float inline Dot(float x1, float y1, float x2, float y2)
	{
		return x1 * x2 + y1 * y2;
	}

	float inline Lerp(float x1, float x2, float x)
	{
		return ((x2 - x1) * x + x1);
		//or
		//return (1.0f - x) * x1 + x * x2;
	}
};