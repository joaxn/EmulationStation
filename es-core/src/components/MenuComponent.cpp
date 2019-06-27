#include "components/MenuComponent.h"

#include "components/ButtonComponent.h"

#define BUTTON_GRID_VERT_PADDING 32
#define BUTTON_GRID_HORIZ_PADDING 10

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight())
#define SUBTITLE_HEIGHT (mSubtitle->getFont()->getLetterHeight())

MenuComponent::MenuComponent(Window* window, const char* title, const std::shared_ptr<Font>& titleFont, const char* subtitle, const std::shared_ptr<Font>& subtitleFont) : GuiComponent(window),
	mBackground(window), mGrid(window, Vector2i(1, 6))
{
	addChild(&mBackground);
	addChild(&mGrid);

	mBackground.setImagePath(":/frame.png");

	// set up title
	mTitle = std::make_shared<TextComponent>(mWindow);
	mTitle->setHorizontalAlignment(ALIGN_CENTER);
	mTitle->setColor(0x555555FF);
	setTitle(title, titleFont);
	mGrid.setEntry(mTitle, Vector2i(0, 1), false);
	
	// set up subtitle
	mSubtitle = std::make_shared<TextComponent>(mWindow);
	mSubtitle->setHorizontalAlignment(ALIGN_CENTER);
	mSubtitle->setColor(0x555555FF);
	setSubtitle(subtitle, subtitleFont);
	mGrid.setEntry(mSubtitle, Vector2i(0, 2), false);

	// set up list which will never change (externally, anyway)
	mList = std::make_shared<ComponentList>(mWindow);
	mGrid.setEntry(mList, Vector2i(0, 4), true);

	updateGrid();
	updateSize();

	mGrid.resetCursor();
}

void MenuComponent::setTitle(const char* title, const std::shared_ptr<Font>& font)
{
	mTitle->setText(Utils::String::toUpper(title));
	mTitle->setFont(font);
}

void MenuComponent::setSubtitle(const char* subtitle, const std::shared_ptr<Font>& font)
{
	mSubtitle->setText(Utils::String::toUpper(subtitle));
	mSubtitle->setFont(font);
}

float MenuComponent::getButtonGridHeight() const
{
	return (mButtonGrid ? mButtonGrid->getSize().y() : Font::get(FONT_SIZE_MEDIUM)->getHeight() + BUTTON_GRID_VERT_PADDING);
}

void MenuComponent::updateSize()
{
	const float maxHeight = Renderer::getScreenHeight() * 0.75f;
	float subtitleHeight;
	if(mSubtitle->getValue() != ""){
		subtitleHeight = SUBTITLE_HEIGHT;
	}else{
		subtitleHeight = 0;
	}
	float height = TITLE_HEIGHT + subtitleHeight + TITLE_VERT_PADDING * 2 + mList->getTotalRowHeight() + getButtonGridHeight() + 2;
	if(height > maxHeight)
	{
		height = TITLE_HEIGHT + subtitleHeight + TITLE_VERT_PADDING * 2 + getButtonGridHeight();
		int i = 0;
		while(i < mList->size())
		{
			float rowHeight = mList->getRowHeight(i);
			if(height + rowHeight < maxHeight)
				height += rowHeight;
			else
				break;
			i++;
		}
	}

	float width = (float)Math::min((int)Renderer::getScreenHeight(), (int)(Renderer::getScreenWidth() * 0.90f));
	setSize(width, height);
}

void MenuComponent::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));
	
	float subtitleHeight;
	if(mSubtitle->getValue() != ""){
		subtitleHeight = SUBTITLE_HEIGHT + TITLE_LINE_ADD;
	}else{
		subtitleHeight = 1;
	}
	
	// update grid row/col sizes
	mGrid.setRowHeightPerc(0, TITLE_VERT_PADDING / mSize.y());
	mGrid.setRowHeightPerc(1, (TITLE_HEIGHT + TITLE_LINE_ADD) / mSize.y());
	mGrid.setRowHeightPerc(2, subtitleHeight / mSize.y());
	mGrid.setRowHeightPerc(3, TITLE_VERT_PADDING / mSize.y());
	mGrid.setRowHeightPerc(5, getButtonGridHeight() / mSize.y());

	mGrid.setSize(mSize);
}

void MenuComponent::addButton(const std::string& name, const std::string& helpText, const std::function<void()>& callback)
{
	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, Utils::String::toUpper(name), helpText, callback));
	updateGrid();
	updateSize();
}

void MenuComponent::updateGrid()
{
	if(mButtonGrid)
		mGrid.removeEntry(mButtonGrid);

	mButtonGrid.reset();

	if(mButtons.size())
	{
		mButtonGrid = makeButtonGrid(mWindow, mButtons);
		mGrid.setEntry(mButtonGrid, Vector2i(0, 5), true, false);
	}
}

std::vector<HelpPrompt> MenuComponent::getHelpPrompts()
{
	return mGrid.getHelpPrompts();
}

std::shared_ptr<ComponentGrid> makeButtonGrid(Window* window, const std::vector< std::shared_ptr<ButtonComponent> >& buttons)
{
	std::shared_ptr<ComponentGrid> buttonGrid = std::make_shared<ComponentGrid>(window, Vector2i((int)buttons.size(), 2));

	float buttonGridWidth = (float)BUTTON_GRID_HORIZ_PADDING * buttons.size(); // initialize to padding
	for(int i = 0; i < (int)buttons.size(); i++)
	{
		buttonGrid->setEntry(buttons.at(i), Vector2i(i, 0), true, false);
		buttonGridWidth += buttons.at(i)->getSize().x();
	}
	for(unsigned int i = 0; i < buttons.size(); i++)
	{
		buttonGrid->setColWidthPerc(i, (buttons.at(i)->getSize().x() + BUTTON_GRID_HORIZ_PADDING) / buttonGridWidth);
	}

	buttonGrid->setSize(buttonGridWidth, buttons.at(0)->getSize().y() + BUTTON_GRID_VERT_PADDING + 2);
	buttonGrid->setRowHeightPerc(1, 2 / buttonGrid->getSize().y()); // spacer row to deal with dropshadow to make buttons look centered

	return buttonGrid;
}

std::shared_ptr<ImageComponent> makeArrow(Window* window)
{
	auto bracket = std::make_shared<ImageComponent>(window);
	bracket->setImage(":/arrow.svg");
	bracket->setResize(0, Math::round(Font::get(FONT_SIZE_MEDIUM)->getLetterHeight()));
	return bracket;
}
