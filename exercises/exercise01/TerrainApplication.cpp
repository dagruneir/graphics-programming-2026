#include "TerrainApplication.h"
#include <cmath>
#include <iostream>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

// Helper structures. Declared here only for this exercise
struct Vector2
{
    Vector2() : Vector2(0.f, 0.f) {}
    Vector2(float x, float y) : x(x), y(y) {}
    float x, y;
};

struct Vector3
{
    Vector3() : Vector3(0.f,0.f,0.f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x, y, z;

    Vector3 Normalize() const
    {
        float length = std::sqrt(1 + x * x + y * y);
        return Vector3(x / length, y / length, z / length);
    }
};

struct Vertex
{
    Vector3 position;
    Vector2 texCoord;
    Vector3 color;
    Vector3 normal;
};



TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(80), m_gridY(80), m_shaderProgram(0)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Build shaders and store in m_shaderProgram
    BuildShaders();

    //std::vector<Vector3> positions;
    //std::vector<Vector2> texCoords;
    //std::vector<Vector3> colors;
    
    std::vector<uint32_t> indices;

    std::vector<Vertex> vertices;

    float scaleX = 1.0f / m_gridX;
    float scaleY = 1.0f / m_gridY;

    for (int y = 0; y <= m_gridY; y++)
    {
        for (int x = 0; x <= m_gridX; x++)
        {
            // Using EBO
            float px = x * scaleX - 0.5f;
            float py = y * scaleY - 0.5f;

            float lacunarity = 2.0f;
            float gain = 0.5f;
            int octaves = 6;
            float frequency = 2.0f;
            float amplitude = 1.0f;
    
            float pz = amplitude * stb_perlin_fbm_noise3(px * frequency, py * frequency, 0.0f, lacunarity, gain, octaves);

            Vector3 position = Vector3(px, py, pz);
            Vector2 texCoord = Vector2(x, y);
            Vector3 color;
            Vector3 normal = Vector3(0, 0, 0); // Placeholder normal

            if (pz < 0.1f)
                color = Vector3(1.5f, 0.2f, 3.2f);
            else if (pz < 0.2f)
                color = Vector3(0.2f, 1.5f, 0.2f);
            else if (pz < 0.3f)
                color = Vector3(0.2f, 0.2f, 1.5f);
            else
                color = Vector3(1.5f, 1.5f, 1.5f);

            Vertex vertex = Vertex{position, texCoord, color, normal};
            vertices.push_back(vertex);
            
            //positions.push_back(position);
            //texCoords.push_back(texCoord);
            //colors.push_back(color);

            // Using VBO
            // // Position coordinates
            // Vector3 V3_bottomLeft = Vector3(x * scaleX - 0.5f, y * scaleY - 0.5f, 0);
            // Vector3 V3_bottomRight = Vector3((x + 1) * scaleX - 0.5f, y * scaleY - 0.5f, 0);
            // Vector3 V3_topLeft = Vector3(x * scaleX - 0.5f, (y + 1) * scaleY - 0.5f, 0);
            // Vector3 V3_topRight = Vector3((x + 1) * scaleX - 0.5f, (y + 1) * scaleY - 0.5f, 0);

            // positions.push_back(V3_topLeft);
            // positions.push_back(V3_bottomLeft);
            // positions.push_back(V3_topRight);
            // positions.push_back(V3_topRight);
            // positions.push_back(V3_bottomLeft);
            // positions.push_back(V3_bottomRight);

            // // Texture coordinates
            // Vector2 V2_bottomLeft = Vector2(0, 0);
            // Vector2 V2_bottomRight = Vector2(1, 0);
            // Vector2 V2_topLeft = Vector2(0, 1);
            // Vector2 V2_topRight = Vector2(1, 1);

            // texCoords.push_back(V2_topLeft);
            // texCoords.push_back(V2_bottomLeft);
            // texCoords.push_back(V2_topRight);
            // texCoords.push_back(V2_topRight);
            // texCoords.push_back(V2_bottomLeft);
            // texCoords.push_back(V2_bottomRight);

        }
    }

    for (int y = 0; y <= m_gridY; y++)
    {
        for (int x = 0; x <= m_gridX; x++)
        {
            Vertex currentVertex = vertices[y * (m_gridX + 1) + x];
            Vertex leftVertex, rightVertex, upVertex, downVertex;

            leftVertex = (x == 0) ? leftVertex = currentVertex : leftVertex = vertices[y * (m_gridX + 1) + x - 1]; 
            rightVertex = (x == m_gridX) ? rightVertex = currentVertex : rightVertex = vertices[y * (m_gridX + 1) + x + 1];
            downVertex = (y == 0) ? downVertex = currentVertex : downVertex = vertices[(y - 1) * (m_gridX + 1) + x];
            upVertex = (y == m_gridY) ? upVertex = currentVertex : upVertex = vertices[(y + 1) * (m_gridX + 1) + x];

            float deltaX = (rightVertex.position.z - leftVertex.position.z) / (rightVertex.position.x - leftVertex.position.x);
            float deltaY = (upVertex.position.z - downVertex.position.z) / (upVertex.position.y - downVertex.position.y);
            
            Vector3 normalVec = Vector3(deltaX, deltaY, 1).Normalize();
            currentVertex.normal = normalVec;
            vertices[y * (m_gridX + 1) + x] = currentVertex;
        }
    }




    for (int y = 0; y < m_gridY; y++)
    {
        for (int x = 0; x < m_gridX; x++)
        {
            uint32_t bl = (uint32_t)( y      * (m_gridX + 1) + x     );
            uint32_t br = (uint32_t)( y      * (m_gridX + 1) + (x+1) );
            uint32_t tl = (uint32_t)( (y+1)  * (m_gridX + 1) + x     );
            uint32_t tr = (uint32_t)( (y+1)  * (m_gridX + 1) + (x+1) );

            indices.push_back(tl);
            indices.push_back(bl);
            indices.push_back(tr);

            indices.push_back(tr);
            indices.push_back(bl);
            indices.push_back(br);
        }
    }

    //const size_t posSize = positions.size() * sizeof(Vector3);
    //const size_t texSize  = texCoords.size() * sizeof(Vector2);
    //const size_t colSize = colors.size() * sizeof(Vector3);
    const size_t verSize = vertices.size() * sizeof(Vertex);

    VAO.Bind();
    VBO.Bind();

    //VBO.AllocateData(posSize + texSize + colSize, BufferObject::Usage::StaticDraw);
    
    //VBO.UpdateData<Vector3>(positions, 0);
    //VBO.UpdateData<Vector2>(texCoords, posSize);
    //VBO.UpdateData<Vector3>(colors, posSize + texSize);
    
    VBO.AllocateData(std::as_bytes(std::span(vertices)), BufferObject::Usage::StaticDraw);

    VertexAttribute position(Data::Type::Float, 3);
    VertexAttribute texCoord(Data::Type::Float, 2);
    VertexAttribute color(Data::Type::Float, 3);
    VertexAttribute normal(Data::Type::Float, 3);

    size_t vertexSize = sizeof(Vertex);
    size_t positionOffset = 0;
    size_t texCoordOffset = positionOffset + position.GetSize();
    size_t colorOffset = texCoordOffset + texCoord.GetSize();
    size_t normalOffset = colorOffset + color.GetSize();

    VAO.SetAttribute(0, position, positionOffset, vertexSize);
    VAO.SetAttribute(1, texCoord, texCoordOffset, vertexSize);
    VAO.SetAttribute(2, color, colorOffset, vertexSize);
    VAO.SetAttribute(3, normal, normalOffset, vertexSize);

    EBO.Bind();
    EBO.AllocateData(std::as_bytes(std::span(indices)), BufferObject::Usage::StaticDraw);

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();   

}

void TerrainApplication::Update()
{
    Application::Update();

    UpdateOutputMode();
}

void TerrainApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Set shader to be used
    glUseProgram(m_shaderProgram);

    VAO.Bind();
    //glDrawArrays(GL_TRIANGLES, 0, 6 * m_gridX * m_gridY);
    glDrawElements(GL_TRIANGLES, 2 * 3 * m_gridX * m_gridY, GL_UNSIGNED_INT, 0);

}

void TerrainApplication::Cleanup()
{
    Application::Cleanup();
}


void TerrainApplication::BuildShaders()
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "layout (location = 2) in vec3 aColor;\n"
        "layout (location = 3) in vec3 aNormal;\n"
        "uniform mat4 Matrix = mat4(1);\n"
        "out vec2 texCoord;\n"
        "out vec3 color;\n"
        "out vec3 normal;\n"
        "void main()\n"
        "{\n"
        "   texCoord = aTexCoord;\n"
        "   color = aColor;\n"
        "   normal = aNormal;\n"
        "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "uniform uint Mode = 0u;\n"
        "in vec2 texCoord;\n"
        "in vec3 color;\n"
        "in vec3 normal;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   switch (Mode)\n"
        "   {\n"
        "   default:\n"
        "   case 0u:\n"
        "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "       break;\n"
        "   case 1u:\n"
        "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
        "       break;\n"
        "   case 2u:\n"
        "       FragColor = vec4(color, 1.0f);\n"
        "       break;\n"
        "   case 3u:\n"
        "       FragColor = vec4(normalize(normal), 1.0f);\n"
        "       break;\n"
        "   case 4u:\n"
        "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
        "       break;\n"
        "   }\n"
        "}\n\0";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    m_shaderProgram = shaderProgram;
}

void TerrainApplication::UpdateOutputMode()
{
    for (int i = 0; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }
}