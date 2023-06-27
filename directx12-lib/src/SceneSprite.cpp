#include "SceneSprite.h"

void SceneSprite::init(SceneConf conf)
{
    //3角形のリソースを作成&初期化
    sprite = std::make_shared<Sprite>();
    Sprite::SpriteConf spriteConf = {};
    spriteConf.device = conf.device;
    sprite->init(spriteConf);
}

void SceneSprite::finalize()
{
}

void SceneSprite::update()
{
}

void SceneSprite::render(SceneConf conf)
{
    sprite->draw(conf.renderContext);
}

void SceneSprite::updateImguiMenu()
{
}
