//
// Created by egors on 09.12.2025.
//

#ifndef DUMPENGINE_DRENDERER_H
#define DUMPENGINE_DRENDERER_H
#include <vector>
#include <glm/fwd.hpp>

#include "../Objects/DBaseLight.h"
#include "../Objects/DCamera.h"


struct RenderCommand;

class DRenderer {
public:
    DRenderer() = default;


public:
    void Init();
    static void BeginScene(const glm::mat4& viewProjMatrix);
    static void Submit(const std::vector<RenderCommand>& commands, DBaseLight* LightComponent, DCamera* camera );
    static void RenderDebugQuad();
    void PrepareSkyBox();
    static unsigned int CompileShaderProgramFromSource(const char* vertexSource, const char* fragmentSource);

private:
    static unsigned int gBufferProgramID;
    static unsigned int debugQuadProgramID;

    static unsigned int shadowMapTexture;
    static unsigned int GBuffer;

    static unsigned int gNormalTextureID;
    static unsigned int gAlbedoRoughnessTextureID;
    static unsigned int gMetallicAOTextureID;
    static unsigned int depthTextureID;
    static unsigned int forwardProgramID;

    int SCREEN_WIDTH = 1280;
    int SCREEN_HEIGHT = 720;

    static unsigned int SkyBoxVAO;
    static unsigned int SkyBoxVBO;

    static unsigned int cubemapTexture;

    static unsigned int quadVAO;

    const float skyboxVertices[108] = {
        // координаты
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
};


#endif //DUMPENGINE_DRENDERER_H