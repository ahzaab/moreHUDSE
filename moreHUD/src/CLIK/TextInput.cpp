#include "PCH.h"
#include "CLIK/TextInput.h"


namespace CLIK
{
	namespace GFx
	{
		namespace Controls
		{
			TextInput::TextInput() :
				super()
			{}


			TextInput::TextInput(const TextInput& a_rhs) :
				super(a_rhs)
			{}


			TextInput::TextInput(TextInput&& a_rhs) :
				super(std::move(a_rhs))
			{}


			TextInput::TextInput(const Core::UIComponent& a_rhs) :
				super(a_rhs)
			{}


			TextInput::TextInput(Core::UIComponent&& a_rhs) :
				super(std::move(a_rhs))
			{}


			TextInput::TextInput(const RE::GFxValue& a_val) :
				super(a_val)
			{}


			TextInput::TextInput(RE::GFxValue&& a_val) :
				super(std::move(a_val))
			{}


			TextInput::~TextInput()
			{}


			TextInput& TextInput::operator=(const TextInput& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			TextInput& TextInput::operator=(TextInput&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			TextInput& TextInput::operator=(const Core::UIComponent& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			TextInput& TextInput::operator=(Core::UIComponent&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			TextInput& TextInput::operator=(const RE::GFxValue& a_rhs)
			{
				_instance = a_rhs;
				return *this;
			}


			TextInput& TextInput::operator=(RE::GFxValue&& a_rhs)
			{
				_instance = std::move(a_rhs);
				return *this;
			}


			std::string_view TextInput::TextID() const
			{
				return GetString("textID");
			}


			void TextInput::TextID(std::string_view a_textID)
			{
				SetString("textID", a_textID);
			}


			std::string_view TextInput::Text() const
			{
				return GetString("text");
			}


			void TextInput::Text(std::string_view a_text)
			{
				SetString("text", a_text);
			}


			std::string_view TextInput::HTMLText() const
			{
				return GetString("htmlText");
			}


			void TextInput::HTMLText(std::string_view a_htmlText)
			{
				SetString("htmlText", a_htmlText);
			}


			bool TextInput::Editable() const
			{
				return GetBoolean("editable");
			}


			void TextInput::Editable(bool a_editable)
			{
				SetBoolean("editable", a_editable);
			}


			bool TextInput::Password() const
			{
				return GetBoolean("password");
			}


			void TextInput::Password(bool a_password)
			{
				SetBoolean("password", a_password);
			}


			double TextInput::MaxChars() const
			{
				return GetNumber("maxChars");
			}


			void TextInput::MaxChars(double a_maxChars)
			{
				SetNumber("maxChars", a_maxChars);
			}


			bool TextInput::Disabled() const
			{
				return GetBoolean("disabled");
			}


			void TextInput::Disabled(bool a_disabled)
			{
				SetBoolean("disabled", a_disabled);
			}


			void TextInput::AppendText(std::string_view a_text)
			{
				enum
				{
					kText,
					kNumArgs
				};

				RE::GFxValue args[kNumArgs];

				args[kText] = a_text;
				assert(args[kText].IsString());

				[[maybe_unused]] auto success = _instance.Invoke("appendText", 0, args, kNumArgs);
				assert(success);
			}


			void TextInput::AppendHTML(std::string_view a_text)
			{
				enum
				{
					kText,
					kNumArgs
				};

				RE::GFxValue args[kNumArgs];

				args[kText] = a_text;
				assert(args[kText].IsString());

				[[maybe_unused]] auto success = _instance.Invoke("appendHtml", 0, args, kNumArgs);
				assert(success);
			}


			double TextInput::Length() const
			{
				return GetNumber("length");
			}


			std::string_view TextInput::ToString()
			{
				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("toString", &str);
				assert(success);
				return str.GetString();
			}
		}
	}
}
