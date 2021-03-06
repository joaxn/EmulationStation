#include "components/BusyComponent.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"
#include "GuiComponent.h"

class ComponentList;
class TextComponent;

class GuiWifiConnect : public GuiComponent
{
public:
	GuiWifiConnect(Window* window,const std::function<void()>& callback);

	void update(int deltaTime) override;
	void onSizeChanged() override;

private:
	
	NinePatchComponent mBackground;
	ComponentGrid mGrid;
	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<ComponentGrid> mAnimationGrid;
	std::shared_ptr<AnimatedImageComponent> mAnimation;
	std::shared_ptr<TextComponent> mText;
	std::function<void()> okCallback;
	
	int mTimer;
	int mTrys;
	int mState;
};
