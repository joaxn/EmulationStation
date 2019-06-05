#pragma once
#ifndef ES_APP_GUIS_GUI_MENU_H
#define ES_APP_GUIS_GUI_MENU_H

#include "components/MenuComponent.h"
#include "GuiComponent.h"

class GuiNetwork : public GuiComponent
{
public:
	GuiNetwork(Window* window);

	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	void onSizeChanged() override;
	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	std::string getIP();
	std::string getNetStatus();
	bool getWifiBool();
	
	int mTimer;
	std::string mCurrent;
	std::shared_ptr<TextComponent> updateIP;
	
	MenuComponent mMenu;
	TextComponent mVersion;
};

#endif // ES_APP_GUIS_GUI_MENU_H