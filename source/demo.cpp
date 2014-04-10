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

    pp->createEffect(irr::video::EPE_WARM_COLORS);
    pp->createEffect(irr::video::EPE_FXAA);
    //pp->createEffect(irr::video::EPE_BLUR_V);
    //pp->createEffect(irr::video::EPE_BLUR_H);


    //! let's create the render target texture
    video::ITexture* rt = driver->addRenderTargetTexture(driver->getCurrentRenderTargetSize(), "RTT1");

    while (device->run())
    {
        driver->beginScene(true, true, video::SColor(255,100,101,140));

        //! at the beginning of each render loop, we'll set the render target texture we created as the render target
        driver->setRenderTarget(rt);

        device->getSceneManager()->drawAll();
        //guienv->drawAll();

        //! after everything has been rendered into the texture, we'll let irrPP render the effects
        //! we'll set the render target back to screen before that, because that's where we want our "output"
        driver->setRenderTarget(0);
        pp->render(rt);

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


    scene::ICameraSceneNode* cam = device->getSceneManager()->addCameraSceneNodeFPS(0, 100.0, 1.0);
    cam->setFarValue(8000);
    cam->setPosition(core::vector3df(5000, 1000, 5000));
}
