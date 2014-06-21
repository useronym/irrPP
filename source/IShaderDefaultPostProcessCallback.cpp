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

        for (u32 i = 0; i < 3; i++)
            TexIDs[i] = services->getPixelShaderConstantID((irr::core::stringc("Tex") + irr::core::stringc(i)).c_str());

        PixelSizeXID = services->getPixelShaderConstantID("PixelSizeX");
        PixelSizeYID = services->getPixelShaderConstantID("PixelSizeY");

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
    irr::core::dimension2du screensize= services->getVideoDriver()->getCurrentRenderTargetSize();
    irr::f32 pixelSizeX= 1.0/screensize.Width;
    irr::f32 pixelSizeY= 1.0/screensize.Height;
    services->setPixelShaderConstant(PixelSizeXID, (irr::f32*)&pixelSizeX, 1);
    services->setPixelShaderConstant(PixelSizeYID, (irr::f32*)&pixelSizeY, 1);
}

void irr::video::IShaderDefaultPostProcessCallback::OnSetMaterial (const SMaterial &material)
{
    NumTextures = 0;
    for (u32 i = 1; i < 4; i++)
    {
        if (material.getTexture(i))
            NumTextures++;
        else
            break;
    }
}
