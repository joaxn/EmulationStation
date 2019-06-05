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
	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	std::string getIP();
	std::string getNetStatus();
	bool getWifiBool();
	void displayNetworkSettings();
	
	int mTimer;
	
	MenuComponent mMenu;
};

#endif // ES_APP_GUIS_GUI_MENU_H