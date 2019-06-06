#include "components/MenuComponent.h"
#include "GuiComponent.h"

class GuiNetwork : public GuiComponent
{
public:
	GuiNetwork(Window* window);
	
	void save();
	inline void addSaveFunc(const std::function<void()>& func) { mSaveFuncs.push_back(func); };
	
	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	std::vector< std::function<void()> > mSaveFuncs;
	
	std::string getIP();
	std::string getNetStatus();
	bool getWifiBool();
	void displayNetworkSettings();
	
	int mTimer;
	
	MenuComponent mMenu;
};