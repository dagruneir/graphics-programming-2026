#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include <iostream>

int buildShaderProgram();
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    DeviceGL deviceGL;

    // glfw window creation
    // --------------------
    Window window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL");
    if (!window.IsValid())
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    deviceGL.SetCurrentWindow(window);
    if (!deviceGL.IsReady())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    int shaderProgram = buildShaderProgram();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // -0.5f, -0.5f, 0.0f, // bottom left  
        //  0.5f, -0.5f, 0.0f, // bottom right 
        //  0.5f,  0.5f, 0.0f,  // top right
        // -0.5f, 0.5f, 0.0f, // top left  


        //creating a circle
        0.0f,  0.0f, 0.0f, // center point
    };

    unsigned int indices[] = {
        0, 1, 2,   // first triangle
        2, 0, 3    // second triangle
    };

    VertexBufferObject vbo;
    VertexArrayObject vao;
    ElementBufferObject ebo;

    //float time = 0.0f;
    //const float rotationSpeed = 1.0f;
    //const float length = std::sqrt(2.0f) / 2.0f;

    vao.Bind();
    vbo.Bind();
    ebo.Bind();

    ebo.AllocateData<unsigned int>(indices);
    vbo.AllocateData<float>(vertices);

    VertexAttribute position = VertexAttribute(Data::Type::Float, 3);
    vao.SetAttribute(0, position, 0);

    vbo.Unbind();
    vao.Unbind();
    ebo.Unbind();


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!window.ShouldClose())
    {
        // input
        // -----
        processInput(window.GetInternalWindow());

        // render
        // ------
        deviceGL.Clear(0.2f, 0.3f, 0.3f, 1.0f);

        // draw our first triangle
        glUseProgram(shaderProgram);
        vao.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        /*         time += 0.01f;
        float angle = time * rotationSpeed;

        for (int i = 0; i < 4; ++i)
        {
            vertices[i * 3 + 0] = std::sin(angle + i * M_PI / 2.0f)*length;
            vertices[i * 3 + 1] = std::cos(angle + i * M_PI / 2.0f)*length;
        }

        vbo.Bind();
        vbo.UpdateData<float>(vertices);
        vbo.Unbind(); */

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        window.SwapBuffers();
        deviceGL.PollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // This is now done in the destructor of DeviceGL
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// build the shader program
// ------------------------
int buildShaderProgram() 
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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
    return shaderProgram;
}