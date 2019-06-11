#include "components/MenuComponent.h"
#include "components/SwitchComponent.h"
#include "GuiComponent.h"

class GuiNetwork : public GuiComponent
{
public:
	GuiNetwork(Window* window);
	
	void save();
	
	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	void connect();
	void writeNetworkSettings();
	
	int mTimer;
	int mState;
	int mTrys;
	std::shared_ptr<TextComponent> updateStat;
	std::shared_ptr<TextComponent> updateIP;
	std::shared_ptr<SwitchComponent> wifi_enabled;
	MenuComponent mMenu;
};