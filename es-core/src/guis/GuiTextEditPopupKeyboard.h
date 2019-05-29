#include "GuiComponent.h"

#include "components/NinePatchComponent.h"
#include "components/ButtonComponent.h"
#include "components/ComponentGrid.h"
#include "components/TextEditComponent.h"
#include "components/TextComponent.h"

class GuiTextEditPopupKeyboard : public GuiComponent
{
public:
	GuiTextEditPopupKeyboard(Window* window, const std::string& title, const std::string& initValue,
		const std::function<void(const std::string&)>& okCallback, bool multiLine, const char* acceptBtnText = "OK");

	bool input(InputConfig* config, Input input);
	void update(int deltatime) override;
	void onSizeChanged();
	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	void shiftKeys();
	void specialKeys();
	void updateKeys();

	NinePatchComponent mBackground;
	ComponentGrid mGrid;

	std::shared_ptr<TextComponent> mTitle;
	std::shared_ptr<TextEditComponent> mText;
	std::shared_ptr<ComponentGrid> mKeyboardGrid;
	std::shared_ptr<ComponentGrid> mButtonGrid;
	std::shared_ptr<ComponentGrid> mNewGrid;
	
	std::shared_ptr<ButtonComponent> mShiftButton;
	std::shared_ptr<ButtonComponent> mSpecialButton;
	
	std::vector< std::vector< std::shared_ptr<ButtonComponent> > > buttonList;

	const int ROWS = 4;
    const int COLUMNS = 11;

	// Define keyboard key rows.
	const char* charArray[4][11] = {
		{"1","2","3","4","5","6","7","8","9","0","ß"},
		{ "q","w","e","r","t","z","u","i","o","p","ü"},
		{ "a","s","d","f","g","h","j","k","l","ö","ä"},
		{ "SHIFT","#+=","y","x","c","v","b","n","m",",","."}
	};
	const char* charArrayUp[4][11] = {
		{"1","2","3","4","5","6","7","8","9","0","ß"},
		{ "Q","W","E","R","T","Z","U","I","O","P","Ü"},
		{ "A","S","D","F","G","H","J","K","L","Ö","Ä"},
		{ "SHIFT","#+=","Y","X","C","V","B","N","M",";",":"}
	};
	const char* charArraySpecial[4][11] = {
		{ "+","×","÷","=","%","/","\\","$","€","£","@"},
		{ "*","!","#",":",";","&","_","(",")","-","'"},
		{ "\"",",",".","?","<",">","[","]","{","}","~"},
		{ "SHIFT","#+=","§","|","^","°","©","¿","¢","¬","¦"}
	};
	
	
	std::string charList = "abcdefghijklmnopqrstuvwxyzüöä";
	std::string minText = "SHIFT";

	int mxIndex = 0;		// Stores the X index and makes every grid the same.

	bool mMultiLine;
	bool mShift = false;
	bool mShiftChange = false;
	bool mSpecial = false;
	bool mSpecialChange = false;
};
