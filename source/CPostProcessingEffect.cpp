// This file is part of the "irrRenderer".
// For conditions of distribution and use, see copyright notice in irrRenderer.h

#include "CPostProcessingEffect.h"
#include "CPostProcessingEffectChain.h"

irr::video::CPostProcessingEffect::CPostProcessingEffect(irr::IrrlichtDevice* device, irr::core::stringc sourceV, irr::core::stringc sourceF, irr::video::IShaderConstantSetCallBack* callback)
    :Device(device),
    Chain(0),
    Name(""),
    Active(true),
    Callback(callback),
    Quality(irr::video::EPQ_FULL),
    CustomRTT(0)
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

void irr::video::CPostProcessingEffect::setName(irr::core::stringc name)
{
    Name = name;
}

irr::core::stringc irr::video::CPostProcessingEffect::getName() const
{
    return Name;
}


void irr::video::CPostProcessingEffect::setQuality(irr::video::E_POSTPROCESSING_EFFECT_QUALITY quality)
{
    irr::core::dimension2d<irr::u32> fullRes = Device->getVideoDriver()->getCurrentRenderTargetSize();
    irr::core::dimension2d<irr::u32> qRes;

    switch (quality)
    {
    case EPQ_FULL:
        if (CustomRTT)
        {
            Device->getVideoDriver()->removeTexture(CustomRTT);
            CustomRTT = 0;
        }
        Quality = EPQ_FULL;
        return;

    case EPQ_HALF:
        qRes = fullRes / 2;
        break;

    case EPQ_QUARTER:
        qRes = fullRes / 4;
        break;

    case EPQ_OCTOPUS:
        qRes = fullRes / 8;
        break;

    case EPQ_CUSTOM:
        Device->getLogger()->log("irrPP", "Attempted to set quality to 'custom' with an enum", irr::ELL_ERROR);
        Device->getLogger()->log("irrPP", "Set with the custom resolution instead", irr::ELL_ERROR);
        return;
    }

    CustomRTT = Device->getVideoDriver()->addRenderTargetTexture(qRes);
    Quality = quality;
}

void irr::video::CPostProcessingEffect::setQuality(irr::core::dimension2d<irr::u32> resolution)
{
    if (CustomRTT)
    {
        Device->getVideoDriver()->removeTexture(CustomRTT);
        CustomRTT = 0;
    }

    CustomRTT = Device->getVideoDriver()->addRenderTargetTexture(resolution);
    Quality = EPQ_CUSTOM;
}

irr::video::E_POSTPROCESSING_EFFECT_QUALITY irr::video::CPostProcessingEffect::getQuality() const
{
    return Quality;
}

irr::core::dimension2d<irr::u32> irr::video::CPostProcessingEffect::getQualityResolution() const
{
    if (CustomRTT)
        return CustomRTT->getSize();
    else
        return Device->getVideoDriver()->getCurrentRenderTargetSize();
}

irr::video::ITexture* irr::video::CPostProcessingEffect::getCustomRTT() const
{
    return CustomRTT;
}
