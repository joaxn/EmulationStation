#include "components/MenuComponent.h"
#include "components/SwitchComponent.h"
#include "GuiComponent.h"

class GuiNetwork : public GuiComponent
{
public:
	GuiNetwork(Window* window);
	

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