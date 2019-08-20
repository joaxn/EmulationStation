#include "components/MenuComponent.h"
#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "GuiComponent.h"

class GuiStorage : public GuiComponent
{
public:
	GuiStorage(Window* window);
	bool input(InputConfig* config, Input input) override;

private:
	void writeStorageSettings();
	
	MenuComponent mMenu;
};