#include <irrlicht.h>
#include <irrPP.h>

using namespace irr;

void createScene(IrrlichtDevice *device);

int main()
{
    IrrlichtDevice *device = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(800, 600), 32);

    if (!device)
        return 1;

    video::IVideoDriver* driver = device->getVideoDriver();
    device->setWindowCaption(L"irrPP Demo");

    // create a scene
    createScene(device);

    //! we'll need to render our scene into a texture, then let irrPP post-process it.
    //! initialize irrPP
    video::irrPP* pp = createIrrPP(device);

    //pp->createEffect(video::EPE_COLD_COLORS);
    pp->createEffect(video::EPE_FXAA);
    //pp->createEffect(video::EPE_BLUR_V);
    //pp->createEffect(video::EPE_BLUR_H);

    //pp->createEffect(video::EPE_ALBEDO);

    irr::video::CPostProcessingEffectChain* bloom = pp->createEffectChain();
    bloom->setKeepOriginalRender(true);
    bloom->createEffect(video::EPE_BLOOM_PREPASS); // only let bright areas of the image through
    bloom->createEffect(video::EPE_BLUR_V); // blur them vertically
    bloom->createEffect(video::EPE_BLUR_H_ADD)->addTextureToShader(bloom->getOriginalRender()); // blur horizontally and add to the original render


    //! this is how we would create the render target texture.
    //! since we're lazy, we won't do that - it's not really necessary, which will become clear shortly
    //video::ITexture* rt = driver->addRenderTargetTexture(driver->getCurrentRenderTargetSize(), "RTT1");

    while (device->run())
    {
        driver->beginScene(true, true, video::SColor(255,100,101,140));

        //! at the beginning of each render loop, we'll set the render target texture we created as the render target
        //! but since irrPP created 2 RTT's anywaydx, we can just use any one of those instead:
        driver->setRenderTarget(pp->getRTT2());

        device->getSceneManager()->drawAll();

        //! after everything has been rendered into the texture, we'll let irrPP render the effects
        //! we need to supply it with the RTT it will render the effects onto
        pp->render(pp->getRTT2());
        //! this will render directly to screen. we could also pass in a second RTT:
        //! pp->render(rt, myOutputRTT);
        //! then, the  final result would be rendered into that texture. something like this:
        //! pp->render(rt, rt);
        //! should therefore work as well, provided there are at least two active effects :)

        driver->endScene();
    }
}

void createScene(IrrlichtDevice *device)
{
    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();

    scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
        "media/terrain-heightmap.bmp",
        0,                  // parent node
        -1,                 // node id
        core::vector3df(0.f, 0.f, 0.f),     // position
        core::vector3df(0.f, 0.f, 0.f),     // rotation
        core::vector3df(40.f, 4.4f, 40.f),  // scale
        video::SColor ( 255, 255, 255, 255 ),   // vertexColor
        5,                  // maxLOD
        scene::ETPS_17,             // patchSize
        4                   // smoothFactor
        );

    terrain->setMaterialFlag(video::EMF_LIGHTING, false);
    terrain->setMaterialTexture(0,
            driver->getTexture("media/terrain-texture.jpg"));
    terrain->setMaterialTexture(1,
            driver->getTexture("media/detailmap3.jpg"));
    terrain->setMaterialType(video::EMT_DETAIL_MAP);
    terrain->scaleTexture(1.0f, 20.0f);

    scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS(0, 100.0, 1.0);
    cam->setFarValue(8000);
    cam->setPosition(core::vector3df(5000, 1000, 5000));

    scene::ISceneNode* cube = smgr->addCubeSceneNode(200);
    cube->setMaterialFlag(video::EMF_LIGHTING, false);
    cube->setPosition(cam->getPosition());
}
