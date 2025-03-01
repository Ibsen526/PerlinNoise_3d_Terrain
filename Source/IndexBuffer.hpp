struct IndexData
{
	std::vector<Uint32> i;
	Uint32 size;
};

class IndexBuffer
{
public:
	int iboSize = 0;

	IndexBuffer()
	{
		bufferID = 0;
	}

	IndexBuffer(void* data, int numIndices, Uint8 elementSize)
	{
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, numIndices * elementSize, data, GL_STATIC_DRAW);
		iboSize = numIndices;
	}

	~IndexBuffer()
	{
		glDeleteBuffers(1, &bufferID);
	}

	void Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	}

	void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void ResizeBuffer(void* data, int numVertices, Uint8 indexSize)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numVertices * indexSize, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

private:
	GLuint bufferID;
};
