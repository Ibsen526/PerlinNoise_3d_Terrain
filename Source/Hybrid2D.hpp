#pragma once

class Hybrid2D
{
public:
	int octaves = 8;
	float influence = 2.0f;
	bool color = true;
	int xOffset = 0;
	int yOffset = 0;

	Hybrid2D()
	{
		seedTable = new float[SW * SH];
		output = new float[SW * SH];
		seed = 0;
	}

	~Hybrid2D()
	{
		delete[] seedTable;
		delete[] output;
	}

	void NewSeed()
	{
		seed = std::clock() + time(NULL);
	}

	void CalcSeeds()
	{
		std::mt19937 mt(seed); //hier vielleicht anderer rnd generator nehmen, mit besserem seeding
		//std::mt19937 mt(seed + xOffset + yOffset * SW);
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		for (Uint16 x = 0; x < SW; x++)
		{
			for (Uint16 y = 0; y < SH; y++)
			{
				seedTable[x + y * SW] = dis(mt);
				output[x + y * SW] = 0.0f;
			}
		}
	}

	void GenNoise()
	{
		for (int x = 0; x < SW; x++)
		{
			for (int y = 0; y < SH; y++)
			{
				float noiseAcc = 0.0f;
				float scaleAcc = 0.0f;
				float scale = 1.0f;
				int secLenW = SW;
				int secLenH = SH;

				for (int o = 0; o < octaves; o++)
				{
					// -- Samples --
					int sampleX1 = (x / secLenW) * secLenW; //The next sample left of x
					int sampleX2 = (sampleX1 + secLenW) % SW; //The next sample right of x

					int sampleY1 = (y / secLenH) * secLenH; //Boundaries
					int sampleY2 = (sampleY1 + secLenH) % SH;


					// -- Interpolation --
					float sx = (float)(x - sampleX1) / (float)secLenW; //The x of our desired point
					float sy = (float)(y - sampleY1) / (float)secLenH; //The y of our desired point

					float sampleXT = (1.0f - sx) * seedTable[sampleX1 + sampleY1 * SW] + sx * seedTable[sampleX2 + sampleY1 * SW]; //Interpolate between top x
					float sampleXB = (1.0f - sx) * seedTable[sampleX1 + sampleY2 * SW] + sx * seedTable[sampleX2 + sampleY2 * SW]; //Interpolate between bottom x

					//float sampleY = (1.0f - sy) * sampleXT + sy * sampleXB; //Interpolate between the interpolated x values
					float sampleY = (sy * (sampleXB - sampleXT) + sampleXT); //The same as above, just shorter


					// -- Accumulation --
					noiseAcc += sampleY * scale; //Scales the current sample at x according to the octave; lower octaves have more influence than higher
					scaleAcc += scale; //Accumulates all of the scaling values, to scale the end result

					scale /= influence; //The lower octaves have more influence than the higher ones
					secLenW /= 2.0f; //Halfs the sector lenght each ocatave
					secLenH /= 2.0f; 
				}

				float result = noiseAcc / scaleAcc; //Scales the result, because otherwise the graph would get higher with more octaves

				output[x + y * SW] = result;
			}
		}
	}

	void DrawNoise(SDL_Renderer* renderer)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		
		for (Uint16 x = 0; x < SW; x++)
		{
			for (Uint16 y = 0; y < SH; y++)
			{
				if (!color)
				{
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, (output[x + y * SW] * 255.0f));
					SDL_RenderDrawPoint(renderer, x, y);
				}
				else
				{
					float v = output[x + y * SW];
					int val = (int) (v * 255.0f);
					int valB = (int) NormalizeColor(v, 100.0f, 101, 110);
					int valL = (int) NormalizeColor(v, 50.0f, 111, 120);
					int valF = (int) NormalizeColor(v, 100.0f, 121, 189);
					int valM = (int) NormalizeColor(v, 200.0f, 190, 255);

					if (val <= 100)					  SDL_SetRenderDrawColor(renderer, 0, val, 50 + val, 255); //Water
					else if (val > 100 && val <= 110) SDL_SetRenderDrawColor(renderer, 170 - valB, 170 - valB, 0, 255); //Beach
					else if (val > 110 && val <= 120) SDL_SetRenderDrawColor(renderer, 70 - valL, 80 - valL, 0, 255); //Jungle (light)
					else if (val > 120 && val < 190)  SDL_SetRenderDrawColor(renderer, valF + 5, valF + 25, 0, 255); //Jungle (full)
					else if (val >= 190 && val <= 255) SDL_SetRenderDrawColor(renderer, 400 - valM, 400 - valM, 400 - valM, 255); //Mountains with snow
					
					SDL_RenderDrawPoint(renderer, x, y);
				}
			}
		}
		SDL_RenderPresent(renderer);
	}

private:
	float* output;
	float* seedTable;
	int seed;
	//float seedTable[SW][SH];

	float NormalizeColor(float col, float scale, int b1, int b2)
	{
		float sb1 = (float)b1 / 255.0f; //The boundaries are given in 0-255-space, and must be normalized and then scaled
		float sb2 = (float)b2 / 255.0f;

		float normalized = (col - sb1) / (sb2 - sb1); //Normalizes the color between b1 and b2 to a value between 0 and 1

		//if (scale == 50.0f)
		//	std::cout << "sdf";
		return normalized * scale; //Scale the color
	}
};

