#pragma once

class Hybrid1D
{
public:
	int octaves = 8;

	void CalcSeeds()
	{
		std::mt19937 mt(std::clock());
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		for (Uint16 x = 0; x < SW; x++)
		{
			seedTable[x] = dis(mt);
		}
	}

	void GenNoise(SDL_Renderer* renderer)
	{
		for (int x = 0; x < SW; x++)
		{
			float noiseAcc = 0.0f;
			float scaleAcc = 0.0f;
			float scale = 1.0f;
			int secLen = SW;

			for (int o = 0; o < octaves; o++)
			{
				// -- Samples --
				int sampleX1 = (x / secLen) * secLen; //The next sample left of x
				int sampleX2 = (sampleX1 + secLen) % SW; //The next sample right of x


				// -- Interpolation --
				float sx = (float)(x - sampleX1) / (float)secLen; //The x of our desired point

				float sample = (1.0f - sx) * seedTable[sampleX1] + sx * seedTable[sampleX2]; //Adds the areas of the triangles and this results in y

				// -- Accumulation --
				noiseAcc += sample * scale; //Scales the current sample at x according to the octave; lower octaves have more influence than higher
				scaleAcc += scale; //Accumulates all of the scaling values, to scale the end result

				scale /= 2.0f; //The lower octaves have more influence than the higher ones
				secLen /= 2.0f; //Halfs the sector lenght each ocatave
			}

			float result = noiseAcc / scaleAcc; //Scales the result, because otherwise the graph would get higher with more octaves

			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			int y = -(result * (float)SH / 2.0f) + (float)SH / 2.0f;
			SDL_RenderDrawLine(renderer, x, SH, x, SH - y);
		}
	}

private:
	float seedTable[SW];
};
