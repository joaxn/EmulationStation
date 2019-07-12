#include "guis/GuiMetaDataEd.h"

#include "components/ButtonComponent.h"
#include "components/ComponentList.h"
#include "components/DateTimeEditComponent.h"
#include "components/MenuComponent.h"
#include "components/RatingComponent.h"
#include "components/SwitchComponent.h"
#include "components/TextComponent.h"
#include "components/SliderComponent.h"
#include "guis/GuiGameScraper.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiTextEditPopup.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "resources/Font.h"
#include "utils/StringUtil.h"
#include "views/ViewController.h"
#include "CollectionSystemManager.h"
#include "FileData.h"
#include "FileFilterIndex.h"
#include "SystemData.h"
#include "Window.h"

GuiMetaDataEd::GuiMetaDataEd(Window* window, MetaDataList* md, const std::vector<MetaDataDecl>& mdd, ScraperSearchParams scraperParams, const std::string& /*header*/, std::function<void()> saveCallback) : GuiComponent(window),
	mScraperParams(scraperParams),

	mMenu(window, "EDIT GAME INFO"),
	mMetaDataDecl(mdd),
	mMetaData(md),
	mSavedCallback(saveCallback)
{
	addChild(&mMenu);
	
	mMenu.setSubtitle(Utils::FileSystem::getFileNameNoExt(scraperParams.game->getPath()).c_str());

	// populate list
	for(auto iter = mdd.cbegin(); iter != mdd.cend(); iter++)
	{
		std::shared_ptr<GuiComponent> ed;

		// don't add statistics
		if(iter->isStatistic)
			continue;

		// create ed and add it (and any related components) to mMenu
		// ed's value will be set below
		ComponentListRow row;
		auto lbl = std::make_shared<TextComponent>(mWindow, Utils::String::toUpper(iter->displayName), Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
		row.addElement(lbl, true); // label

		switch(iter->type)
		{
		case MD_BOOL:
			{
				ed = std::make_shared<SwitchComponent>(window);
				row.addElement(ed, false, true);
				break;
			}
		case MD_RATING:
			{
				ed = std::make_shared<RatingComponent>(window);
				const float height = lbl->getSize().y() * 0.71f;
				ed->setSize(0, height);
				row.addElement(ed, false, true);

				auto spacer = std::make_shared<GuiComponent>(mWindow);
				spacer->setSize(Renderer::getScreenWidth() * 0.0025f, 0);
				row.addElement(spacer, false);

				// pass input to the actual RatingComponent instead of the spacer
				row.input_handler = std::bind(&GuiComponent::input, ed.get(), std::placeholders::_1, std::placeholders::_2);

				break;
			}
		case MD_INT:
			{
				ed = std::make_shared<SliderComponent>(window, 0.f, 4.f, 1.f, "", true);
				row.addElement(ed, false, true);
				break;
			}
		case MD_DATE:
			{
				ed = std::make_shared<DateTimeEditComponent>(window);
				row.addElement(ed, false);

				auto spacer = std::make_shared<GuiComponent>(mWindow);
				spacer->setSize(Renderer::getScreenWidth() * 0.0025f, 0);
				row.addElement(spacer, false);

				// pass input to the actual DateTimeEditComponent instead of the spacer
				row.input_handler = std::bind(&GuiComponent::input, ed.get(), std::placeholders::_1, std::placeholders::_2);

				break;
			}
		case MD_TIME:
			{
				ed = std::make_shared<DateTimeEditComponent>(window, DateTimeEditComponent::DISP_RELATIVE_TO_NOW);
				row.addElement(ed, false);
				break;
			}
		case MD_MULTILINE_STRING:
		default:
			{
				// MD_STRING
				ed = std::make_shared<TextComponent>(window, "", Font::get(FONT_SIZE_MEDIUM, FONT_PATH_LIGHT), 0x777777FF, ALIGN_RIGHT);
				row.addElement(ed, true);

				auto spacer = std::make_shared<GuiComponent>(mWindow);
				spacer->setSize(Renderer::getScreenWidth() * 0.005f, 0);
				row.addElement(spacer, false);

				auto bracket = std::make_shared<ImageComponent>(mWindow);
				bracket->setImage(":/arrow.svg");
				bracket->setResize(Vector2f(0, lbl->getFont()->getLetterHeight()));
				row.addElement(bracket, false);

				bool multiLine = iter->type == MD_MULTILINE_STRING;
				const std::string title = iter->displayPrompt;
				auto updateVal = [ed](const std::string& newVal) { ed->setValue(newVal); }; // ok callback (apply new value to ed)
				row.makeAcceptInputHandler([this, title, ed, updateVal, multiLine] {
					mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, title, ed->getValue(), updateVal, multiLine));
				});
				break;
			}
		}

		assert(ed);
		mMenu.addRow(row);
		ed->setValue(mMetaData->get(iter->key));
		mEditors.push_back(ed);
	}

	mMenu.addButton("CANCEL", "cancel", [&] { delete this; });
	mMenu.addButton("SAVE", "save", [&] { save(); delete this; });
	
	//center
	setSize((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());
	mMenu.setPosition((mSize.x() - mMenu.getSize().x()) / 2, (mSize.y() - mMenu.getSize().y()) / 2);
}

void GuiMetaDataEd::save()
{
	// remove game from index
	mScraperParams.system->getIndex()->removeFromIndex(mScraperParams.game);

	for(unsigned int i = 0; i < mEditors.size(); i++)
	{
		if(mMetaDataDecl.at(i).isStatistic)
			continue;
		LOG(LogInfo) << "Meta set " << mMetaDataDecl.at(i).key << " = " << mEditors.at(i)->getValue();
		mMetaData->set(mMetaDataDecl.at(i).key, mEditors.at(i)->getValue());
	}

	// enter game in index
	mScraperParams.system->getIndex()->addToIndex(mScraperParams.game);

	if(mSavedCallback)
		mSavedCallback();

	// update respective Collection Entries
	CollectionSystemManager::get()->refreshCollectionSystems(mScraperParams.game);
}

void GuiMetaDataEd::fetch()
{
	GuiGameScraper* scr = new GuiGameScraper(mWindow, mScraperParams, std::bind(&GuiMetaDataEd::fetchDone, this, std::placeholders::_1));
	mWindow->pushGui(scr);
}

void GuiMetaDataEd::fetchDone(const ScraperSearchResult& result)
{
	for(unsigned int i = 0; i < mEditors.size(); i++)
	{
		if(mMetaDataDecl.at(i).isStatistic)
			continue;

		const std::string& key = mMetaDataDecl.at(i).key;
		mEditors.at(i)->setValue(result.mdl.get(key));
	}
}

void GuiMetaDataEd::close(bool closeAllWindows)
{
	// find out if the user made any changes
	bool dirty = false;
	for(unsigned int i = 0; i < mEditors.size(); i++)
	{
		const std::string& key = mMetaDataDecl.at(i).key;
		LOG(LogInfo) << "Meta close check " << key << " == " << mEditors.at(i)->getValue();
		if(mMetaData->get(key) != mEditors.at(i)->getValue())
		{
			dirty = true;
			break;
		}
	}

	std::function<void()> closeFunc;
	if(!closeAllWindows)
	{
		closeFunc = [this] { delete this; };
	}else{
		Window* window = mWindow;
		closeFunc = [window, this] {
			while(window->peekGui() != ViewController::get())
				delete window->peekGui();
		};
	}


	if(dirty)
	{
		// changes were made, ask if the user wants to save them
		mWindow->pushGui(new GuiMsgBox(mWindow,
			"SAVE CHANGES?",
			"YES", [this, closeFunc] { save(); closeFunc(); },
			"NO", closeFunc
		));
	}else{
		closeFunc();
	}
}

bool GuiMetaDataEd::input(InputConfig* config, Input input)
{
	if(GuiComponent::input(config, input))
		return true;

	const bool isStart = config->isMappedTo("start", input);
	if(input.value != 0 && (config->isMappedTo("b", input) || isStart))
	{
		close(isStart);
		return true;
	}

	return false;
}

std::vector<HelpPrompt> GuiMetaDataEd::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();
	prompts.push_back(HelpPrompt("b", "back"));
	prompts.push_back(HelpPrompt("start", "close"));
	return prompts;
}
