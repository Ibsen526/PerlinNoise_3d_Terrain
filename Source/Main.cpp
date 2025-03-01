#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <Windows.h>
#include <thread>
#include <future>
#include <chrono>
#include <random>
#include <thread>

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

const int SW = 1280;
const int SH = 720;

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

#include "Hybrid1D.hpp"
#include "Hybrid2D.hpp"
#include "Classic1D.hpp"
#include "Classic2D.hpp"

const int renderDistance = 3; // How many chunks will be displayed around the player

// Determines if the generated tiles will be deleted or not
//#define NO_PATH 
#define PATH

void ThreadedLoad(Classic2D* tp, std::vector<Vertex>* v, std::vector<Uint32>* i, std::vector<VertexBuffer*> vbo,
	std::vector<VertexData>* vboData, std::vector<IndexData>* iboData, bool* r)
{
	#ifdef PATH
		vboData->clear();
		iboData->clear();
	#endif

	// Go through all the surrounding chunks and check their location against the chunkVector
	// If its not already in there, then load this chunk and add it to the vbo/ibo-Vector
	for (int x = -renderDistance - 1; x <= renderDistance + 1; x++)
	{
		for (int y = -renderDistance - 1; y <= renderDistance + 1; y++)
		{
			int xRel = x + tp->playerChunk.x;
			int yRel = y + tp->playerChunk.y;

			bool exist = false;
			for (VertexBuffer* tVbo : vbo)
			{
				if (tVbo->xChunk == xRel && tVbo->yChunk == yRel)
				{
					exist = true;
					break;
				}
			}
			if (!exist)
			{
				float xChunk = (float)(xRel * tp->chunkSize) - (float)xRel;
				float yChunk = (float)(yRel * tp->chunkSize) - (float)yRel;
				tp->GenNoise(xChunk, yChunk);
				tp->Store3DChunk(*v, *i, xChunk, yChunk);

				vboData->push_back({ *v, xRel, yRel });
				iboData->push_back({ *i, (Uint32)i->size() });
			}
		}
	}

#ifdef NO_PATH
	for (Uint32 i = 0; i < vboData->size(); i++)
	{
		if (abs(vboData->at(i).xChunk - tp->playerChunk.x) > renderDistance + 3 ||
		abs(vboData->at(i).yChunk - tp->playerChunk.y) > renderDistance + 3)
		{
			vboData->erase(vboData->begin() + i);
			iboData->erase(iboData->begin() + i);
			i = 0;
		}
	}
#endif

	*r = true;
}

void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei lenght, const GLchar* message, const void* userParam) //Bei Fehler ruft OpenGl diese Funktion auf
{
	if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) 
		std::cout << "[OpenGL Error] " << message << std::endl;
}

int main(int argc, char* args[])
{
	SDL_Window* window = NULL;

	SDL_Event event;
	bool quit = false;
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef _DEBUG // Only for debugging (slower than release)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: %s\n", SDL_GetError();
		return -1;
	}

	// Create window
	window = SDL_CreateWindow("Procedural Generation Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SW, SH, SDL_WINDOW_OPENGL);
	if (window == NULL)
	{
		std::cout << "Window could not be created! SDL_Error: %s\n", SDL_GetError();
		return -1;
	} 
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	// Initialize Glew
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		std::cin.get();
		return -1;
	}
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

#ifdef _DEBUG // Initialize the debug callback
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLDebugCallback, 0);
#endif

	glEnable(GL_DEPTH_TEST);
	SDL_ShowCursor(false);

	std::vector<Vertex> vLand;
	std::vector<Uint32> iLand;

	Shader shader3D("shader3D.vert", "shader3D.frag");
	Shader shader2D("shader2D.vert", "shader2D.frag");

	GLint mvpUniId = glGetUniformLocation(shader3D.GetShaderID(), "u_mvp");
	GLint resUniId = glGetUniformLocation(shader2D.GetShaderID(), "resolution");
	GLint offUniId = glGetUniformLocation(shader2D.GetShaderID(), "offset");

	glm::mat4 model = glm::mat4(1.0f);
		
	Classic2D p2D;

	Camera cam;

	p2D.NewSeed();

	std::vector<VertexBuffer*> vboLand; 
	std::vector<IndexBuffer*> iboLand;

	std::vector<VertexData> fullVboData;
	std::vector<IndexData> fullIboData;

	for (int x = -renderDistance; x <= renderDistance; x++)
	{
		for (int y = -renderDistance; y <= renderDistance; y++)
		{
			float xChunk = (float)(x * p2D.chunkSize) - (float)x;
			float yChunk = (float)(y * p2D.chunkSize) - (float)y;
			p2D.GenNoise(xChunk, yChunk);			
			p2D.Store3DChunk(vLand, iLand, xChunk, yChunk);

			vboLand.push_back(new VertexBuffer(&vLand[0], vLand.size(), x, y));
			fullVboData.push_back({ vLand, x, y });

			iboLand.push_back(new IndexBuffer(&iLand[0], iLand.size(), sizeof(iLand[0])));
			fullIboData.push_back({ iLand, (Uint32)iLand.size() });
		}
	}


	std::thread noiseGeneration;
	bool resizeBuffer = false;

	noiseGeneration = std::thread(ThreadedLoad, &p2D, &vLand, &iLand, vboLand,
		&fullVboData, &fullIboData, &resizeBuffer);
	noiseGeneration.join();
	resizeBuffer = false;

	// FPS limiting
	Uint64 perfFreq = SDL_GetPerformanceFrequency();
	Uint64 timeStart = SDL_GetPerformanceCounter();


	const float baseSpeed = 2.0f;
	float speed = 2.0f;
	while (!quit) // Main loop
	{	
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (Uint16 i = 0; i < vboLand.size(); i++)
			p2D.Draw3DNoise(vboLand[i], iboLand[i], iboLand[i]->iboSize, mvpUniId, cam.proj * cam.view * model, &shader3D);
		
		SDL_GL_SwapWindow(window);

		if (resizeBuffer)
		{
			noiseGeneration.join();
			resizeBuffer = false;
		}

		bool renew = false;
		// Handle events on queue
		if (SDL_PollEvent(&event) != 0)
		{
			// User requests quit
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_SPACE)
				{
					p2D.NewSeed();
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				cam.RotateCamera(window, SW, SH);
			}
		}

		// Movement
		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
		cam.ChangeSpeedKey(currentKeyStates, baseSpeed, speed);

		if (currentKeyStates[SDL_SCANCODE_W])
		{
			p2D.yOffset += speed;
			renew = true;
		}
		else if (currentKeyStates[SDL_SCANCODE_S])
		{
			p2D.yOffset -= speed;
			renew = true;
		}
		if (currentKeyStates[SDL_SCANCODE_A])
		{
			p2D.xOffset += speed;
			renew = true;
		}
		else if (currentKeyStates[SDL_SCANCODE_D])
		{
			p2D.xOffset -= speed;
			renew = true;
		}

		if (renew) 
		{
			cam.MoveCamera(p2D.xOffset / 25.0f, p2D.yOffset / 25.0f);

			glm::ivec2 oldPlayerChunk = p2D.playerChunk;

			p2D.playerChunk = { (int)floor(p2D.xOffset / (float)p2D.chunkSize), // The chunks skip the 0, because 0 has no + or -
								(int)floor(p2D.yOffset / (float)p2D.chunkSize) }; 
			

			if (p2D.playerChunk != oldPlayerChunk)
			{
				#ifdef NO_PATH
					for (Uint32 i = 0; i < vboLand.size(); i++)
					{
						delete vboLand[i];
						delete iboLand[i];
					}
					vboLand.clear();
					iboLand.clear();
				#endif

				for (Uint32 i = 0; i < fullVboData.size(); i++)
				{
					if (abs(fullVboData[i].xChunk - p2D.playerChunk.x) <= renderDistance &&
						abs(fullVboData[i].yChunk - p2D.playerChunk.y) <= renderDistance)
					{
						vboLand.push_back(new VertexBuffer(fullVboData[i].v.data(), fullVboData[i].v.size(), fullVboData[i].xChunk, fullVboData[i].yChunk));
						iboLand.push_back(new IndexBuffer(fullIboData[i].i.data(), fullIboData[i].size, sizeof(fullIboData[i].i[0])));
					}
				}

				if(!noiseGeneration.joinable())
					noiseGeneration = std::thread(ThreadedLoad, &p2D, &vLand, &iLand, vboLand,
						&fullVboData, &fullIboData, &resizeBuffer);
			}
		}

		// Wait according to fps (60fps -> 16.6ms)
		Uint64 timeEnd = SDL_GetPerformanceCounter();
		float delta = (float)(timeEnd - timeStart) / (float)perfFreq * 1000.0f;
		float msPerFrame = 1000.0f / 60.0f;
		if (delta < msPerFrame)
			SDL_Delay(floor(msPerFrame - delta));
		timeStart = timeEnd;
	}

	for (size_t i = 0; i < vboLand.size(); i++)
	{
		delete vboLand[i];
		delete iboLand[i];
	}

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}
