struct Vertex
{
	float x, y, z; 
	float r, g, b;
};

struct VertexData
{
	std::vector<Vertex> v;
	int xChunk;
	int yChunk;
};

class VertexBuffer
{
public:
	int xChunk;
	int yChunk;

	VertexBuffer(void* data, int numVertices, int xC, int yC)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &bufferID); //Hier können Vertices direkt im GPU-Speicher abgelegt werden
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, x)); //statt offsetof(struct Vertex,x) kann auch 0 stehen

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, r));

		//offset gibt an wo die Werte beginnen.

		glBindVertexArray(0);

		xChunk = xC;
		yChunk = yC;
	}

	~VertexBuffer()
	{
		glDeleteBuffers(1, &bufferID);
	}

	void Bind()
	{
		glBindVertexArray(vao);
	}

	void Unbind()
	{
		glBindVertexArray(0);
	}

	void ResizeBuffer(void* data, int numVertices)
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

private:
	GLuint bufferID;
	GLuint vao;
};
