#include "guis/GuiWifiConnect.h"

#include "components/AnimatedImageComponent.h"
#include "components/MenuComponent.h"
#include "components/ImageComponent.h"
#include "components/TextComponent.h"
#include "components/ButtonComponent.h"

#include "utils/NetworkUtil.h"
#include "utils/StringUtil.h"

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
	mGrid.setEntry(mText, Vector2i(0, 1), true, false, Vector2i(1, 1));

	mAnimationGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(5, 1));
	mAnimation = std::make_shared<AnimatedImageComponent>(mWindow);
	mAnimation->load(&WIFICON_ANIMATION_DEF);
	mAnimation->setSize(mText->getFont()->getHeight(), mText->getFont()->getHeight());
	mAnimationGrid->setEntry(mAnimation,Vector2i(2,0),true,false);
	mAnimationGrid->setSize(0, mText->getFont()->getHeight());
	mGrid.setEntry(mAnimationGrid, Vector2i(0, 2), true, false, Vector2i(1, 1));

	addChild(&mBackground);
	addChild(&mGrid);
	
	setSize(Renderer::getScreenWidth() * 0.6f + HORIZONTAL_PADDING_PX*2, mTitle->getFont()->getHeight() + mText->getFont()->getHeight() + mAnimationGrid->getSize().y() + 40);
	setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
}

void GuiWifiConnect::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	mTitle->setSize(mSize.x() - 40, mTitle->getSize().y());
	mText->setSize(mSize.x() - 40, mText->getSize().y());

	float fullHeight = mTitle->getFont()->getHeight() + mText->getSize().y() + mAnimationGrid->getSize().y();

	// update grid
	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / fullHeight);
	mGrid.setRowHeightPerc(1, mText->getSize().y() / fullHeight);
	mGrid.setRowHeightPerc(2, mAnimationGrid->getSize().y() / fullHeight);

	mGrid.setSize(mSize);
}

void GuiWifiConnect::update(int deltaTime) {
	mTimer += deltaTime;
	if (mState == 0){
		mState = 1;
		mTimer = 0;
		mTrys = 0;
		mText->setText("RESTARTING NETWORK");
		system("sudo ip addr flush dev wlan0");
		system("sudo systemctl daemon-reload");
		system("sudo systemctl restart dhcpcd > /tmp/networkrestart.txt");
	}
	else if (mState == 1 && mTimer > 1000){
		mTimer = 0;
		mTrys += 1;
		mText->setText("TRYING TO CONNECT");
		if(Utils::Network::isIP() == true){
			mText->setText("SUCESSFULLY CONNECTED");
			mState = 2;
		} else if (mTrys > 15){
			mText->setText("ERROR CONNECTING TO NETWORK");
			mState = 2;
		}
	}
	else if (mState == 2 && mTimer > 3000){
		delete this;
	}
	GuiComponent::update(deltaTime);
}