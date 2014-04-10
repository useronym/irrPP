#include "irrPP.h"

irr::video::irrPP* createIrrPP(irr::IrrlichtDevice* device, const irr::io::path shaderDir)
{
    return new irr::video::irrPP(device, shaderDir);
}



irr::video::irrPP::irrPP(irr::IrrlichtDevice* device, const irr::io::path shaderDir)
    :Device(device),
    ShaderDir(shaderDir)
{
    Quad = new irr::scene::IQuadSceneNode(0, Device->getSceneManager());
    Chains.push_back(createEffectChain());

    RTT1 = Device->getVideoDriver()->addRenderTargetTexture(Device->getVideoDriver()->getCurrentRenderTargetSize(), "irrPP-RTT1");
    RTT2 = Device->getVideoDriver()->addRenderTargetTexture(Device->getVideoDriver()->getCurrentRenderTargetSize(), "irrPP-RTT2");
}

irr::video::irrPP::~irrPP()
{
    //dtor
}

void irr::video::irrPP::render(irr::video::ITexture* texture)
{
    irr::u32 effectsRendered = 0;
    irr::u32 numActiveEffects = getActiveEffectCount();
    irr::video::ITexture* usedRTT = texture;

    irr::u32 numChains = Chains.size();

    for (irr::u32 chain = 0; chain < numChains; chain++)
    {
        irr::video::CPostProcessingEffectChain* thisChain = Chains[chain];

        if (thisChain->isActive())
        {
            irr::u32 numEffects = Chains[chain]->getEffectCount();

            for (irr::u32 effect = 0; effect < numEffects; effect++)
            {
                irr::video::CPostProcessingEffect* thisEffect = thisChain->getEffectFromIndex(effect);

                if (thisEffect->isActive())
                {
                    // decide render targets
                    if (effectsRendered + 1 == numActiveEffects)
                        Device->getVideoDriver()->setRenderTarget(0);
                    else
                    {
                        Device->getVideoDriver()->setRenderTarget(usedRTT == RTT1 ? RTT2 : RTT1);
                    }

                    Quad->setMaterialType(thisEffect->getMaterialType());
                    Quad->setMaterialTexture(0, usedRTT);
                    Quad->render();

                    usedRTT == RTT1 ? usedRTT = RTT2 : usedRTT = RTT1;

                    effectsRendered++;
                }
            }
        }
    }
}

irr::video::CPostProcessingEffectChain* irr::video::irrPP::createEffectChain()
{
    irr::video::CPostProcessingEffectChain* chain = new irr::video::CPostProcessingEffectChain(Device, ShaderDir);
    return chain;
}

irr::video::CPostProcessingEffect* irr::video::irrPP::createEffect(irr::core::stringc sourceFrag, irr::video::IShaderConstantSetCallBack* callback)
{
    return getRootPostProcessingEffectChain()->createEffect(sourceFrag, callback);
}

irr::video::CPostProcessingEffect* irr::video::irrPP::createEffect(irr::video::E_POSTPROCESSING_EFFECT type)
{
    return getRootPostProcessingEffectChain()->createEffect(type);
}

irr::video::ITexture* irr::video::irrPP::getRTT1() const
{
    return RTT1;
}

irr::video::ITexture* irr::video::irrPP::getRTT2() const
{
    return RTT2;
}

irr::u32 irr::video::irrPP::getActiveEffectCount() const
{
    irr::u32 numChains = Chains.size();
    irr::u32 count = 0;

    for (irr::u32 chain = 0; chain < numChains; chain++)
    {
        count += Chains[chain]->getActiveEffectCount();
    }

    return count;
}

irr::video::CPostProcessingEffectChain* irr::video::irrPP::getRootPostProcessingEffectChain() const
{
    return Chains[0];
}
