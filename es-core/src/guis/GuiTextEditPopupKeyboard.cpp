#include "guis/GuiTextEditPopupKeyboard.h"
#include "components/MenuComponent.h"
#include "utils/StringUtil.h"
#include "Log.h"
#include <locale>

GuiTextEditPopupKeyboard::GuiTextEditPopupKeyboard(Window* window, const std::string& title, const std::string& initValue, const std::function<void(const std::string&)>& okCallback, bool multiLine, const char* acceptBtnText): GuiComponent(window), mBackground(window, ":/frame.png"), mGrid(window, Vector2i(1, 7)), mMultiLine(multiLine)
{
	addChild(&mBackground);
	addChild(&mGrid);
 
	float gridHeight, buttonHeight, buttonWidth, gridWidth;
	float horizPadding = (float) 20;

	mTitle = std::make_shared<TextComponent>(mWindow, title, Font::get(FONT_SIZE_LARGE), 0x555555FF, ALIGN_CENTER);
	mTitle->setUppercase(true);
	mKeyboardGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(COLUMNS, ROWS));
	mButtonGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(4, 1));

	mText = std::make_shared<TextEditComponent>(mWindow);
	mText->setValue(initValue);
	mText->forceCursor(true);

	if(!multiLine)
		mText->setCursor(initValue.size());

	// Header
	mGrid.setEntry(mTitle, Vector2i(0, 0), false, true);

	// Text edit add
	mGrid.setEntry(mText, Vector2i(0, 1), false, false, Vector2i(1, 1));

	// Keyboard
	// Case for if multiline is enabled, then don't create the keyboard.
	if (!mMultiLine) {

		std::locale loc;
		
		// Digit Row
		for (int y = 0; y < ROWS; y++) {
			std::vector< std::shared_ptr<ButtonComponent> > buttons;
			for (int x = 0; x < COLUMNS; x++) {
				if (charArray[y][x] == "sft"){
					mShiftButton = std::make_shared<ButtonComponent>(mWindow, charArray[y][x], "SHIFTS FOR UPPER,LOWER, AND SPECIAL", [this] {
						if (mShift) mShift = false;
						else mShift = true;
						shiftKeys();
					},false,minText,":/kbd/sft.svg");
					buttons.push_back(mShiftButton);
	        	}
				else if (charArray[y][x] == "chr"){
					mSpecialButton = std::make_shared<ButtonComponent>(mWindow, charArray[y][x], "SPECIAL CHARACTERS", [this] {
						if (mSpecial) mSpecial = false;
						else mSpecial = true;
						specialKeys();
					},false,minText,":/kbd/chr.svg");
					buttons.push_back(mSpecialButton);
	        	}
				else if (charArray[y][x] == "cul"){
					buttons.push_back(std::make_shared<ButtonComponent>(mWindow, charArray[y][x], charArrayUp[y][x], [this] {
						mText->moveCursorLeft();
					},false,minText,":/kbd/cul.svg"));
	        	}
				else if (charArray[y][x] == "cur"){
					buttons.push_back(std::make_shared<ButtonComponent>(mWindow, charArray[y][x], charArrayUp[y][x], [this] {
						mText->moveCursorRight();
					},false,minText,":/kbd/cur.svg"));
	        	}
	        	else {
					buttons.push_back(std::make_shared<ButtonComponent>(mWindow, charArray[y][x], charArrayUp[y][x], [this, okCallback, x, y, loc] {
						okCallback(mText->getValue());
						mText->startEditing();
						if (mSpecial) mText->textInput(charArraySpecial[y][x]);
						else if(mShift) mText->textInput(charArrayUp[y][x]);
						else mText->textInput(charArray[y][x]);
						mText->stopEditing();
					},false,minText));
				}
				// Send just created button into mGrid
				mKeyboardGrid->setEntry(buttons[x], Vector2i(x, y), true, false);
			}
			buttonList.push_back(buttons);
		}
		
		buttonWidth = buttonList.at(0).at(0)->getSize().x();
		buttonHeight = buttonList.at(0).at(0)->getSize().y();
		gridHeight = (buttonHeight + 2) * ROWS + 2;
		gridWidth = (buttonWidth + 2) * COLUMNS + 2;
		mKeyboardGrid->setSize(gridWidth, gridHeight);
		mGrid.setEntry(mKeyboardGrid, Vector2i(0, 2), true, false);
	}

	// Accept/Cancel buttons
	std::vector< std::shared_ptr<ButtonComponent> > buttons;
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, "CANCEL", "DISCARD CHANGES", [this] { delete this; },true,"CANCEL"));
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, "SPACE", "SPACE", [this] {mText->startEditing();mText->textInput(" ");mText->stopEditing();},true,"CANCEL",":/kbd/spc.svg"));
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, "DELETE", "DELETE A CHAR", [this] {mText->startEditing();mText->textInput("\b");mText->stopEditing();},true,"CANCEL",":/kbd/del.svg"));
	buttons.push_back(std::make_shared<ButtonComponent>(mWindow, acceptBtnText, acceptBtnText, [this, okCallback] { okCallback(mText->getValue()); delete this; },true,"CANCEL"));

	// Add a/c buttons
	mButtonGrid->setEntry(buttons[0], Vector2i(0, 0), true, false);
	mButtonGrid->setEntry(buttons[1], Vector2i(1, 0), true, false);
	mButtonGrid->setEntry(buttons[2], Vector2i(2, 0), true, false);
	mButtonGrid->setEntry(buttons[3], Vector2i(3, 0), true, false);
	
	buttonWidth = buttons.at(0)->getSize().x();
	buttonHeight = buttons.at(0)->getSize().y();
	gridHeight = buttonHeight + 20;
	gridWidth = (buttonWidth + 10) * buttons.size() + 20;
	mButtonGrid->setSize(gridWidth, gridHeight);
	mGrid.setEntry(mButtonGrid, Vector2i(0, 3), true, false);

	

	// Determine size from text size
	float textHeight = mText->getFont()->getHeight();
	if (multiLine)
		textHeight *= 6;
	mText->setSize(0, textHeight);

	// If multiline, set all diminsions back to default, else draw size for keyboard.
	if (mMultiLine) {
		setSize(Renderer::getScreenWidth() * 0.5f, mTitle->getFont()->getHeight() + textHeight + mKeyboardGrid->getSize().y() + 40);
		setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
	}
	else {
		// Set size based on ScreenHieght * .08f by the amount of keyboard rows there are.
		setSize(mKeyboardGrid->getSize().x()+40, mTitle->getFont()->getHeight() + textHeight + 40 + mKeyboardGrid->getSize().y() + mButtonGrid->getSize().y());
		setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
	}
}


void GuiTextEditPopupKeyboard::onSizeChanged()
{
	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	mText->setSize(mSize.x() - 40, mText->getSize().y());

	float fullHeight = mTitle->getFont()->getHeight() + mText->getSize().y() + mKeyboardGrid->getSize().y() + mButtonGrid->getSize().y();

	// update grid
	mGrid.setRowHeightPerc(0, mTitle->getFont()->getHeight() / fullHeight);
	mGrid.setRowHeightPerc(1, mText->getSize().y() / fullHeight);
	mGrid.setRowHeightPerc(2, mKeyboardGrid->getSize().y() / fullHeight);
	mGrid.setRowHeightPerc(3, mButtonGrid->getSize().y() / fullHeight);

	mGrid.setSize(mSize);
}

bool GuiTextEditPopupKeyboard::input(InputConfig* config, Input input)
{
	if (GuiComponent::input(config, input))
		return true;

	// pressing back when not text editing closes us
	if (config->isMappedTo("b", input) && input.value)
	{
		delete this;
		return true;
	}

	// For deleting a chara (Left Top Button)
	if (config->isMappedTo("lefttop", input) && input.value) {
		mText->startEditing();
		mText->textInput("\b");
		mText->stopEditing();
	}

	// For Adding a space (Right Top Button)
	if (config->isMappedTo("righttop", input) && input.value) {
		mText->startEditing();
		mText->textInput(" ");
	}

	// For Shifting (X)
	if (config->isMappedTo("x", input) && input.value) {
		if (mShift) mShift = false;
		else mShift = true;
		shiftKeys();
	}

	

	return false;
}

void GuiTextEditPopupKeyboard::update(int deltatime) {

}

// Shifts the keys when user hits the shift button.
void GuiTextEditPopupKeyboard::shiftKeys() {
	if (mShift){
		mShiftButton->setColorShift(0x00000044);
		mSpecialButton->removeColorShift();	
		mSpecial = false;
	}else{
		mShiftButton->removeColorShift();
	}
	updateKeys();
}

// Special keys when user hits the shift button.
void GuiTextEditPopupKeyboard::specialKeys() {
	if (mSpecial){
		mSpecialButton->setColorShift(0x00000044);
		mShiftButton->removeColorShift();
		mShift = false;
	}else{
		mSpecialButton->removeColorShift();
	}
	updateKeys();
}

void GuiTextEditPopupKeyboard::updateKeys(){
	for (int y = 0; y < ROWS; y++) {
		for (int x = 0; x < COLUMNS; x++) {
			if (charArray[y][x] != "sft" && charArray[y][x] != "chr" && charArray[y][x] != "cul" && charArray[y][x] != "cur"){
				if (mSpecial){
					buttonList.at(y).at(x)->setText(charArraySpecial[y][x],charArraySpecial[y][x],false,minText);
				}else if(mShift){
					buttonList.at(y).at(x)->setText(charArrayUp[y][x],charArrayUp[y][x],false,minText);
				}else{
					buttonList.at(y).at(x)->setText(charArray[y][x],charArrayUp[y][x],false,minText);
				}
			}
		}
	}
}

std::vector<HelpPrompt> GuiTextEditPopupKeyboard::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts = mGrid.getHelpPrompts();
	prompts.push_back(HelpPrompt("x", "SHIFT"));
	prompts.push_back(HelpPrompt("b", "back"));
	prompts.push_back(HelpPrompt("r", "SPACE"));
	prompts.push_back(HelpPrompt("l", "DELETE"));
	return prompts;
}

