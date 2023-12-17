/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "MainScene.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    // super init first
    if ( !Scene::init() )
    {
        return false;
    }

    // graphics config
    static cocos2d::Size designResolutionSize = cocos2d::Size(1920, 1080);
    Director::getInstance()->setDisplayStats(false);
    auto glview = Director::getInstance()->getOpenGLView();
    glview->setFrameSize(1920, 1080);
    glview->setDesignResolutionSize(
        designResolutionSize.width,
        designResolutionSize.height,
        ResolutionPolicy::SHOW_ALL
    );
    cocos2d::Director::getInstance()->setContentScaleFactor(0.5f);

    // useful vectors
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add jump menu item
    auto jumpItem = MenuItemImage::create(
                                           "path940.png",
                                           "path940.png",
                                           CC_CALLBACK_1(MainScene::jumpCallback, this));

    if (jumpItem == nullptr ||
        jumpItem->getContentSize().width <= 0 ||
        jumpItem->getContentSize().height <= 0)
    {
        problemLoading("'path940.png' and 'path940.png'");
    }
    else
    {
        jumpItem->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - jumpItem->getContentSize().height / 3));
        jumpItem->setScale(0.25f);
    }

    // add a label shows "Drag Scene"
    auto label = Label::createWithTTF("DRAG SCENE", "fonts/arial.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));
        this->addChild(label, 1);
    }
    // create menu
    auto menu = Menu::create(jumpItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    // add background image
    auto background = Sprite::create("2.png");
    this->addChild(background);

    // add 1st parallax layer
    auto layer1 = Sprite::create("3.png");
    layer1->setPosition(Vec2(origin.x, origin.y));
    initialLayerPositions.push_back(layer1->getPosition());
    auto sublayer1_1 = Sprite::create("3.png");
    auto sublayer1_2 = Sprite::create("3.png");
    sublayer1_1->setPosition(layer1->convertToNodeSpace(Vec2(origin.x + layer1->getContentSize().width, origin.y)));
    sublayer1_2->setPosition(layer1->convertToNodeSpace(Vec2(origin.x - layer1->getContentSize().width, origin.y)));
    layer1->addChild(sublayer1_1);
    layer1->addChild(sublayer1_2);
    this->addChild(layer1);

    // add 2nd parallax layer
    auto layer2 = Sprite::create("4.png");
    layer2->setPosition(Vec2(origin.x, origin.y));
    initialLayerPositions.push_back(layer2->getPosition());
    auto sublayer2_1 = Sprite::create("4.png");
    auto sublayer2_2 = Sprite::create("4.png");
    sublayer2_1->setPosition(layer2->convertToNodeSpace(Vec2(origin.x + layer2->getContentSize().width, origin.y)));
    sublayer2_2->setPosition(layer2->convertToNodeSpace(Vec2(origin.x - layer2->getContentSize().width, origin.y)));
    layer2->addChild(sublayer2_1);
    layer2->addChild(sublayer2_2);
    this->addChild(layer2);

    // store layers in a vector for parallax effects
    layers.pushBack(layer1);
    layers.pushBack(layer2);

    // spawn character
    character = Sprite::create("character.png");
    character->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + 280));
    character->setScale(0.25f);
    this->addChild(character);

    // listen for drag
    auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}





void MainScene::jumpCallback(Ref* pSender)
{
    auto jump = JumpBy::create(0.5, Vec2(0, 0), 100, 1);
    character->runAction(jump);
}

bool MainScene::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
    lastTouchPos = touch->getLocation();
    return true;
}

void MainScene::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {
    auto currentTouchPos = touch->getLocation();
    auto delta = currentTouchPos - lastTouchPos;

    // Update the position of the parallax node based on the dragging movement
    for (int i = 0; i < layers.size(); i++)
    {
        layers.at(i)->setPosition(
            Vec2(
                layers.at(i)->getPositionX() + delta.x * (i + 1) * 0.2f,
                layers.at(i)->getPositionY()
            ));
        if (abs(layers.at(i)->getPositionX()) > layers.at(i)->getContentSize().width / 2)
        {
            layers.at(i)->setPosition(initialLayerPositions[i]);
        }
    }
    //paraNode->setPosition(paraNode->getPosition() + delta);
    lastTouchPos = currentTouchPos;
}
