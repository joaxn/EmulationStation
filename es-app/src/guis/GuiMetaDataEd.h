#pragma once
#ifndef ES_APP_GUIS_GUI_META_DATA_ED_H
#define ES_APP_GUIS_GUI_META_DATA_ED_H

#include "components/MenuComponent.h"
#include "components/ComponentGrid.h"
#include "scrapers/Scraper.h"
#include "GuiComponent.h"
#include "MetaData.h"

class ComponentList;
class TextComponent;

class GuiMetaDataEd : public GuiComponent
{
public:
	GuiMetaDataEd(Window* window, MetaDataList* md, const std::vector<MetaDataDecl>& mdd, ScraperSearchParams params, const std::string& header, std::function<void()> savedCallback);
	
	bool input(InputConfig* config, Input input) override;
	void onSizeChanged() override;
	virtual std::vector<HelpPrompt> getHelpPrompts() override;

private:
	void save();
	void fetch();
	void fetchDone(const ScraperSearchResult& result);
	void close(bool closeAllWindows);
	
	MenuComponent mMenu;

	ScraperSearchParams mScraperParams;

	std::vector< std::shared_ptr<GuiComponent> > mEditors;

	std::vector<MetaDataDecl> mMetaDataDecl;
	MetaDataList* mMetaData;
	std::function<void()> mSavedCallback;
	std::function<void()> mDeleteFunc;
};

#endif // ES_APP_GUIS_GUI_META_DATA_ED_H
