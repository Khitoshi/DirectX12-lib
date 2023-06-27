#include "SceneSprite.h"

void SceneSprite::init(SceneConf conf)
{
    //3�p�`�̃��\�[�X���쐬&������
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
