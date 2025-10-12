#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include "Libs/stb_image.h"

const GLint WIDTH = 800, HEIGHT = 600;

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader> shaderList;

// Mesh* light;
// static const char *lightVShader = "Shaders/lightShader.vert";
// static const char *lightFShader = "Shaders/lightShader.frag";

// Vertex Shader
static const char *vShader = "Shaders/shader.vert";
// Fragment Shader
static const char *fShader = "Shaders/shader.frag";


void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);

    // Shader *shader2 = new Shader();
    // shader2->CreateFromFiles(lightVShader, lightFShader);
    // shaderList.push_back(*shader2);
}

void CreateTriangle()
{
    GLfloat vertices[] =
        {
            // pos                  //TexCoord
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            0.0, -1.0f, 1.0f, 0.5f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.5f, 1.0f};

    unsigned int indices[] =
        {
            0,
            3,
            1,
            1,
            3,
            2,
            2,
            3,
            0,
            0,
            1,
            2,
        };

    Mesh *obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 20, 12);
    for (int i = 0; i < 10; i++)
    {
        meshList.push_back(obj1);
    }
}

void CreateOBJ()
{
    Mesh *obj2 = new Mesh();
    bool loaded = obj2->CreateMeshFromOBJ("Models/suzanne.obj");
    if (loaded)
    {
        for (size_t i = 0; i < 10; i++)
        {
            meshList.push_back(obj2);
        }
        
    } else {
        std::cout << "OBJ loaded fail!" << std::endl;
    }
    
    // light = new Mesh();
    // loaded = light->CreateMeshFromOBJ("Models/cube.obj");
    // if (!loaded)
    // {
    //     std::cout << "OBJ loaded fail!" << std::endl;
    // }
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateOBJ();
    CreateShaders();

    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
    // glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f);

    unsigned int texture1;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width1, height1, nrChannels1;

    unsigned char *data1 = stbi_load("Textures/uvmap.png", &width1, &height1, &nrChannels1, 0);

    if (!data1)
    {
        std::cerr << "Failed to load texture: " << stbi_failure_reason() << "\n";
    }
    else
    {
        // Handle 1/3/4 channel images correctly
        GLenum format = GL_RGB;
        if (nrChannels1 == 1)
            format = GL_RED;
        else if (nrChannels1 == 3)
            format = GL_RGB;
        else if (nrChannels1 == 4)
            format = GL_RGBA;
        // Avoid row-alignment issues for non-4-byte widths
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width1, height1, 0, format, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data1);
    }

    glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(-10.0f, 0.0f, 0.0f);

    while (!mainWindow.getShouldClose())
    {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw here
        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetUniformLocation("model");
        uniformView = shaderList[0].GetUniformLocation("view");
        uniformProjection = shaderList[0].GetUniformLocation("projection");

        glm::mat4 view(1.0f);

        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 15.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        glm::vec3 pyramidPosition[] = {
            glm::vec3(0.0f, 0.0f, -2.5f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)};


        lightPos.x = 1.0f + sin(glfwGetTime()) *2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
        lightPos.z = 0.0f; 
        // Object
        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model(1.0f);

            model = glm::translate(model, pyramidPosition[i]);
            model = glm::rotate(model, glm::radians(2.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            
            GLint uniformTexture1 = shaderList[0].GetUniformLocation("texture1");
            glUniform1i(uniformTexture1, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            
            glUniform3fv(shaderList[0].GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);
            glUniform3fv(shaderList[0].GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);
            glUniform3fv(shaderList[0].GetUniformLocation("viewPos"), 1, (GLfloat *)&cameraPos);

            meshList[i]->RenderMesh();
        }


        // // Light
        // shaderList[1].UseShader();
        // uniformModel = shaderList[1].GetUniformLocation("model");
        // uniformView = shaderList[1].GetUniformLocation("view");
        // uniformProjection = shaderList[1].GetUniformLocation("projection");

        // glm::mat4 model(1.0f);

        // model = glm::translate(model, lightPos);
        // model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        // glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        // glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        // glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        // glUniform3fv(shaderList[1].GetUniformLocation("lightColour"), 1, glm::value_ptr(lightColour));
        // light->RenderMesh();

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}