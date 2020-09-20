#include "PCH.h"
#include "CLIK/Button.h"


namespace CLIK
{
	namespace GFx
	{
		namespace Controls
		{
			Button::Button() :
				super()
			{}


			Button::Button(const Button& a_rhs) :
				super(a_rhs)
			{}


			Button::Button(Button&& a_rhs) :
				super(std::move(a_rhs))
			{}


			Button::Button(const Core::UIComponent& a_rhs) :
				super(a_rhs)
			{}


			Button::Button(Core::UIComponent&& a_rhs) :
				super(std::move(a_rhs))
			{}


			Button::Button(const RE::GFxValue& a_val) :
				super(a_val)
			{}


			Button::Button(RE::GFxValue&& a_val) :
				super(std::move(a_val))
			{}


			Button::~Button()
			{}


			Button& Button::operator=(const Button& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			Button& Button::operator=(Button&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			Button& Button::operator=(const Core::UIComponent& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			Button& Button::operator=(Core::UIComponent&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			Button& Button::operator=(const RE::GFxValue& a_rhs)
			{
				_instance = a_rhs;
				return *this;
			}


			Button& Button::operator=(RE::GFxValue&& a_rhs)
			{
				_instance = std::move(a_rhs);
				return *this;
			}


			std::string_view Button::LabelID() const
			{
				return GetString("labelID");
			}


			void Button::LabelID(std::string_view a_labelID)
			{
				SetString("labelID", a_labelID);
			}


			std::string_view Button::Label() const
			{
				return GetString("label");
			}


			void Button::Label(std::string_view a_label)
			{
				SetString("label", a_label);
			}


			bool Button::Disabled() const
			{
				return GetBoolean("disabled");
			}


			void Button::Disabled(bool a_disabled)
			{
				SetBoolean("disabled", a_disabled);
			}


			bool Button::Selected() const
			{
				return GetBoolean("selected");
			}


			void Button::Selected(bool a_selected)
			{
				SetBoolean("selected", a_selected);
			}


			std::string_view Button::GroupName() const
			{
				return GetString("groupName");
			}


			void Button::GroupName(std::string_view a_groupName)
			{
				SetString("groupName", a_groupName);
			}


			Object Button::Group() const
			{
				return GetObject("group");
			}


			void Button::Group(const Object& a_group)
			{
				SetObject("group", a_group);
			}


			bool Button::DisableFocus() const
			{
				return GetBoolean("disableFocus");
			}


			void Button::DisableFocus(bool a_disableFocus)
			{
				SetBoolean("disableFocus", a_disableFocus);
			}


			bool Button::DisableConstraints() const
			{
				return GetBoolean("disableConstraints");
			}


			void Button::DisableConstraints(bool a_disableConstraints)
			{
				SetBoolean("disableConstraints", a_disableConstraints);
			}


			std::string_view Button::AutoSize() const
			{
				return GetString("autoSize");
			}


			void Button::AutoSize(std::string_view a_autoSize)
			{
				SetString("autoSize", a_autoSize);
			}


			void Button::SetSize(double a_width, double a_height)
			{
				enum
				{
					kWidth,
					kHeight,
					kNumArgs
				};

				RE::GFxValue args[kNumArgs];

				args[kWidth] = a_width;
				assert(args[kWidth].IsNumber());

				args[kHeight] = a_height;
				assert(args[kHeight].IsNumber());

				[[maybe_unused]] auto success = _instance.Invoke("setSize", 0, args, kNumArgs);
				assert(success);
			}


			std::string_view Button::ToString()
			{
				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("toString", &str);
				assert(success);
				return str.GetString();
			}
		}
	}
}
