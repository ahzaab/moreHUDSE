#include "PCH.h"
#include "CLIK/TextField.h"


namespace CLIK
{
	TextField::TextField() :
		super()
	{}


	TextField::TextField(const TextField& a_rhs) :
		super(a_rhs)
	{}


	TextField::TextField(TextField&& a_rhs) :
		super(std::move(a_rhs))
	{}


	TextField::TextField(const Object& a_rhs) :
		super(a_rhs)
	{}


	TextField::TextField(Object&& a_rhs) :
		super(std::move(a_rhs))
	{}


	TextField::TextField(const RE::GFxValue& a_val) :
		super(a_val)
	{}


	TextField::TextField(RE::GFxValue&& a_val) :
		super(std::move(a_val))
	{}


	TextField::~TextField()
	{}


	TextField& TextField::operator=(const TextField& a_rhs)
	{
		super::operator=(a_rhs);
		return *this;
	}


	TextField& TextField::operator=(TextField&& a_rhs)
	{
		super::operator=(std::move(a_rhs));
		return *this;
	}


	TextField& TextField::operator=(const Object& a_rhs)
	{
		super::operator=(a_rhs);
		return *this;
	}


	TextField& TextField::operator=(Object&& a_rhs)
	{
		super::operator=(std::move(a_rhs));
		return *this;
	}


	TextField& TextField::operator=(const RE::GFxValue& a_rhs)
	{
		_instance = a_rhs;
		return *this;
	}


	TextField& TextField::operator=(RE::GFxValue&& a_rhs)
	{
		_instance = std::move(a_rhs);
		return *this;
	}


	double TextField::Alpha() const
	{
		return GetNumber("_alpha");
	}


	void TextField::Alpha(double a_alpha)
	{
		SetNumber("_alpha", a_alpha);
	}


	std::string_view TextField::AntiAliasType() const
	{
		return GetString("antiAliasType");
	}


	void TextField::AntiAliasType(std::string_view a_antiAliasType)
	{
		SetString("antiAliasType", a_antiAliasType);
	}


	Object TextField::AutoSize() const
	{
		return GetObject("autoSize");
	}


	void TextField::AutoSize(const Object& a_autoSize)
	{
		SetObject("autoSize", a_autoSize);
	}


	bool TextField::Background() const
	{
		return GetBoolean("background");
	}


	void TextField::Background(bool a_background)
	{
		SetBoolean("background", a_background);
	}


	double TextField::BackgroundColor() const
	{
		return GetNumber("backgroundColor");
	}


	void TextField::BackgroundColor(double a_backgroundColor)
	{
		SetNumber("backgroundColor", a_backgroundColor);
	}


	bool TextField::Border() const
	{
		return GetBoolean("border");
	}


	void TextField::Border(bool a_border)
	{
		SetBoolean("border", a_border);
	}


	double TextField::BorderColor() const
	{
		return GetNumber("borderColor");
	}


	void TextField::BorderColor(double a_borderColor)
	{
		SetNumber("borderColor", a_borderColor);
	}


	double TextField::BottomScroll() const
	{
		return GetNumber("bottomScroll");
	}


	bool TextField::CondenseWhite() const
	{
		return GetBoolean("condenseWhite");
	}


	void TextField::CondenseWhite(bool a_condenseWhite)
	{
		SetBoolean("condenseWhite", a_condenseWhite);
	}


	bool TextField::EmbedFonts() const
	{
		return GetBoolean("embedFonts");
	}


	void TextField::EmbedFonts(bool a_embedFonts)
	{
		SetBoolean("embedFonts", a_embedFonts);
	}


	std::string_view TextField::GridFitType() const
	{
		return GetString("gridFitType");
	}


	void TextField::GridFitType(std::string_view a_gridFitType)
	{
		SetString("gridFitType", a_gridFitType);
	}


	double TextField::Height() const
	{
		return GetNumber("_height");
	}


	void TextField::Height(double a_height)
	{
		SetNumber("_height", a_height);
	}


	double TextField::HighQuality() const
	{
		return GetNumber("_highquality");
	}


	void TextField::HighQuality(double a_highQuality)
	{
		SetNumber("_highquality", a_highQuality);
	}


	double TextField::HScroll() const
	{
		return GetNumber("hscroll");
	}


	void TextField::HScroll(double a_hscroll)
	{
		SetNumber("hscroll", a_hscroll);
	}


	bool TextField::HTML() const
	{
		return GetBoolean("html");
	}


	void TextField::HTML(bool a_html)
	{
		SetBoolean("html", a_html);
	}


	std::string_view TextField::HTMLText() const
	{
		return GetString("htmlText");
	}


	void TextField::HTMLText(std::string_view a_htmlText)
	{
		SetString("htmlText", a_htmlText);
	}


	double TextField::Length() const
	{
		return GetNumber("length");
	}


	double TextField::MaxChars() const
	{
		return GetNumber("maxChars");
	}


	void TextField::MaxChars(double a_maxChars)
	{
		SetNumber("maxChars", a_maxChars);
	}


	double TextField::MaxHScroll() const
	{
		return GetNumber("maxhscroll");
	}


	double TextField::MaxScroll() const
	{
		return GetNumber("maxscroll");
	}


	bool TextField::MouseWheelEnabled() const
	{
		return GetBoolean("mouseWheelEnabled");
	}


	void TextField::MouseWheelEnabled(bool a_mouseWheelEnabled)
	{
		SetBoolean("mouseWheelEnabled", a_mouseWheelEnabled);
	}


	bool TextField::Multiline() const
	{
		return GetBoolean("multiline");
	}


	void TextField::Multiline(bool a_multiline)
	{
		SetBoolean("multiline", a_multiline);
	}


	std::string_view TextField::Name() const
	{
		return GetString("_name");
	}


	void TextField::Name(std::string_view a_name)
	{
		SetString("_name", a_name);
	}


	bool TextField::Password() const
	{
		return GetBoolean("password");
	}


	void TextField::Password(bool a_password)
	{
		SetBoolean("password", a_password);
	}


	std::string_view TextField::Quality() const
	{
		return GetString("_quality");
	}


	void TextField::Quality(std::string_view a_quality)
	{
		SetString("_quality", a_quality);
	}


	std::string_view TextField::Restrict() const
	{
		return GetString("restrict");
	}


	void TextField::Restrict(std::string_view a_restrict)
	{
		SetString("restrict", a_restrict);
	}


	double TextField::Rotation() const
	{
		return GetNumber("_rotation");
	}


	void TextField::Rotation(double a_rotation)
	{
		SetNumber("_rotation", a_rotation);
	}


	double TextField::Scroll() const
	{
		return GetNumber("scroll");
	}


	void TextField::Scroll(double a_scroll)
	{
		SetNumber("scroll", a_scroll);
	}


	bool TextField::Selectable() const
	{
		return GetBoolean("selectable");
	}


	void TextField::Selectable(bool a_selectable)
	{
		SetBoolean("selectable", a_selectable);
	}


	double TextField::Sharpness() const
	{
		return GetNumber("sharpness");
	}


	void TextField::Sharpness(double a_sharpness)
	{
		SetNumber("sharpness", a_sharpness);
	}


	double TextField::SoundBufTime() const
	{
		return GetNumber("_soundbuftime");
	}


	void TextField::SoundBufTime(double a_soundBufTime)
	{
		SetNumber("_soundbuftime", a_soundBufTime);
	}


	bool TextField::TabEnabled() const
	{
		return GetBoolean("tabEnabled");
	}


	void TextField::TabEnabled(bool a_tabEnabled)
	{
		SetBoolean("tabEnabled", a_tabEnabled);
	}


	double TextField::TabIndex() const
	{
		return GetNumber("tabIndex");
	}


	void TextField::TabIndex(double a_tabIndex)
	{
		SetNumber("tabIndex", a_tabIndex);
	}


	std::string_view TextField::Target() const
	{
		return GetString("_target");
	}


	std::string_view TextField::Text() const
	{
		return GetString("text");
	}


	void TextField::Text(std::string_view a_text)
	{
		SetString("text", a_text);
	}


	double TextField::TextColor() const
	{
		return GetNumber("textColor");
	}


	void TextField::TextColor(double a_textColor)
	{
		SetNumber("textColor", a_textColor);
	}


	double TextField::TextHeight() const
	{
		return GetNumber("textHeight");
	}


	void TextField::TextHeight(double a_textHeight)
	{
		SetNumber("textHeight", a_textHeight);
	}


	double TextField::TextWidth() const
	{
		return GetNumber("textWidth");
	}


	void TextField::TextWidth(double a_textWidth)
	{
		SetNumber("textWidth", a_textWidth);
	}


	double TextField::Thickness() const
	{
		return GetNumber("thickness");
	}


	void TextField::Thickness(double a_thickness)
	{
		SetNumber("thickness", a_thickness);
	}


	std::string_view TextField::Type() const
	{
		return GetString("type");
	}


	void TextField::Type(std::string_view a_type)
	{
		SetString("type", a_type);
	}


	std::string_view TextField::URL() const
	{
		return GetString("_url");
	}


	std::string_view TextField::Variable() const
	{
		return GetString("variable");
	}


	void TextField::Variable(std::string_view a_variable)
	{
		SetString("variable", a_variable);
	}


	bool TextField::Visible() const
	{
		return GetBoolean("_visible");
	}


	void TextField::Visible(bool a_visible)
	{
		SetBoolean("_visible", a_visible);
	}


	double TextField::Width() const
	{
		return GetNumber("_width");
	}


	void TextField::Width(double a_width)
	{
		SetNumber("_width", a_width);
	}


	bool TextField::WordWrap() const
	{
		return GetBoolean("wordWrap");
	}


	void TextField::WordWrap(bool a_wordWrap)
	{
		SetBoolean("wordWrap", a_wordWrap);
	}


	double TextField::X() const
	{
		return GetNumber("_x");
	}


	void TextField::X(double a_x)
	{
		SetNumber("_x", a_x);
	}


	double TextField::XMouse() const
	{
		return GetNumber("_xmouse");
	}


	double TextField::XScale() const
	{
		return GetNumber("_xscale");
	}


	void TextField::XScale(double a_xScale)
	{
		SetNumber("_xscale", a_xScale);
	}


	double TextField::Y() const
	{
		return GetNumber("_y");
	}


	void TextField::Y(double a_y)
	{
		SetNumber("_y", a_y);
	}


	double TextField::YMouse() const
	{
		return GetNumber("_ymouse");
	}


	double TextField::YScale() const
	{
		return GetNumber("_yscale");
	}


	void TextField::YScale(double a_yScale)
	{
		SetNumber("_yscale", a_yScale);
	}


	bool TextField::AddListener(Object& a_listener)
	{
		enum
		{
			kListener,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kListener] = a_listener.GetInstance();
		assert(args[kListener].IsObject());

		RE::GFxValue		  boolean;
		[[maybe_unused]] auto success = _instance.Invoke("addListener", &boolean, args, kNumArgs);
		assert(success);

		return boolean.GetBool();
	}


	double TextField::GetDepth()
	{
		RE::GFxValue		  number;
		[[maybe_unused]] auto success = _instance.Invoke("getDepth", &number);
		assert(success);

		return number.GetNumber();
	}


	bool TextField::RemoveListener(Object& a_listener)
	{
		enum
		{
			kListener,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kListener] = a_listener.GetInstance();
		assert(args[kListener].IsObject());

		RE::GFxValue		  boolean;
		[[maybe_unused]] auto success = _instance.Invoke("removeListener", &boolean, args, kNumArgs);
		assert(success);

		return boolean.GetBool();
	}


	void TextField::RemoveTextField()
	{
		[[maybe_unused]] auto success = _instance.Invoke("removeTextField");
		assert(success);
	}


	void TextField::ReplaceSel(std::string_view a_newText)
	{
		enum
		{
			kNewText,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kNewText] = a_newText;
		assert(args[kNewText].IsString());

		[[maybe_unused]] auto success = _instance.Invoke("replaceSel", 0, args, kNumArgs);
		assert(success);
	}


	void TextField::ReplaceText(double a_beginIndex, double a_endIndex, std::string_view a_newText)
	{
		enum
		{
			kBeginIndex,
			kEndIndex,
			kNewText,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kBeginIndex] = a_beginIndex;
		assert(args[kBeginIndex].IsNumber());

		args[kEndIndex] = a_endIndex;
		assert(args[kEndIndex].IsNumber());

		args[kNewText] = a_newText;
		assert(args[kNewText].IsString());

		[[maybe_unused]] auto success = _instance.Invoke("replaceText", 0, args, kNumArgs);
		assert(success);
	}


	bool TextField::AutoFit() const
	{
		return GetBoolean("autoFit");
	}


	void TextField::AutoFit(bool a_autoFit)
	{
		SetBoolean("autoFit", a_autoFit);
	}


	double TextField::CaretIndex() const
	{
		return GetNumber("caretIndex");
	}


	void TextField::CaretIndex(double a_caretIndex)
	{
		SetNumber("caretIndex", a_caretIndex);
	}


	double TextField::FocusGroup() const
	{
		return GetNumber("focusGroup");
	}


	void TextField::FocusGroup(double a_focusGroup)
	{
		SetNumber("focusGroup", a_focusGroup);
	}


	bool TextField::HitTestDisable() const
	{
		return GetBoolean("hitTestDisable");
	}


	void TextField::HitTestDisable(bool a_hitTestDisable)
	{
		SetBoolean("hitTestDisable", a_hitTestDisable);
	}


	bool TextField::NoTranslate() const
	{
		return GetBoolean("noTranslate");
	}


	void TextField::NoTranslate(bool a_noTranslate)
	{
		SetBoolean("noTranslate", a_noTranslate);
	}


	double TextField::NumLines() const
	{
		return GetNumber("numLines");
	}


	void TextField::NumLines(double a_numLines)
	{
		SetNumber("numLines", a_numLines);
	}


	bool TextField::TopmostLevel() const
	{
		return GetBoolean("topmostLevel");
	}


	void TextField::TopmostLevel(bool a_topmostLevel)
	{
		SetBoolean("topmostLevel", a_topmostLevel);
	}


	double TextField::InactiveSelectionBkgColor() const
	{
		return GetNumber("inactiveSelectionBkgColor");
	}


	void TextField::InactiveSelectionBkgColor(double a_inactiveSelectionBkgColor)
	{
		SetNumber("inactiveSelectionBkgColor", a_inactiveSelectionBkgColor);
	}


	bool TextField::AlwaysShowSelection() const
	{
		return GetBoolean("alwaysShowSelection");
	}


	void TextField::AlwaysShowSelection(bool a_alwaysShowSelection)
	{
		SetBoolean("alwaysShowSelection", a_alwaysShowSelection);
	}


	bool TextField::NoAutoSelection() const
	{
		return GetBoolean("noAutoSelection");
	}


	void TextField::NoAutoSelection(bool a_noAutoSelection)
	{
		SetBoolean("noAutoSelection", a_noAutoSelection);
	}


	double TextField::SelectionBeginIndex() const
	{
		return GetNumber("selectionBeginIndex");
	}


	void TextField::SelectionBeginIndex(double a_selectionBeginIndex)
	{
		SetNumber("selectionBeginIndex", a_selectionBeginIndex);
	}


	double TextField::SelectionEndIndex() const
	{
		return GetNumber("selectionEndIndex");
	}


	void TextField::SelectionEndIndex(double a_selectionEndIndex)
	{
		SetNumber("selectionEndIndex", a_selectionEndIndex);
	}


	double TextField::SelectionBkgColor() const
	{
		return GetNumber("selectionBkgColor");
	}


	void TextField::SelectionBkgColor(double a_selectionBkgColor)
	{
		SetNumber("selectionBkgColor", a_selectionBkgColor);
	}


	double TextField::SelectionTextColor() const
	{
		return GetNumber("selectionTextColor");
	}


	void TextField::SelectionTextColor(double a_selectionTextColor)
	{
		SetNumber("selectionTextColor", a_selectionTextColor);
	}


	bool TextField::UseRichTextClipboard() const
	{
		return GetBoolean("useRichTextClipboard");
	}


	void TextField::UseRichTextClipboard(bool a_useRichTextClipboard)
	{
		SetBoolean("useRichTextClipboard", a_useRichTextClipboard);
	}


	double TextField::InactiveSelectionTextColor() const
	{
		return GetNumber("inactiveSelectionTextColor");
	}


	void TextField::InactiveSelectionTextColor(double a_inactiveSelectionTextColor)
	{
		SetNumber("inactiveSelectionTextColor", a_inactiveSelectionTextColor);
	}


	double TextField::FontScaleFactor() const
	{
		return GetNumber("fontScaleFactor");
	}


	void TextField::FontScaleFactor(double a_fontScaleFactor)
	{
		SetNumber("fontScaleFactor", a_fontScaleFactor);
	}


	std::string_view TextField::TextAutoSize() const
	{
		return GetString("textAutoSize");
	}


	void TextField::TextAutoSize(std::string_view a_textAutoSize)
	{
		SetString("textAutoSize", a_textAutoSize);
	}


	std::string_view TextField::VerticalAlign() const
	{
		return GetString("verticalAlign");
	}


	void TextField::VerticalAlign(std::string_view a_verticalAlign)
	{
		SetString("verticalAlign", a_verticalAlign);
	}


	std::string_view TextField::VerticalAutoSize() const
	{
		return GetString("verticalAutoSize");
	}


	void TextField::VerticalAutoSize(std::string_view a_verticalAutoSize)
	{
		SetString("verticalAutoSize", a_verticalAutoSize);
	}


	void TextField::AppendText(std::string_view a_newText)
	{
		enum
		{
			kNewText,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kNewText] = a_newText;
		assert(args[kNewText].IsString());

		[[maybe_unused]] auto success = _instance.Invoke("appendText", 0, args, kNumArgs);
		assert(success);
	}


	void TextField::AppendHtml(std::string_view a_newHtml)
	{
		enum
		{
			kNewHtml,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kNewHtml] = a_newHtml;
		assert(args[kNewHtml].IsString());

		[[maybe_unused]] auto success = _instance.Invoke("appendHtml", 0, args, kNumArgs);
		assert(success);
	}


	double TextField::GetCharIndexAtPoint(double a_x, double a_y)
	{
		enum
		{
			kX,
			kY,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kX] = a_x;
		assert(args[kX].IsNumber());

		args[kY] = a_y;
		assert(args[kY].IsNumber());

		RE::GFxValue		  number;
		[[maybe_unused]] auto success = _instance.Invoke("getCharIndexAtPoint", &number, args, kNumArgs);
		assert(success);

		return number.GetNumber();
	}


	double TextField::GetFirstCharInParagraph(double a_charIndex)
	{
		enum
		{
			kCharIndex,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kCharIndex] = a_charIndex;
		assert(args[kCharIndex].IsNumber());

		RE::GFxValue		  number;
		[[maybe_unused]] auto success = _instance.Invoke("getFirstCharInParagraph", &number, args, kNumArgs);
		assert(success);

		return number.GetNumber();
	}


	double TextField::GetLineIndexAtPoint(double a_x, double a_y)
	{
		enum
		{
			kX,
			kY,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kX] = a_x;
		assert(args[kX].IsNumber());

		args[kY] = a_y;
		assert(args[kY].IsNumber());

		RE::GFxValue		  number;
		[[maybe_unused]] auto success = _instance.Invoke("getLineIndexAtPoint", &number, args, kNumArgs);
		assert(success);

		return number.GetNumber();
	}


	double TextField::GetLineLength(double a_lineIndex)
	{
		enum
		{
			kLineIndex,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kLineIndex] = a_lineIndex;
		assert(args[kLineIndex].IsNumber());

		RE::GFxValue		  number;
		[[maybe_unused]] auto success = _instance.Invoke("getLineLength", &number, args, kNumArgs);
		assert(success);

		return number.GetNumber();
	}


	Object TextField::GetLineMetrics(double a_lineIndex)
	{
		enum
		{
			kLineIndex,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kLineIndex] = a_lineIndex;
		assert(args[kLineIndex].IsNumber());

		RE::GFxValue		  object;
		[[maybe_unused]] auto success = _instance.Invoke("getLineMetrics", &object, args, kNumArgs);
		assert(success);

		return Object(object);
	}


	double TextField::GetLineOffset(double a_lineIndex)
	{
		enum
		{
			kLineIndex,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kLineIndex] = a_lineIndex;
		assert(args[kLineIndex].IsNumber());

		RE::GFxValue		  number;
		[[maybe_unused]] auto success = _instance.Invoke("getLineOffset", &number, args, kNumArgs);
		assert(success);

		return number.GetNumber();
	}


	std::string_view TextField::GetLineText(double a_lineIndex)
	{
		enum
		{
			kLineIndex,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kLineIndex] = a_lineIndex;
		assert(args[kLineIndex].IsNumber());

		RE::GFxValue		  str;
		[[maybe_unused]] auto success = _instance.Invoke("getLineText", &str, args, kNumArgs);
		assert(success);

		return str.GetString();
	}


	void TextField::CopyToClipboard(bool a_richClipboard, double a_startIndex, double a_endIndex)
	{
		enum
		{
			kRichClipboard,
			kStartIndex,
			kEndIndex,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kRichClipboard] = a_richClipboard;
		assert(args[kRichClipboard].IsBool());

		args[kStartIndex] = a_startIndex;
		assert(args[kStartIndex].IsNumber());

		args[kEndIndex] = a_endIndex;
		assert(args[kEndIndex].IsNumber());

		[[maybe_unused]] auto success = _instance.Invoke("copyToClipboard", 0, args, kNumArgs);
		assert(success);
	}


	void TextField::CutToClipboard(bool a_richClipboard, double a_startIndex, double a_endIndex)
	{
		enum
		{
			kRichClipboard,
			kStartIndex,
			kEndIndex,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kRichClipboard] = a_richClipboard;
		assert(args[kRichClipboard].IsBool());

		args[kStartIndex] = a_startIndex;
		assert(args[kStartIndex].IsNumber());

		args[kEndIndex] = a_endIndex;
		assert(args[kEndIndex].IsNumber());

		[[maybe_unused]] auto success = _instance.Invoke("cutToClipboard", 0, args, kNumArgs);
		assert(success);
	}


	void TextField::PasteFromClipboard(bool a_richClipboard, double a_startIndex, double a_endIndex)
	{
		enum
		{
			kRichClipboard,
			kStartIndex,
			kEndIndex,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kRichClipboard] = a_richClipboard;
		assert(args[kRichClipboard].IsBool());

		args[kStartIndex] = a_startIndex;
		assert(args[kStartIndex].IsNumber());

		args[kEndIndex] = a_endIndex;
		assert(args[kEndIndex].IsNumber());

		[[maybe_unused]] auto success = _instance.Invoke("pasteFromClipboard", 0, args, kNumArgs);
		assert(success);
	}
}
