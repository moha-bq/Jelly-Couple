#include "Editor.h"

#if ENABLE_EDITOR
Editor::Editor()
{
}

Editor::~Editor()
{
}

bool Editor::init()
{
	if (!DrawNode::init())
		return false;

	keep_butt = ui::Button::create("button.png");
	keep_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		char full_path_name[128] = "/storage/sdcard0/DCIM/";

		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			strcat(full_path_name, editor->inputs[num_editor_props - 2]->getText());
			save_editor_config_to_file(full_path_name);
			break;
		default:
			break;
		}
	});
	keep_butt->setPosition(Vec2(130, 20));
	keep_butt->setTitleText("Keep");
	keep_butt->setTitleFontSize(8);

	load_butt = ui::Button::create("button.png");
	load_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		char full_path_name[128] = "/storage/sdcard0/DCIM/";
		FILE* file;
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			strcat(full_path_name, editor->inputs[num_editor_props - 1]->getText());
			file = fopen(full_path_name, "r");
			if (file)
			{
				char buff[1024];
				while (!feof(file))
				{
					char param_name[128];
					fgets(buff, 1024, file);

					sscanf(buff, "%s", param_name);
					for (int i = 0; i < num_editor_props - 2; i++)
					{
						if (!strcmp(param_name, editor_prop_names[i]))
						{
							if (editor_prop_types[i])
							{
								float fv;
								sscanf(buff + strlen(param_name) + 1, "%f", &fv);
								char float_string[128];
								sprintf(float_string, "%f", fv);
								inputs[i]->setText(float_string);

							}
							else
							{
								int iv;
								sscanf(buff + strlen(param_name) + 1, "%d", &iv);
								char int_string[128];
								sprintf(int_string, "%d", iv);
								inputs[i]->setText(int_string);

								sscanf(buff + strlen(param_name) + 1, "%d", &iv);
							}
						}
					}
				}
				fclose(file);
			}
			else
			{
				log("failed to load config %s", full_path_name);
			}
			break;
		default:
			break;
		}
	});
	load_butt->setPosition(Vec2(240, 20));
	load_butt->setTitleText("Load");
	load_butt->setTitleFontSize(8);
	addChild(load_butt);

	ok_butt = ui::Button::create("button.png");
	ok_butt->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::ENDED:
			editor->pause();
			editor->setVisible(false);
			in_editor = false;

			for (int i = 0; i < num_editor_props - 2; i++)
			{
				if (editor_prop_types[i])
				{
					*((float*)&config + i) = strtof(editor->inputs[i]->getText(), 0);
				}
				else
				{
					int to_watch = strtol(editor->inputs[i]->getText(), 0, 10);
					*((int*)&config + i) = to_watch;
				}
			}
			levels_data[current_level].config = config;
			save_editor_config_to_file("/storage/sdcard0/DCIM/LAST_CONFIG.TXT");
			break;
		default:
			break;
		}
	});
	ok_butt->setPosition(Vec2(350, 20));
	ok_butt->setTitleText("OK");
	ok_butt->setTitleFontSize(8);

	this->addChild(ok_butt);
	this->addChild(keep_butt);

	this->drawSolidRect(Vec2(0, 0), Vec2(480, 780), Color4F(1, 1, 1, 1));


	int y = 750;
	int x = 90;
	int y_spacing = 25;

	for (int i = 0; i < num_editor_props; i++)
	{
		labels[i] = ui::Text::create(editor_prop_names[i], "Arial", 12);
		labels[i]->setColor(Color3B(0, 0, 0));
		labels[i]->setPosition(Vec2(x, y));

		inputs[i] = ui::EditBox::create(Size(240, 25), ui::Scale9Sprite::create("green_edit.png"));
		inputs[i]->setFontName("Arial");
		inputs[i]->setFontSize(15);
		inputs[i]->setFontColor(Color3B(0, 0, 0));
		inputs[i]->setAnchorPoint(Vec2(0, 0));
		inputs[i]->setPosition(Vec2(240, y - 12));

		//this->drawRect(Vec2(240 - 10, y+14), Vec2(480, y-11),Color4F(0,0,0,1));

		y -= y_spacing;
		addChild(labels[i]);
		addChild(inputs[i]);
	}

	FILE* file = fopen("/storage/sdcard0/DCIM/LAST_CONFIG.TXT", "r");
	if (file)
	{
		char buff[1024];
		while (!feof(file))
		{
			char param_name[128];
			fgets(buff, 1024, file);

			sscanf(buff, "%s", param_name);
			for (int i = 0; i < num_editor_props - 2; i++)
			{
				if (!strcmp(param_name, editor_prop_names[i]))
				{
					if (editor_prop_types[i])
					{
						float fv;
						sscanf(buff + strlen(param_name) + 1, "%f", &fv);
						char float_string[128];
						sprintf(float_string, "%f", fv);
						inputs[i]->setText(float_string);

					}
					else
					{
						int iv;
						sscanf(buff + strlen(param_name) + 1, "%d", &iv);
						char int_string[128];
						sprintf(int_string, "%d", iv);
						inputs[i]->setText(int_string);

						sscanf(buff + strlen(param_name) + 1, "%d", &iv);
					}
				}
			}
		}
		fclose(file);
	}
	else
	{
		log("failed to load last_config [first_init]");
	}

	return true;
}

#endif