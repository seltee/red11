// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/primitives.h"
#include "utils/utils.h"
#include "data/usable.h"
#include "data/shader.h"
#include "settings.h"
#include <list>

#define MAX_MATERIAL_COUNT 10000

enum class MaterialType
{
    Simple,
    Complex
};

enum class MaterialDisplay
{
    Solid,
    SolidMask,
    Alpha
};

class Material : public Usable
{
public:
    EXPORT Material();
    EXPORT virtual ~Material();

    inline void destroy() { delete this; }

    virtual MaterialType getType() = 0;
    virtual MaterialDisplay getDisplay() = 0;
    virtual bool isUsingNormalMap() = 0;
    virtual bool isAlphaPhase() = 0;

    static inline std::vector<Material *> *getMaterialsList() { return &materials; }

    inline unsigned int getIndex() { return index; }
    inline unsigned int getUpdateIndex() { return updIndex; }

    void unload() override;

    inline void setShaderDepth(Shader *depthShader) { this->depthShader[(int)depthShader->getType()] = depthShader; }
    inline Shader *getShaderDepth(RendererType renderType) { return depthShader[(int)renderType]; }
    inline void setShaderDepthSkinned(Shader *depthSkinnedShader) { this->depthSkinnedShader[(int)depthSkinnedShader->getType()] = depthSkinnedShader; }
    inline Shader *getShaderDepthSkinned(RendererType renderType) { return depthSkinnedShader[(int)renderType]; }
    inline void setShaderColor(Shader *colorShader) { this->colorShader[(int)colorShader->getType()] = colorShader; }
    inline Shader *getShaderColor(RendererType renderType) { return colorShader[(int)renderType]; }
    inline void setShaderColorSkinned(Shader *colorSkinnedShader) { this->colorSkinnedShader[(int)colorSkinnedShader->getType()] = colorSkinnedShader; }
    inline Shader *getShaderColorSkinned(RendererType renderType) { return colorSkinnedShader[(int)renderType]; }
    inline void setShaderShadow(Shader *shadowShader) { this->shadowShader[(int)shadowShader->getType()] = shadowShader; }
    inline Shader *getShaderShadow(RendererType renderType) { return shadowShader[(int)renderType]; }
    inline void setShaderShadowSkinned(Shader *shadowSkinnedShader) { this->shadowSkinnedShader[(int)shadowSkinnedShader->getType()] = shadowSkinnedShader; }
    inline Shader *getShaderShadowSkinned(RendererType renderType) { return shadowSkinnedShader[(int)renderType]; }

protected:
    unsigned int getNextIndex();

    unsigned int index;
    unsigned int updIndex;

    static std::vector<Material *> materials;

    // Index pool
    static bool indexPool[MAX_ELEMENT_INDEX];
    static unsigned int nextIndex;

    Shader *depthShader[4] = {nullptr, nullptr, nullptr, nullptr};
    Shader *depthSkinnedShader[4] = {nullptr, nullptr, nullptr, nullptr};
    Shader *colorShader[4] = {nullptr, nullptr, nullptr, nullptr};
    Shader *colorSkinnedShader[4] = {nullptr, nullptr, nullptr, nullptr};
    Shader *shadowShader[4] = {nullptr, nullptr, nullptr, nullptr};
    Shader *shadowSkinnedShader[4] = {nullptr, nullptr, nullptr, nullptr};
};
