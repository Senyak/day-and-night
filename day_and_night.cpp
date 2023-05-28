#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "assist/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assist/shader.h"
#include "assist/camera.h"
#include "assist/model.h"

#include <iostream>

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

float mixValue = 0.4f;

Camera camera(glm::vec3(1.0f, 1.0f, 5.0f));

float deltaTime = 0.0f;	// разница между текущим и последним кадром
float lastFrame = 0.0f; // время на отрисовку последнего кадра

bool firstMouse = true;
float lastX = (float)SCR_WIDTH / 2.0f;
float lastY = (float)SCR_HEIGHT / 2.0f;

float lightX;
float lightY;
float lightZ;


glm::vec3 lightPos(0.0f, -1.0f, 2.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);


unsigned int grassVAO;
void renderGrass(const Shader& shader);



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif 

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "okno", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);   // МЫШКА
    glfwSetScrollCallback(window, scroll_callback);   //МЫШКА
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  //МЫШКА


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    //-----------------------работаем-----------------------

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //создание шейдеров
    Shader objectShader("shaders_files/shader.vs", "shaders_files/shader.fs");
    Shader modelShader("shaders_files/model.vs", "shaders_files/model.fs");
    Shader shadowdepthShader("shaders_files/shadowdepth.vs", "shaders_files/shadowdepth.fs"); 
    Shader skyboxShader("shaders_files/skybox.vs", "shaders_files/skybox.fs");
    
    //загрузка моделей
    stbi_set_flip_vertically_on_load(false);
    Model houseModel("assets/models/house/HouseSuburban.obj");
    Model house3Model("assets/models/house2/HouseSuburban.obj");

    stbi_set_flip_vertically_on_load(true);
    Model zhModel("assets/models/zh/zh2.obj");

    // создаем наборы вершин and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  25.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  25.0f, 25.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  25.0f, 25.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 25.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   25.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   25.0f, 25.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   25.0f, 25.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 25.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  25.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  25.0f, 25.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 25.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 25.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  25.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  25.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  25.0f, 25.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 25.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 25.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  25.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 25.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  25.0f, 25.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  25.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  25.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 25.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 25.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  25.0f, 25.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  25.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  25.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 25.0f
    };


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };


    //создаем атрибуты вершин  
    
    // атрибуты вершин для травы
    unsigned int grassVBO;
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);


    //атрибуты вершин для скайбокса
    unsigned int skyVAO, skyVBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    
    // создаем карту глубины
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // создаем текстуру хранящую в себе глубину
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // прикрепляем текстуру в качестве буфера глубины к карте
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // загружаем текстуры для объектов
    stbi_set_flip_vertically_on_load(true);
    unsigned int grassTexture = loadTexture("assets/textures/grass.jpg");
   
    vector<std::string> faces
    {
        "assets/skybox2/right.jpg",
        "assets/skybox2/left.jpg",
        "assets/skybox2/top.jpg",
        "assets/skybox2/bottom.jpg",
        "assets/skybox2/front.jpg",
        "assets/skybox2/back.jpg"
    };

    stbi_set_flip_vertically_on_load(false);
    unsigned int skyboxTexture = loadCubemap(faces);
    
    //
    objectShader.use();
    objectShader.setInt("diffuseTexture", 0);
    objectShader.setInt("shadowMap", 1);
    modelShader.use();
    modelShader.setInt("shadowMap", 13);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
    float strengthLight = 0.1f;

    //главный цикл отрисовки
    while (!glfwWindowShouldClose(window))
    {
        // установка необходимых временных переменных
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // обработка ввода
        processInput(window);

        // отображение
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        float y = 4.5f;
        float s = sin(glfwGetTime()/2);
        float lightY = y * 1.5f * abs(s);

        int sign;
        int count = 0;
        bool flag;

        if (s >= 0.0f)
        {
            sign = 1;
            flag = true;
        }
        else
        {
            sign = -1;
            flag = false;
        }
        
        float c = cos(glfwGetTime()/2);
        float x = -4.5f;
        float lightX = x * c *1.5f * sign;

        float lightZ = 1.0f;
        
        if (abs(s) <= 0.1f)
            strengthLight = 0.1f;
        if (abs(s) > 0.1f and abs(s) <= 0.2f)
            strengthLight = 0.25f;
        if (abs(s) > 0.2f and abs(s) <= 0.3f)
            strengthLight = 0.4f;
        if (abs(s) > 0.3f and abs(s) <= 0.4f)
            strengthLight = 0.6f;
        if (abs(s) > 0.4f and abs(s) <= 0.5f)
            strengthLight = 0.7f;
        if (abs(s) > 0.5f and abs(s) <= 0.75f)
            strengthLight = 0.8f;
        if (abs(s) > 0.75f)
            strengthLight = 0.9f;
        

        glm::vec3 lightPos = glm::vec3(lightX, lightY, lightZ);


        // сохраняем глубину сцены с точки зрения источника света
        // --------------------------------------------------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 0.002f, far_plane = 15.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // отображаем сцену с точки зрения источника света
        shadowdepthShader.use();
        shadowdepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        renderGrass(shadowdepthShader);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
        model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));	
        shadowdepthShader.setMat4("model", model);
        houseModel.Draw(shadowdepthShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f,  0.0f, 0.5f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
        shadowdepthShader.setMat4("model", model);
        house3Model.Draw(shadowdepthShader);


        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, -2.5f));
        model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
        shadowdepthShader.setMat4("model", model);
        zhModel.Draw(shadowdepthShader);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // 2. отображем сцену с точки зрения камеры учитывая карту теней
        objectShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);
        // устанавливаем необходимые униформы для источника света
        objectShader.setVec3("viewPos", camera.Position);
        objectShader.setVec3("lightPos", lightPos);
        objectShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        objectShader.setFloat("strengthLight", strengthLight);
        objectShader.setBool("flag", flag);

        glBindTexture(GL_TEXTURE_2D, depthMap);
        glActiveTexture(GL_TEXTURE2);

        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderGrass(objectShader);


        modelShader.use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setVec3("lightPos", lightPos);
        modelShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        modelShader.setFloat("strengthLight", strengthLight);
        modelShader.setBool("flag", flag);

        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
        model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));	
        modelShader.setMat4("model", model);
        houseModel.Draw(modelShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.5f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));
        modelShader.setMat4("model", model);
        house3Model.Draw(modelShader);


        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, -2.5f));
        model = glm::rotate(model, glm::radians(120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
        modelShader.setMat4("model", model);
        zhModel.Draw(modelShader);
     
        // СКАЙБОКС
        // устанавливаем параметры для скайбокса
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();

        model = glm::mat4(1.0f);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("model", model);
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // отображаем скайбокс
        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); 

        // смена буфера обмена
        glfwSwapBuffers(window);
        // обработка событий (нажатие кнопок, перемещение курсора)
        glfwPollEvents();
    }

    // освобожденеие занятых ресурсов
    glDeleteVertexArrays(1, &grassVAO);
    glDeleteBuffers(1, &grassVBO);

    glfwTerminate();

    return 0;
}



// >>>>>>>>>>>>>>>>>>>>>>>>> вспомогательные функции <<<<<<<<<<<<<<<<<<<<<<<<<<<< //


// отображение трехмерной сцены
// --------------------
void renderGrass(const Shader& shader)
{
    shader.setInt("diffuseTexture", 0);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.15f, 0.0f));
    model = glm::scale(model, glm::vec3(25.0f, 0.3f, 25.0f));
    shader.setMat4("model", model);
    glBindVertexArray(grassVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.001f; // можете изменить это значение подходящим вам образом (так как скорость передвижения камеры зависит от системного оборудования).
        if (mixValue >= 1.0f)
            mixValue = 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.001f; // можете изменить это значение подходящим вам образом (так как скорость передвижения камеры зависит от системного оборудования).
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}




void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // меняем направление, поскольку координаты оси y идут снизу вверх

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        if (nrComponents == 4)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}