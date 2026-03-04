// Renderer.cpp
#include "DRenderer.h"

#include "DRenderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>             // Чтобы компилятор понял, что такое mat4
#include <glm/gtc/type_ptr.hpp>    // (Опционально) для удобной работы с указателями
#include "../Core/Types.h"
#include <iostream>
#include <stb_image.h>

#include "../Core/DBaseMesh.h"
#include "../Objects/DBaseLight.h"
#include "../Objects/DCamera.h"
#include "../Systems/ModelLoader.h"

// Инициализируем статическую переменную

unsigned int DRenderer::SkyBoxVAO = 0;
unsigned int DRenderer::SkyBoxVBO = 0;
unsigned int DRenderer::quadVAO = 0;
unsigned int DRenderer::shadowMapTexture = 0;
unsigned int DRenderer::cubemapTexture = 0;
unsigned int DRenderer::gBufferProgramID = 0;
unsigned int DRenderer::GBuffer = 0;
unsigned int DRenderer::depthTextureID = 0;
unsigned int DRenderer::gAlbedoRoughnessTextureID = 0;
unsigned int DRenderer::gMetallicAOTextureID = 0;
unsigned int DRenderer::gNormalTextureID = 0;
unsigned int DRenderer::debugQuadProgramID = 0;
unsigned int DRenderer::forwardProgramID = 0;


// --- ВРЕМЕННЫЙ FORWARD ШЕЙДЕР ---
const char* ForwardVertexSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoords;

    out vec2 TexCoords;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        TexCoords = aTexCoords;
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* ForwardFragmentSrc = R"(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoords;
    uniform sampler2D texture_albedo;

    void main()
    {
        // Просто выводим текстуру.
        // Если текстуры нет, объект будет черным (или прозрачным).
        vec4 texColor = texture(texture_albedo, TexCoords);

        // ОТЛАДКА: Если альфа 0 или текстура не загрузилась, подмешиваем красный, чтобы видеть силуэт
        // FragColor = vec4(texColor.rgb + vec3(0.2, 0.0, 0.0), 1.0);

        // Обычный вывод:
        FragColor = texColor;
    }
)";


// --- Шейдеры прямо в строках ---
// Вершинный шейдер для Geometry Pass
const char* GBuffervertexSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoords;
    layout (location = 3) in vec3 aTangent;
    layout (location = 4) in vec3 aBitangent;

    out vec2 TexCoords;
    out vec3 FragPos;
    out mat3 TBN; // Матрица для преобразования нормалей из tangent-space в world-space

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        // Передаем текстурные координаты без изменений
        TexCoords = aTexCoords;

        // Вычисляем позицию фрагмента в мировом пространстве
        FragPos = vec3(model * vec4(aPos, 1.0));

        // Создаем TBN матрицу для корректного normal mapping
        vec3 T = normalize(mat3(model) * aTangent);
        vec3 B = normalize(mat3(model) * aBitangent);
        vec3 N = normalize(mat3(model) * aNormal);
        TBN = mat3(T, B, N);

        // Финальная позиция вершины для OpenGL
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

// Фрагментный шейдер для Geometry Pass
const char* GBufferfragmentSrc = R"(
    #version 330 core

    // ВЫХОДНЫЕ ПЕРЕМЕННЫЕ
    layout (location = 0) out vec3 gNormal;
    layout (location = 1) out vec4 gAlbedoRough;
    layout (location = 2) out vec3 gMetallicAOEmissive;

    // ВХОДНЫЕ ПЕРЕМЕННЫЕ
    in vec2 TexCoords;
    in vec3 FragPos;
    in mat3 TBN;

    uniform sampler2D texture_albedo;
    uniform sampler2D texture_normal;
    uniform sampler2D texture_metallic;
    uniform sampler2D texture_roughness;
    uniform sampler2D texture_ao;

    void main()
    {
        // 1. АЛЬБЕДО (ЦВЕТ)
        vec4 albedoSample = texture(texture_albedo, TexCoords);

        // ЗАЩИТА ОТ ЧЕРНОГО ЭКРАНА:
        // Если альфа текстуры < 0.1 (прозрачная) или цвет полностью черный — делаем объект темно-красным.
        if(albedoSample.a < 0.1 || length(albedoSample.rgb) < 0.01) {
             gAlbedoRough.rgb = vec3(0.3, 0.0, 0.0); // Темно-красный (резерв)
        } else {
             gAlbedoRough.rgb = albedoSample.rgb;    // Нормальная текстура
        }

        // Записываем шероховатость в альфа-канал
        gAlbedoRough.a = texture(texture_roughness, TexCoords).r;

        // 2. НОРМАЛИ
        vec3 normal = texture(texture_normal, TexCoords).rgb;
        if (length(normal) == 0.0) {
            // Если карты нормалей нет, используем геометрическую нормаль (смотрит вверх Z в Tangent Space)
            gNormal = normalize(TBN * vec3(0.0, 0.0, 1.0));
        } else {
            normal = normal * 2.0 - 1.0;
            gNormal = normalize(TBN * normal);
        }

        // 3. PBR ПАРАМЕТРЫ
        gMetallicAOEmissive.r = texture(texture_metallic, TexCoords).r;
        gMetallicAOEmissive.g = texture(texture_ao, TexCoords).r;
        gMetallicAOEmissive.b = 0.0; // Emissive пока 0
    }
)";

const char* debugQuadVertexSrc = R"(
    #version 330 core
    // Координаты вершин для полноэкранного треугольника
    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );
    // Текстурные координаты для него
    vec2 texCoords[3] = vec2[](
        vec2(0.0, 0.0),
        vec2(2.0, 0.0),
        vec2(0.0, 2.0)
    );

    out vec2 TexCoords;

    void main()
    {
        // Берем вершину по ее встроенному индексу
        gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
        TexCoords = texCoords[gl_VertexID];
    }
)";

const char* debugQuadFragmentSrc = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;

    uniform sampler2D screenTexture;

    void main()
    {
        vec4 texColor = texture(screenTexture, TexCoords);
        // Принудительно ставим Альфу = 1.0.
        // Если в G-буфере (в альфа канале) записана шероховатость 0.0,
        // мы все равно увидим цвет.
        FragColor = vec4(texColor.rgb, 1.0);
    }
)";

const char* SkyBoxVertexSrc = R"(

)";

const char* SkyBoxFrag = R"(

)";


const char* ZBufferVertexSrc = R"(


)";
const char* ZBufferFragSrc = R"(
    #version 330 core

    void main() {

    }
)";

void DRenderer::Init() {
    // 1. Создаем и привязываем FBO
    glGenFramebuffers(1, &GBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, GBuffer);

    // --- Текстура 1: Нормали (World Space) ---
    // Вывод шейдера: layout (location = 0) out vec3 gNormal;
    glGenTextures(1, &gNormalTextureID);
    glBindTexture(GL_TEXTURE_2D, gNormalTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormalTextureID, 0);


    glGenTextures(1, &gAlbedoRoughnessTextureID);
    glBindTexture(GL_TEXTURE_2D, gAlbedoRoughnessTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedoRoughnessTextureID, 0);


    glGenTextures(1, &gMetallicAOTextureID);
    glBindTexture(GL_TEXTURE_2D, gMetallicAOTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gMetallicAOTextureID, 0);


    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);


    glGenTextures(1, &depthTextureID);
    glBindTexture(GL_TEXTURE_2D, depthTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureID, 0);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    gBufferProgramID = CompileShaderProgramFromSource(GBuffervertexSrc, GBufferfragmentSrc);
    if (gBufferProgramID == 0) {
        std::cout << "G-Buffer shader compilation failed!" << std::endl;
    }
    debugQuadProgramID = CompileShaderProgramFromSource(debugQuadVertexSrc, debugQuadFragmentSrc);
    if (debugQuadProgramID == 0) {
        std::cout << "debugQuadProgramID shader compilation failed!" << std::endl;
    }

    forwardProgramID = CompileShaderProgramFromSource(ForwardVertexSrc, ForwardFragmentSrc);
    if (forwardProgramID == 0) {
        std::cout << "Forward shader compilation failed!" << std::endl;
    }
}

void DRenderer::BeginScene(const glm::mat4& viewProjMatrix) {
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void DRenderer::Submit(const std::vector<RenderCommand>& commands, DBaseLight* light , DCamera* camera ) {
    glUseProgram(gBufferProgramID);
    glBindFramebuffer(GL_FRAMEBUFFER, GBuffer);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    GLint modelLoc = glGetUniformLocation(gBufferProgramID, "model");
    GLint viewLoc = glGetUniformLocation(gBufferProgramID, "view");
    GLint projectionLoc = glGetUniformLocation(gBufferProgramID, "projection");
    GLint albedoMapLoc = glGetUniformLocation(gBufferProgramID, "texture_albedo");
    GLint normalMapLoc = glGetUniformLocation(gBufferProgramID, "texture_normal");
    GLint metallicMapLoc = glGetUniformLocation(gBufferProgramID, "texture_metallic");
    GLint roughnessMapLoc = glGetUniformLocation(gBufferProgramID, "texture_roughness");
    GLint aoMapLoc = glGetUniformLocation(gBufferProgramID, "texture_ao");

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));


    for (const auto& command : commands) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(command.modelMatrix));


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, command.material->albedoMap);
        glUniform1i(albedoMapLoc, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, command.material->normalMap);
        glUniform1i(normalMapLoc, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, command.material->metallicMap);
        glUniform1i(metallicMapLoc, 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, command.material->roughnessMap);
        glUniform1i(roughnessMapLoc, 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, command.material->aoMap);
        glUniform1i(aoMapLoc, 4);

        glBindVertexArray(command.mesh->GetVAO());
        glDrawElements(GL_TRIANGLES, command.mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    }


    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void DRenderer::RenderDebugQuad() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(debugQuadProgramID);


    glDisable(GL_DEPTH_TEST);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gAlbedoRoughnessTextureID);


    GLint texLoc = glGetUniformLocation(debugQuadProgramID, "screenTexture");
    if (texLoc != -1) {
        glUniform1i(texLoc, 0);
    }

    if (quadVAO == 0) {
        glGenVertexArrays(1, &quadVAO);
    }
    glBindVertexArray(quadVAO);


    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void DRenderer::PrepareSkyBox() {

    float vertices[] = {
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

    glGenVertexArrays(1, &SkyBoxVAO);
    glGenBuffers(1, &SkyBoxVBO);

    glBindVertexArray(SkyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, SkyBoxVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

unsigned int DRenderer::CompileShaderProgramFromSource(const char *vertexSource, const char *fragmentSource) {
        unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }


    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }


    unsigned int shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);


    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgramID;
}
