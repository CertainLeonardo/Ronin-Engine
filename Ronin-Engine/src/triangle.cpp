#include "glad/gl.h"

int drawTriangle()
{
    float vertices[] = 
    {
        // vertex 1
        -0.5f, -0.5f, 0.10f,
        // vertex 2
        0.5f, -0.5f, 0.0f,
        // vertex 3
        0.0f, 0.5f, 0.0f
    };

    // Create Vertex Buffer Object (VBO) and Vertex Array Object (VAO)
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO and copy vertex data to buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // upload vertex data to GPU
    glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(vertices), 
        vertices, 
        GL_STATIC_DRAW
    );

    // Describe vertex data
    glVertexAttribPointer(
        0, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        3 * sizeof(float), 
        (void*)0
    );
    glEnableVertexAttribArray(0);

    return 0;
}