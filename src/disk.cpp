#include "disk.h"

#include <iostream>
#include <vector>

#ifdef _WIN32
#include <glad/glad.h>

#elif __APPLE__
#include <OpenGL/gl3.h>

#elif __linux__
#include <glad/glad.h>
#endif

DiskPtr Disk::Make(int num_segments)
{
	return DiskPtr(new Disk(num_segments));
}

Disk::Disk(int num_segments)
{
	//centro do circulo
	coord.push_back(0.0f);
	coord.push_back(0.0f);

	//mapeamento da textura [0 .. 1]
	coord.push_back(0.5f); // tex.u (centro da textura)
	coord.push_back(0.5f); // tex.v (centro da textura)

	const float twice_pi = 2.0f * 3.142f;
	for (int i = 0; i <= num_segments; i++)
	{
		float angle = twice_pi * float(i) / float(num_segments); //theta_i
		coord.push_back(0.5f * cos(angle));
		coord.push_back(0.5f * sin(angle));

		// mapeamento da textura [0 .. 1]
		float u = 0.5f + 0.5f * cos(angle); // tex.u
		float v = 0.5f + 0.5f * sin(angle); // tex.v
		coord.push_back(u);
		coord.push_back(v);
	}
	// create VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// create coord buffer
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, coord.size() * sizeof(float), coord.data(), GL_STATIC_DRAW);

	GLsizei stride = 4 * sizeof(float);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0); // coord
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));  // texcoord
	glEnableVertexAttribArray(1);

}

Disk::~Disk()
{
}

void Disk::Draw(StatePtr state)
{
	//auto shader = state->GetShader();

	//shader->ActiveTexture(m_text_name);


	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, coord.size()/4);

	//shader->DeactiveTexture();

}