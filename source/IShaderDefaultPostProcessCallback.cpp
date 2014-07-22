#include "IShaderDefaultPostProcessCallback.h"


irr::video::IShaderDefaultPostProcessCallback::IShaderDefaultPostProcessCallback()
    :HaveIDs(false)
{

}

void irr::video::IShaderDefaultPostProcessCallback::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
{
    if (!HaveIDs)
    {
        RenderID = services->getPixelShaderConstantID("Render");

        for (u32 i = 0; i < 1; i++)
            TexIDs[i] = services->getPixelShaderConstantID((irr::core::stringc("Tex") + irr::core::stringc(i)).c_str());

        PixelSizeID = services->getPixelShaderConstantID("PixelSize");

        HaveIDs = true;
    }

    irr::s32 render= 0;
    services->setPixelShaderConstant(RenderID, &render, 1);

    for (u32 i = 0; i < NumTextures; i++)
    {
        irr::s32 tex = i+1;
        services->setPixelShaderConstant(TexIDs[i], &tex, 1);
    }

    //some more useful stuff for post processing shaders
    services->setPixelShaderConstant(PixelSizeID, (irr::f32*)&PixelSize, 2);
}

void irr::video::IShaderDefaultPostProcessCallback::OnSetMaterial (const SMaterial &material)
{
    irr::core::dimension2du tSize = material.getTexture(0)->getSize();
    PixelSize = irr::core::vector2df(1.0 / tSize.Width, 1.0 / tSize.Height);

    NumTextures = 0;
    for (u32 i = 1; i <= 4; i++)
    {
        if (material.getTexture(i))
            NumTextures++;
        else
            break;
    }
}
