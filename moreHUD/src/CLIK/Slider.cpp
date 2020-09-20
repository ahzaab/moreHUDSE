#include "PCH.h"
#include "CLIK/Slider.h"


namespace CLIK
{
	namespace GFx
	{
		namespace Controls
		{
			Slider::Slider() :
				super()
			{}


			Slider::Slider(const Slider& a_rhs) :
				super(a_rhs)
			{}


			Slider::Slider(Slider&& a_rhs) :
				super(std::move(a_rhs))
			{}


			Slider::Slider(const Core::UIComponent& a_rhs) :
				super(a_rhs)
			{}


			Slider::Slider(Core::UIComponent&& a_rhs) :
				super(std::move(a_rhs))
			{}


			Slider::Slider(const RE::GFxValue& a_val) :
				super(a_val)
			{}


			Slider::Slider(RE::GFxValue&& a_val) :
				super(std::move(a_val))
			{}


			Slider::~Slider()
			{}


			Slider& Slider::operator=(const Slider& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			Slider& Slider::operator=(Slider&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			Slider& Slider::operator=(const Core::UIComponent& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			Slider& Slider::operator=(Core::UIComponent&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			Slider& Slider::operator=(const RE::GFxValue& a_rhs)
			{
				_instance = a_rhs;
				return *this;
			}


			Slider& Slider::operator=(RE::GFxValue&& a_rhs)
			{
				_instance = std::move(a_rhs);
				return *this;
			}


			double Slider::Maximum() const
			{
				return GetNumber("maximum");
			}


			void Slider::Maximum(double a_maximum)
			{
				SetNumber("maximum", a_maximum);
			}


			double Slider::Minimum() const
			{
				return GetNumber("minimum");
			}


			void Slider::Minimum(double a_minimum)
			{
				SetNumber("minimum", a_minimum);
			}


			double Slider::Value() const
			{
				return GetNumber("value");
			}


			void Slider::Value(double a_value)
			{
				SetNumber("value", a_value);
			}


			bool Slider::Disabled() const
			{
				return GetBoolean("disabled");
			}


			void Slider::Disabled(bool a_disabled)
			{
				SetBoolean("disabled", a_disabled);
			}


			double Slider::Position() const
			{
				return GetNumber("position");
			}


			void Slider::Position(double a_position)
			{
				SetNumber("position", a_position);
			}


			bool Slider::Snapping() const
			{
				return GetBoolean("snapping");
			}


			void Slider::Snapping(bool a_snapping)
			{
				SetBoolean("snapping", a_snapping);
			}


			double Slider::SnapInterval() const
			{
				return GetNumber("snapInterval");
			}


			void Slider::SnapInterval(double a_snapInterval)
			{
				SetNumber("snapInterval", a_snapInterval);
			}


			std::string_view Slider::ToString()
			{
				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("toString", &str);
				assert(success);
				return str.GetString();
			}
		}
	}
}
