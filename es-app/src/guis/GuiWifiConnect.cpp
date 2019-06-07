#include "guis/GuiWifiConnect.h"

#include "components/AnimatedImageComponent.h"
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"

#define HORIZONTAL_PADDING_PX 20

AnimationFrame WIFICON_ANIMATION_FRAMES[] = {
	{":/busy_0.svg", 300},
	{":/busy_1.svg", 300},
	{":/busy_2.svg", 300},
	{":/busy_3.svg", 300},
};
const AnimationDef WIFICON_ANIMATION_DEF = { WIFICON_ANIMATION_FRAMES, 4, true };

GuiWifiConnect::GuiWifiConnect(Window* window) : GuiComponent(window), mGrid(window, Vector2i(1, 3)), mBackground(window, ":/frame.png"), mTrys(0), mTimer(0), mState(0)
{

	float width = Renderer::getScreenWidth() * 0.6f; // max width
	
	mTitle = std::make_shared<TextComponent>(mWindow, "CONNECTING", Font::get(FONT_SIZE_LARGE), 0x555555FF, ALIGN_CENTER);
	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true);

	mText = std::make_shared<TextComponent>(mWindow, "TRYING TO CONNECT", Font::get(FONT_SIZE_MEDIUM), 0x777777FF, ALIGN_CENTER);
	mGrid.setEntry(mText, Vector2i(0, 1), true, false, Vector2i(1, 1), GridFlags::BORDER_TOP | GridFlags::BORDER_BOTTOM);

	animationGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(3, 1));
	mAnimation = std::make_shared<AnimatedImageComponent>(mWindow);
	mAnimation->load(&WIFICON_ANIMATION_DEF);
	animationGrid.addEntry(mAnimation,Vector(1,0),true,false);
	animationGrid->setSize(0, mTitle->getFont()->getHeight());
	mGrid.setEntry(animationGrid, Vector2i(0, 2), true, false, Vector2i(1, 1));

	addChild(&mBackground);
	addChild(&mGrid);
	
	setSize(Renderer::getScreenWidth() * 0.6f + HORIZONTAL_PADDING_PX*2, mTitle->getFont()->getHeight() + mText->getFont()->getHeight() + animationGrid->getSize().y() + 40);
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
}

void GuiWifiConnect::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	mText->setSize(mSize.x() - 40, mText->getSize().y());

	float fullHeight = mTitle->getFont()->getHeight() + mText->getSize().y() + animationGrid->getSize().y();

	// update grid
	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / fullHeight);
	mGrid.setRowHeightPerc(1, mText->getSize().y() / fullHeight);
	mGrid.setRowHeightPerc(2, animationGrid->getSize().y() / fullHeight);

	mGrid.setSize(mSize);
}

std::string GuiWifiConnect::getIP()
{
	FILE *wIPP;
	char wip[1035];
	std::string currentLine;
	std::size_t found;
	
	std::string wIP;
	wIPP = popen("hostname -I", "r");
	while (fgets(wip, sizeof(wip), wIPP) != NULL) {
		wIP = wip;
		found = wIP.find(".");
		if (found != std::string::npos) {
			int trim = wIP.find("\n");
			wIP = wIP.substr(0, trim-1);
		}else{
			wIP = "";
		}
	}
	return wIP;
}

void GuiWifiConnect::update(int deltaTime) {
	mTimer += deltaTime;
	if (mTimer > 1000){
		mTimer = 0;
		mTrys += 1;
		if(mState == 0){
			mState = 1;
			mTrys = 0;
			mText->setText("RESTARTING NETWORK");
			system("sudo systemctl daemon-reload");
			system("sudo systemctl restart dhcpcd &");
		}
		if(mState == 1 && mTrys > 20){
			delete this;
		}
	}
	GuiComponent::update(deltaTime);
}