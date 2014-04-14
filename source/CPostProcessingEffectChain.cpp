// This file is part of the "irrRenderer".
// For conditions of distribution and use, see copyright notice in irrRenderer.h

#include "CPostProcessingEffectChain.h"

irr::video::CPostProcessingEffectChain::CPostProcessingEffectChain(irr::IrrlichtDevice* device, irr::io::path shaderDir)
    :Device(device),
    ShaderDir(shaderDir),
    Active(false),
    ActiveEffectCount(0),
    OriginalRender(0)
{

}

irr::video::CPostProcessingEffectChain::~CPostProcessingEffectChain()
{
    if(OriginalRender)
    {
        Device->getVideoDriver()->removeTexture(OriginalRender);
        OriginalRender = 0;
    }
}

irr::u32 irr::video::CPostProcessingEffectChain::attachEffect(irr::video::CPostProcessingEffect* effect)
{
    if (effect->getChain())
        effect->getChain()->detachEffect(effect);

    effect->setChain(this);
    Effects.push_back(effect);

    if (effect->isActive())
    {
        Active = true;
        ActiveEffectCount++;
    }

    return getEffectIndex(effect);
}

irr::video::CPostProcessingEffect* irr::video::CPostProcessingEffectChain::createEffect(irr::core::stringc sourceFrag, irr::video::IShaderConstantSetCallBack* callback)
{
    //Device->getLogger()->log("compile shader", shader.Name.c_str(), irr::ELL_INFORMATION);

    CPostProcessingEffect* effect = new CPostProcessingEffect(Device, sourceFrag, readShader("quad.vert"), callback);
    attachEffect(effect);

    return effect;
}

irr::video::CPostProcessingEffect* irr::video::CPostProcessingEffectChain::createEffect(irr::video::E_POSTPROCESSING_EFFECT type)
{
    irr::core::stringc shaderSource;

    switch(type)
    {
        case EPE_I_SEE_RED:
            shaderSource = readShader("iseered.frag");
            break;

        case EPE_ALBEDO:
            shaderSource = readShader("albedo.frag");
            break;

        case EPE_FXAA:
            shaderSource = readShader("fxaa.frag");
            break;

        case EPE_BLOOM_PREPASS:
            shaderSource = readShader("bloom_prepass.frag");
            break;

        case EPE_BLUR_V:
            shaderSource = readShader("blur_v.frag");
            break;

        case EPE_BLUR_H:
            shaderSource = readShader("blur_h.frag");
            break;

        case EPE_BLUR_H_ADD:
            shaderSource = readShader("blur_h_add.frag");
            break;

        case EPE_COLD_COLORS:
            shaderSource = readShader("coldcolors.frag");
            break;

        case EPE_WARM_COLORS:
            shaderSource = readShader("warmcolors.frag");
            break;

        default: return 0; //this should never happen
    }

    return createEffect(shaderSource);
}


void irr::video::CPostProcessingEffectChain::detachEffect(irr::u32 index)
{
    Effects[index]->setChain(0);
    Effects.erase(index);

    ActiveEffectCount--;
    if(getActiveEffectCount() == 0)
    {
        Active = false;
    }
}

void irr::video::CPostProcessingEffectChain::detachEffect(irr::video::CPostProcessingEffect* effect)
{
    detachEffect(getEffectIndex(effect));
}


void irr::video::CPostProcessingEffectChain::removeEffect(irr::u32 index)
{
    removeEffect(getEffectFromIndex(index));
}

void irr::video::CPostProcessingEffectChain::removeEffect(irr::video::CPostProcessingEffect* effect)
{
    detachEffect(effect);
    delete effect;
}


void irr::video::CPostProcessingEffectChain::setKeepOriginalRender(bool keep)
{
    irr::video::IVideoDriver* video = Device->getVideoDriver();

    if (keep && !getKeepOriginalRender())
    {
        irr::core::stringc originalName = "irrPP-Chain-";
        originalName += *(int*)this;
        OriginalRender = video->addRenderTargetTexture(video->getCurrentRenderTargetSize(), originalName.c_str());
    }
    else if (!keep && getKeepOriginalRender())
    {
        video->removeTexture(OriginalRender);
        OriginalRender = 0;
    }
}

bool irr::video::CPostProcessingEffectChain::getKeepOriginalRender() const
{
    return (OriginalRender != 0);
}

irr::video::ITexture* irr::video::CPostProcessingEffectChain::getOriginalRender()
{
    return OriginalRender;
}

void irr::video::CPostProcessingEffectChain::requestActiveEffectUpdate()
{
    ActiveEffectCount = 0;

    if (!isActive())
        return;

    for(irr::u32 i = 0; i < Effects.size(); i++)
    {
        if(Effects[i]->isActive()) ActiveEffectCount++;
    }
}

irr::u32 irr::video::CPostProcessingEffectChain::getEffectCount() const
{
    return Effects.size();
}

irr::u32 irr::video::CPostProcessingEffectChain::getActiveEffectCount() const
{
    return ActiveEffectCount;
}


irr::video::CPostProcessingEffect* irr::video::CPostProcessingEffectChain::getEffectFromIndex(irr::u32 index)
{
    return Effects[index];
}


irr::u32 irr::video::CPostProcessingEffectChain::getEffectIndex(irr::video::CPostProcessingEffect* effect) const
{
    for(irr::u32 i= 0; i < Effects.size(); i++)
    {
        if(Effects[i] == effect) return i;
    }
    return 0;
}


void irr::video::CPostProcessingEffectChain::setActive(bool active)
{
    Active = active;
    requestActiveEffectUpdate();
}

bool irr::video::CPostProcessingEffectChain::isActive()
{
    return Active;
}


irr::core::stringc irr::video::CPostProcessingEffectChain::readShader(irr::io::path filename) const
{
    irr::io::path path;
    path = ShaderDir;
    path += filename;

    irr::io::IReadFile* file = Device->getFileSystem()->createAndOpenFile(path);
    irr::u32 size = file->getSize();
    irr::c8 *buff = new irr::c8 [size + 1];
    file->read(buff, file->getSize());
    buff[size] = '\0';

    return irr::core::stringc(buff);
}
