#include "PCH.h"
#include "CLIK/UIComponent.h"

#include <cassert>


namespace CLIK
{
	namespace GFx
	{
		namespace Core
		{
			UIComponent::UIComponent() :
				super()
			{}


			UIComponent::UIComponent(const UIComponent& a_rhs) :
				super(a_rhs)
			{}


			UIComponent::UIComponent(UIComponent&& a_rhs) :
				super(std::move(a_rhs))
			{}


			UIComponent::UIComponent(const MovieClip& a_rhs) :
				super(a_rhs)
			{}


			UIComponent::UIComponent(MovieClip&& a_rhs) :
				super(std::move(a_rhs))
			{}


			UIComponent::UIComponent(const RE::GFxValue& a_val) :
				super(a_val)
			{}


			UIComponent::UIComponent(RE::GFxValue&& a_val) :
				super(std::move(a_val))
			{}


			UIComponent::~UIComponent()
			{}


			UIComponent& UIComponent::operator=(const UIComponent& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			UIComponent& UIComponent::operator=(UIComponent&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			UIComponent& UIComponent::operator=(const MovieClip& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			UIComponent& UIComponent::operator=(MovieClip&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			UIComponent& UIComponent::operator=(const RE::GFxValue& a_rhs)
			{
				_instance = a_rhs;
				return *this;
			}


			UIComponent& UIComponent::operator=(RE::GFxValue&& a_rhs)
			{
				_instance = std::move(a_rhs);
				return *this;
			}


			bool UIComponent::Disabled() const
			{
				return GetBoolean("disabled");
			}


			void UIComponent::Disabled(bool a_disabled)
			{
				SetBoolean("disabled", a_disabled);
			}


			bool UIComponent::Visible() const
			{
				return GetBoolean("visible");
			}


			void UIComponent::Visible(bool a_visible)
			{
				SetBoolean("visible", a_visible);
			}


			double UIComponent::Width() const
			{
				return GetNumber("width");
			}


			void UIComponent::Width(double a_width)
			{
				SetNumber("width", a_width);
			}


			double UIComponent::Height() const
			{
				return GetNumber("height");
			}


			void UIComponent::Height(double a_height)
			{
				SetNumber("height", a_height);
			}


			void UIComponent::SetSize(double a_width, double a_height)
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


			double UIComponent::Focused() const
			{
				return GetNumber("focused");
			}


			void UIComponent::Focused(double a_focused)
			{
				SetNumber("focused", a_focused);
			}


			bool UIComponent::DisplayFocus() const
			{
				return GetBoolean("displayFocus");
			}


			void UIComponent::DisplayFocus(bool a_displayFocus)
			{
				SetBoolean("displayFocus", a_displayFocus);
			}


			void UIComponent::Invalidate()
			{
				[[maybe_unused]] auto success = _instance.Invoke("invalidate");
				assert(success);
			}


			void UIComponent::ValidateNow()
			{
				[[maybe_unused]] auto success = _instance.Invoke("validateNow");
				assert(success);
			}


			std::string_view UIComponent::ToString()
			{
				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("toString", &str);
				assert(success);
				return str.GetString();
			}


			void UIComponent::DispatchEventToGame(Object& a_event)
			{
				enum
				{
					kEvent,
					kNumArgs
				};

				RE::GFxValue args[kNumArgs];

				args[kEvent] = a_event.GetInstance();
				assert(args[kEvent].IsObject());

				[[maybe_unused]] auto success = _instance.Invoke("dispatchEventToGame", 0, args, kNumArgs);
				assert(success);
			}
		}
	}
}
