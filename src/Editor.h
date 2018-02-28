#pragma once


#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "globals.h"

#if ENABLE_EDITOR
using namespace cocos2d;

struct  Editor : public cocos2d::DrawNode
{
	Editor();
    virtual ~Editor();
	CREATE_FUNC(Editor);

	virtual bool init();

	ui::Button* keep_butt;
	ui::Button* ok_butt;
	ui::Button* load_butt;

	ui::Text* labels[32];
	ui::EditBox* inputs[32];
};

#endif


