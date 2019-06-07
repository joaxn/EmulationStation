#include "guis/GuiWifiConnect.h"

#include "components/AnimatedImageComponent.h"
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"

#define HORIZONTAL_PADDING_PX 20

AnimationFrame BUSY_ANIMATION_FRAMES[] = {
	{":/busy_0.svg", 300},
	{":/busy_1.svg", 300},
	{":/busy_2.svg", 300},
	{":/busy_3.svg", 300},
};
const AnimationDef BUSY_ANIMATION_DEF = { BUSY_ANIMATION_FRAMES, 4, true };

GuiWifiConnect::GuiWifiConnect(Window* window) : GuiComponent(window), mBackground(window, ":/frame.png"), mGrid(window, Vector2i(5, 3)), mTrys(0), mTimer(0), mState(0)
{

	mText = std::make_shared<TextComponent>(mWindow, "TRYING TO CONNECT", Font::get(FONT_SIZE_MEDIUM), 0x777777FF, ALIGN_CENTER);
	mGrid.setEntry(mText, Vector2i(0, 0), false, false);

	mAnimation = std::make_shared<AnimatedImageComponent>(mWindow);
	mAnimation->load(&BUSY_ANIMATION_DEF);

	mGrid.setEntry(mAnimation, Vector2i(1, 1), false, true);
	mGrid.setEntry(mText, Vector2i(3, 1), false, true);

	addChild(&mBackground);
	addChild(&mGrid);
}

std::string GuiNetwork::getIP()
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

void GuiWifiConnect::onSizeChanged()
{
	mGrid.setSize(mSize);

	if(mSize.x() == 0 || mSize.y() == 0)
		return;

	const float middleSpacerWidth = 0.01f * Renderer::getScreenWidth();
	const float textHeight = mText->getFont()->getLetterHeight();
	mText->setSize(0, textHeight);
	const float textWidth = mText->getSize().x() + 4;

	mGrid.setColWidthPerc(1, textHeight / mSize.x()); // animation is square
	mGrid.setColWidthPerc(2, middleSpacerWidth / mSize.x());
	mGrid.setColWidthPerc(3, textWidth / mSize.x());

	mGrid.setRowHeightPerc(1, textHeight / mSize.y());
	
	mBackground.fitTo(Vector2f(mGrid.getColWidth(1) + mGrid.getColWidth(2) + mGrid.getColWidth(3), textHeight + 2),
		mAnimation->getPosition(), Vector2f(0, 0));
}

void GuiWifiConnect::update(int deltaTime) {
	mTimer += deltaTime;
	if (mTimer > 1000){
		mTimer = 0;
		mTrys += 1;
		if(mState == 0){
			mSate = 1;
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