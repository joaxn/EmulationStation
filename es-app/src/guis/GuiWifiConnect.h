#include "components/BusyComponent.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"
#include "GuiComponent.h"

class ComponentList;
class TextComponent;

class GuiWifiConnect : public GuiComponent
{
public:
	GuiWifiConnect(Window* window);

	void update(int deltaTime) override;

private:
	std::string getIP();

	MenuComponent mMenu;

	int mTimer;
	int mTrys;
	int mState;
	
	std::shared_ptr<AnimatedImageComponent> mAnimation;
	std::shared_ptr<TextComponent> mText;
};
