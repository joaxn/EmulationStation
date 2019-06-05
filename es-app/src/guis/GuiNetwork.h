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