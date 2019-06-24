#pragma once
#ifndef ES_APP_GUIS_GUI_GAME_LIST_OPTIONS_H
#define ES_APP_GUIS_GUI_GAME_LIST_OPTIONS_H

#include "components/MenuComponent.h"
#include "components/OptionListComponent.h"
#include "scrapers/Scraper.h"
#include "FileData.h"
#include "GuiComponent.h"

class IGameListView;
class SystemData;

class GuiGamelistOptions : public GuiComponent
{
public:
	GuiGamelistOptions(Window* window, SystemData* system);
	virtual ~GuiGamelistOptions();

	void save();
	virtual bool input(InputConfig* config, Input input) override;
	virtual std::vector<HelpPrompt> getHelpPrompts() override;
	virtual HelpStyle getHelpStyle() override;
	inline void addSaveFunc(const std::function<void()>& func) { mSaveFuncs.push_back(func); };

private:
	void openGamelistFilter();
	void openMetaDataEd();
	void openScraper();
	void scrapeDone(const ScraperSearchResult& result);
	void startEditMode();
	void exitEditMode();
	void jumpToLetter();
	std::shared_ptr<ImageComponent> getIcon(const std::string& path);

	MenuComponent mMenu;

	typedef OptionListComponent<char> LetterList;
	std::shared_ptr<LetterList> mJumpToLetterList;

	typedef OptionListComponent<const FileData::SortType*> SortList;
	std::shared_ptr<SortList> mListSort;

	SystemData* mSystem;
	IGameListView* getGamelist();
	bool fromPlaceholder;
	bool mFiltersChanged;
	std::vector< std::function<void()> > mSaveFuncs;
};

#endif // ES_APP_GUIS_GUI_GAME_LIST_OPTIONS_H
