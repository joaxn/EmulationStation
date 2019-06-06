#include "components/DynamicTextComponent.h"

#include "utils/StringUtil.h"
#include "Log.h"
#include "Renderer.h"
#include "Settings.h"

DynamicTextComponent::DynamicTextComponent(Window* window) : GuiComponent(window), 
	mFont(Font::get(FONT_SIZE_MEDIUM)), mUppercase(false), mColor(0x000000FF), mAutoCalcExtent(true, true),
	mHorizontalAlignment(ALIGN_LEFT), mVerticalAlignment(ALIGN_CENTER), mLineSpacing(1.5f), mBgColor(0),
	mRenderBackground(false)
{
}

DynamicTextComponent::DynamicTextComponent(Window* window, const std::string& text, const std::shared_ptr<Font>& font, unsigned int color, Alignment align,
	Vector3f pos, Vector2f size, unsigned int bgcolor) : GuiComponent(window), 
	mFont(NULL), mUppercase(false), mColor(0x000000FF), mAutoCalcExtent(true, true),
	mHorizontalAlignment(align), mVerticalAlignment(ALIGN_CENTER), mLineSpacing(1.5f), mBgColor(0),
	mRenderBackground(false)
{
	setFont(font);
	setColor(color);
	setBackgroundColor(bgcolor);
	setText(text);
	setPosition(pos);
	setSize(size);
}

void DynamicTextComponent::onSizeChanged()
{
	mAutoCalcExtent = Vector2i((getSize().x() == 0), (getSize().y() == 0));
	onTextChanged();
}

void DynamicTextComponent::setFont(const std::shared_ptr<Font>& font)
{
	mFont = font;
	onTextChanged();
}

//  Set the color of the font/text
void DynamicTextComponent::setColor(unsigned int color)
{
	mColor = color;
	mColorOpacity = mColor & 0x000000FF;
	onColorChanged();
}

//  Set the color of the background box
void DynamicTextComponent::setBackgroundColor(unsigned int color)
{
	mBgColor = color;
	mBgColorOpacity = mBgColor & 0x000000FF;
}

void DynamicTextComponent::setRenderBackground(bool render)
{
	mRenderBackground = render;
}

//  Scale the opacity
void DynamicTextComponent::setOpacity(unsigned char opacity)
{
	// This method is mostly called to do fading in-out of the Text component element.
	// Therefore, we assume here that opacity is a fractional value (expressed as an int 0-255),
	// of the opacity originally set with setColor() or setBackgroundColor().

	unsigned char o = (unsigned char)((float)opacity / 255.f * (float) mColorOpacity);
	mColor = (mColor & 0xFFFFFF00) | (unsigned char) o;

	unsigned char bgo = (unsigned char)((float)opacity / 255.f * (float)mBgColorOpacity);
	mBgColor = (mBgColor & 0xFFFFFF00) | (unsigned char)bgo;

	onColorChanged();

	GuiComponent::setOpacity(opacity);
}

unsigned char DynamicTextComponent::getOpacity() const
{
	return mColor & 0x000000FF;
}

void DynamicTextComponent::setText(const std::string& text)
{
	mText = text;
	onTextChanged();
}

void DynamicTextComponent::setUppercase(bool uppercase)
{
	mUppercase = uppercase;
	onTextChanged();
}

void DynamicTextComponent::render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	if (mRenderBackground)
	{
		Renderer::setMatrix(trans);
		Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), mBgColor);
	}

	if(mTextCache)
	{
		const Vector2f& textSize = mTextCache->metrics.size;
		float yOff = 0;
		switch(mVerticalAlignment)
		{
			case ALIGN_TOP:
				yOff = 0;
				break;
			case ALIGN_BOTTOM:
				yOff = (getSize().y() - textSize.y());
				break;
			case ALIGN_CENTER:
				yOff = (getSize().y() - textSize.y()) / 2.0f;
				break;
		}
		Vector3f off(0, yOff, 0);

		if(Settings::getInstance()->getBool("DebugText"))
		{
			// draw the "textbox" area, what we are aligned within
			Renderer::setMatrix(trans);
			Renderer::drawRect(0.f, 0.f, mSize.x(), mSize.y(), 0xFF000033);
		}

		trans.translate(off);
		trans.round();
		Renderer::setMatrix(trans);

		// draw the text area, where the text actually is going
		if(Settings::getInstance()->getBool("DebugText"))
		{
			switch(mHorizontalAlignment)
			{
			case ALIGN_LEFT:
				Renderer::drawRect(0.0f, 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
			case ALIGN_CENTER:
				Renderer::drawRect((mSize.x() - mTextCache->metrics.size.x()) / 2.0f, 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
			case ALIGN_RIGHT:
				Renderer::drawRect(mSize.x() - mTextCache->metrics.size.x(), 0.0f, mTextCache->metrics.size.x(), mTextCache->metrics.size.y(), 0x00000033);
				break;
			}
		}
		mFont->renderTextCache(mTextCache.get());
	}
}

void DynamicTextComponent::calculateExtent()
{
	if(mAutoCalcExtent.x())
	{
		mSize = mFont->sizeText(mUppercase ? Utils::String::toUpper(mText) : mText, mLineSpacing);
	}else{
		if(mAutoCalcExtent.y())
		{
			mSize[1] = mFont->sizeWrappedText(mUppercase ? Utils::String::toUpper(mText) : mText, getSize().x(), mLineSpacing).y();
		}
	}
}

void DynamicTextComponent::onTextChanged()
{
	calculateExtent();

	if(!mFont || mText.empty())
	{
		mTextCache.reset();
		return;
	}

	std::string text = mUppercase ? Utils::String::toUpper(mText) : mText;

	std::shared_ptr<Font> f = mFont;
	const bool isMultiline = (mSize.y() == 0 || mSize.y() > f->getHeight()*1.2f);

	bool addAbbrev = false;
	if(!isMultiline)
	{
		size_t newline = text.find('\n');
		text = text.substr(0, newline); // single line of text - stop at the first newline since it'll mess everything up
		addAbbrev = newline != std::string::npos;
	}

	Vector2f size = f->sizeText(text);
	if(!isMultiline && mSize.x() && text.size() && (size.x() > mSize.x() || addAbbrev))
	{
		// abbreviate text
		const std::string abbrev = "...";
		Vector2f abbrevSize = f->sizeText(abbrev);

		while(text.size() && size.x() + abbrevSize.x() > mSize.x())
		{
			size_t newSize = Utils::String::prevCursor(text, text.size());
			text.erase(newSize, text.size() - newSize);
			size = f->sizeText(text);
		}

		text.append(abbrev);

		mTextCache = std::shared_ptr<TextCache>(f->buildTextCache(text, Vector2f(0, 0), (mColor >> 8 << 8) | mOpacity, mSize.x(), mHorizontalAlignment, mLineSpacing));
	}else{
		mTextCache = std::shared_ptr<TextCache>(f->buildTextCache(f->wrapText(text, mSize.x()), Vector2f(0, 0), (mColor >> 8 << 8) | mOpacity, mSize.x(), mHorizontalAlignment, mLineSpacing));
	}
}

void DynamicTextComponent::onColorChanged()
{
	if(mTextCache)
	{
		mTextCache->setColor(mColor);
	}
}

void DynamicTextComponent::setHorizontalAlignment(Alignment align)
{
	mHorizontalAlignment = align;
	onTextChanged();
}

void DynamicTextComponent::setVerticalAlignment(Alignment align)
{
	mVerticalAlignment = align;
}

void DynamicTextComponent::setLineSpacing(float spacing)
{
	mLineSpacing = spacing;
	onTextChanged();
}

void DynamicTextComponent::setValue(const std::string& value)
{
	setText(value);
}

std::string DynamicTextComponent::getValue() const
{
	return mText;
}

void TextEditComponent::update(int deltaTime)
{
	GuiComponent::update(deltaTime);
}

void DynamicTextComponent::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties)
{
	GuiComponent::applyTheme(theme, view, element, properties);

	using namespace ThemeFlags;

	const ThemeData::ThemeElement* elem = theme->getElement(view, element, "text");
	if(!elem)
		return;

	if (properties & COLOR && elem->has("color"))
		setColor(elem->get<unsigned int>("color"));	

	setRenderBackground(false);
	if (properties & COLOR && elem->has("backgroundColor")) {
		setBackgroundColor(elem->get<unsigned int>("backgroundColor"));
		setRenderBackground(true);
	}

	if(properties & ALIGNMENT && elem->has("alignment"))
	{
		std::string str = elem->get<std::string>("alignment");
		if(str == "left")
			setHorizontalAlignment(ALIGN_LEFT);
		else if(str == "center")
			setHorizontalAlignment(ALIGN_CENTER);
		else if(str == "right")
			setHorizontalAlignment(ALIGN_RIGHT);
		else
			LOG(LogError) << "Unknown text alignment string: " << str;
	}

	if(properties & TEXT && elem->has("text"))
		setText(elem->get<std::string>("text"));

	if(properties & FORCE_UPPERCASE && elem->has("forceUppercase"))
		setUppercase(elem->get<bool>("forceUppercase"));

	if(properties & LINE_SPACING && elem->has("lineSpacing"))
		setLineSpacing(elem->get<float>("lineSpacing"));

	setFont(Font::getFromTheme(elem, properties, mFont));
}
