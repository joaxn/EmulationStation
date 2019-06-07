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

GuiWifiConnect::GuiWifiConnect(Window* window) : GuiComponent(window), mMenu(window, "NETWORK SETTINGS"), mTrys(0), mTimer(0), mState(0)
{

	ComponentListRow row;

	mText = std::make_shared<TextComponent>(mWindow, "TRYING TO CONNECT", Font::get(FONT_SIZE_MEDIUM), 0x777777FF, ALIGN_CENTER);
	row.addElement(mText, true);
	mMenu.addRow(row);
	row.elements.clear();

	mAnimation = std::make_shared<AnimatedImageComponent>(mWindow);
	mAnimation->load(&WIFICON_ANIMATION_DEF);
	row.addElement(mAnimation, true);
	mMenu.addRow(row);
	row.elements.clear();

	addChild(&mMenu);
	setSize(mMenu.getSize());
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
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