// This file is part of the "irrRenderer".
// For conditions of distribution and use, see copyright notice in irrRenderer.h

#include "CPostProcessingEffect.h"
#include "CPostProcessingEffectChain.h"

irr::video::CPostProcessingEffect::CPostProcessingEffect(irr::IrrlichtDevice* device, irr::core::stringc sourceV, irr::core::stringc sourceF, irr::video::IShaderConstantSetCallBack* callback)
    :Device(device),
    Chain(0),
    Active(true),
    Callback(callback)
{
    MaterialType= (irr::video::E_MATERIAL_TYPE) Device->getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterial(
                                                    sourceF.c_str(), "main", irr::video::EVST_VS_2_0,
                                                    sourceV.c_str(), "main", irr::video::EPST_PS_2_0,
                                                    callback);
}

irr::video::CPostProcessingEffect::~CPostProcessingEffect()
{
    if (Chain)
        Chain->removeEffect(this);

    delete Callback;
}

void irr::video::CPostProcessingEffect::setChain(irr::video::CPostProcessingEffectChain* chain)
{
    Chain = chain;
}

irr::video::CPostProcessingEffectChain* irr::video::CPostProcessingEffect::getChain() const
{
    return Chain;
}


void irr::video::CPostProcessingEffect::remove()
{
    delete this;
}


void irr::video::CPostProcessingEffect::addTextureToShader(irr::video::ITexture* tex)
{
    TexturesToPass.push_back(tex);
}

irr::video::ITexture* irr::video::CPostProcessingEffect::getTextureToPass(irr::u32 index) const
{
    return TexturesToPass[index];
}

irr::u32 irr::video::CPostProcessingEffect::getTextureToPassCount() const
{
    return TexturesToPass.size();
}

const irr::video::E_MATERIAL_TYPE& irr::video::CPostProcessingEffect::getMaterialType() const
{
    return MaterialType;
}

irr::video::IShaderConstantSetCallBack* irr::video::CPostProcessingEffect::getCallback() const
{
    return Callback;
}

void irr::video::CPostProcessingEffect::setActive(bool active)
{
    Active = active;
    Chain->requestActiveEffectUpdate();
}

bool irr::video::CPostProcessingEffect::isActive() const
{
    return Active;
}


