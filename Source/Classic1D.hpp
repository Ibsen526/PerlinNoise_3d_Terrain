#pragma once

class Classic1D
{
public:
	Classic1D()
	{
		sectors = SW / secSize;
		seedTable = new Vec2[sectors];
	}

	~Classic1D()
	{
		delete[] seedTable;
	}

	void CalcSeeds()
	{
		std::mt19937 mt(std::clock());
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		for (Uint16 x = 0; x < sectors; x++)
		{
			seedTable[sectors] = Vec2(dis(mt), dis(mt));
		}
	}

	void GenNoise(SDL_Renderer* renderer)
	{
		for (int x = 0; x < SW; x++)
		{

		}
	}

private:
	class Vec2
	{
	public:
		float x, y;
		Vec2()
		{
			x = 0.0f; y = 0.0f;
		}
		Vec2(float xp, float yp)
		{
			x = xp; y = yp;
		}
	};

	float Dot(Vec2 a, Vec2 b)
	{
		return a.x * b.x + a.y * b.y;
	}

	const int secSize = 16;
	int sectors;
	Vec2* seedTable;
};
