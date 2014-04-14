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
    // create the root chain
    createEffectChain();

    RTT1 = Device->getVideoDriver()->addRenderTargetTexture(Device->getVideoDriver()->getCurrentRenderTargetSize(),"irrPP-RTT1");
    RTT2 = Device->getVideoDriver()->addRenderTargetTexture(Device->getVideoDriver()->getCurrentRenderTargetSize(), "irrPP-RTT2");
}

irr::video::irrPP::~irrPP()
{
    //dtor
}

void irr::video::irrPP::render(irr::video::ITexture* input, irr::video::ITexture* output)
{
    irr::u32 numActiveEffects = getActiveEffectCount();

    // first, let's order all the active effects
    EffectEntry orderedEffects[numActiveEffects];

    irr::u32 effectCounter = 0;
    irr::video::ITexture* usedRTT = input, *freeRTT;

    irr::u32 numChains = Chains.size();

    for (irr::u32 chain = 0; chain < numChains; chain++)
    {
        if (Chains[chain]->isActive())
        {
            irr::video::CPostProcessingEffectChain* thisChain = Chains[chain];
            irr::u32 numChainEffects = thisChain->getEffectCount();

            bool shouldveKeptLastRender = false;
            if (thisChain->getKeepOriginalRender())
                shouldveKeptLastRender = true;

            for (irr::u32 effect = 0; effect < numChainEffects; effect++)
            {
                if (thisChain->getEffectFromIndex(effect)->isActive())
                {
                    freeRTT = (usedRTT == RTT1 ? RTT2 : RTT1);

                    orderedEffects[effectCounter].effect = thisChain->getEffectFromIndex(effect);
                    orderedEffects[effectCounter].source = usedRTT;
                    orderedEffects[effectCounter].target = freeRTT;

                    if (shouldveKeptLastRender && effectCounter > 0)
                    {
                        orderedEffects[effectCounter - 1].target = thisChain->getOriginalRender();
                        orderedEffects[effectCounter].source = thisChain->getOriginalRender();

                        shouldveKeptLastRender = false;
                    }

                    usedRTT = freeRTT;
                    effectCounter++;
                }
            }
        }
    }

    // render the ordered effects
    for (irr::u32 i = 0; i < numActiveEffects; i++)
    {
        if (i < numActiveEffects - 1)
            Device->getVideoDriver()->setRenderTarget(orderedEffects[i].target);
        else // last effect
            Device->getVideoDriver()->setRenderTarget(0);

        Quad->setMaterialType(orderedEffects[i].effect->getMaterialType());
        Quad->setMaterialTexture(0, orderedEffects[i].source);

        irr::u32 numTexturesToPass = orderedEffects[i].effect->getTextureToPassCount();
        for (irr::u32 texToPass = 0; texToPass < numTexturesToPass; texToPass++)
            Quad->setMaterialTexture(texToPass + 1, orderedEffects[i].effect->getTextureToPass(texToPass));

        Quad->render();
    }
}





irr::video::CPostProcessingEffectChain* irr::video::irrPP::createEffectChain()
{
    irr::video::CPostProcessingEffectChain* chain = new irr::video::CPostProcessingEffectChain(Device, ShaderDir);
    Chains.push_back(chain);

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