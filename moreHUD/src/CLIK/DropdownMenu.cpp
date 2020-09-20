#include "PCH.h"
#include "CLIK/DropdownMenu.h"


namespace CLIK
{
	namespace GFx
	{
		namespace Controls
		{
			DropdownMenu::DropdownMenu() :
				super()
			{}


			DropdownMenu::DropdownMenu(const DropdownMenu& a_rhs) :
				super(a_rhs)
			{}


			DropdownMenu::DropdownMenu(DropdownMenu&& a_rhs) :
				super(std::move(a_rhs))
			{}


			DropdownMenu::DropdownMenu(const Button& a_rhs) :
				super(a_rhs)
			{}


			DropdownMenu::DropdownMenu(Button&& a_rhs) :
				super(std::move(a_rhs))
			{}


			DropdownMenu::DropdownMenu(const RE::GFxValue& a_val) :
				super(a_val)
			{}


			DropdownMenu::DropdownMenu(RE::GFxValue&& a_val) :
				super(std::move(a_val))
			{}


			DropdownMenu::~DropdownMenu()
			{}


			DropdownMenu& DropdownMenu::operator=(const DropdownMenu& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			DropdownMenu& DropdownMenu::operator=(DropdownMenu&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			DropdownMenu& DropdownMenu::operator=(const Button& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			DropdownMenu& DropdownMenu::operator=(Button&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			DropdownMenu& DropdownMenu::operator=(const RE::GFxValue& a_rhs)
			{
				_instance = a_rhs;
				return *this;
			}


			DropdownMenu& DropdownMenu::operator=(RE::GFxValue&& a_rhs)
			{
				_instance = std::move(a_rhs);
				return *this;
			}


			Object DropdownMenu::Dropdown() const
			{
				return GetObject("dropdown");
			}


			void DropdownMenu::Dropdown(const Object& a_dropdown)
			{
				SetObject("dropdown", a_dropdown);
			}


			Object DropdownMenu::ItemRenderer() const
			{
				return GetObject("itemRenderer");
			}


			void DropdownMenu::ItemRenderer(const Object& a_itemRenderer)
			{
				SetObject("itemRenderer", a_itemRenderer);
			}


			Object DropdownMenu::ScrollBar() const
			{
				return GetObject("scrollBar");
			}


			void DropdownMenu::ScrollBar(const Object& a_scrollBar)
			{
				SetObject("scrollBar", a_scrollBar);
			}


			Object DropdownMenu::DropdownWidth() const
			{
				return GetObject("scrollBar");
			}


			void DropdownMenu::DropdownWidth(const Object& a_dropdownWidth)
			{
				SetObject("scrollBar", a_dropdownWidth);
			}


			double DropdownMenu::RowCount() const
			{
				return GetNumber("rowCount");
			}


			void DropdownMenu::RowCount(double a_rowCount)
			{
				SetNumber("rowCount", a_rowCount);
			}


			Object DropdownMenu::DataProvider() const
			{
				return GetObject("dataProvider");
			}


			void DropdownMenu::DataProvider(const Object& a_dataProvider)
			{
				SetObject("dataProvider", a_dataProvider);
			}


			double DropdownMenu::SelectedIndex() const
			{
				return GetNumber("selectedIndex");
			}


			void DropdownMenu::SelectedIndex(double a_selectedIndex)
			{
				SetNumber("selectedIndex", a_selectedIndex);
			}


			std::string_view DropdownMenu::LabelField() const
			{
				return GetString("labelField");
			}


			void DropdownMenu::LabelField(std::string_view a_selectedIndex)
			{
				SetString("labelField", a_selectedIndex);
			}


			std::string_view DropdownMenu::ItemToLabel(Object& a_item)
			{
				enum
				{
					kItem,
					kNumArgs
				};

				RE::GFxValue args[kNumArgs];

				args[kItem] = a_item.GetInstance();

				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("itemToLabel", &str, args, kNumArgs);
				assert(success);

				return str.GetString();
			}


			void DropdownMenu::Open()
			{
				[[maybe_unused]] auto success = _instance.Invoke("open");
				assert(success);
			}


			void DropdownMenu::Close()
			{
				[[maybe_unused]] auto success = _instance.Invoke("close");
				assert(success);
			}


			void DropdownMenu::InvalidateData()
			{
				[[maybe_unused]] auto success = _instance.Invoke("invalidateData");
				assert(success);
			}


			void DropdownMenu::SetSize(double a_width, double a_height)
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


			void DropdownMenu::RemoveMovieClip()
			{
				[[maybe_unused]] auto success = _instance.Invoke("removeMovieClip");
				assert(success);
			}


			std::string_view DropdownMenu::ToString()
			{
				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("toString", &str);
				assert(success);
				return str.GetString();
			}
		}
	}
}
