#include "PCH.h"
#include "CLIK/ScrollingList.h"


namespace CLIK
{
	namespace GFx
	{
		namespace Controls
		{
			ScrollingList::ScrollingList() :
				super()
			{}


			ScrollingList::ScrollingList(const ScrollingList& a_rhs) :
				super(a_rhs)
			{}


			ScrollingList::ScrollingList(ScrollingList&& a_rhs) :
				super(std::move(a_rhs))
			{}


			ScrollingList::ScrollingList(const CoreList& a_rhs) :
				super(a_rhs)
			{}


			ScrollingList::ScrollingList(CoreList&& a_rhs) :
				super(std::move(a_rhs))
			{}


			ScrollingList::ScrollingList(const RE::GFxValue& a_val) :
				super(a_val)
			{}


			ScrollingList::ScrollingList(RE::GFxValue&& a_val) :
				super(std::move(a_val))
			{}


			ScrollingList::~ScrollingList()
			{}


			ScrollingList& ScrollingList::operator=(const ScrollingList& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			ScrollingList& ScrollingList::operator=(ScrollingList&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			ScrollingList& ScrollingList::operator=(const CoreList& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			ScrollingList& ScrollingList::operator=(CoreList&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			ScrollingList& ScrollingList::operator=(const RE::GFxValue& a_rhs)
			{
				_instance = a_rhs;
				return *this;
			}


			ScrollingList& ScrollingList::operator=(RE::GFxValue&& a_rhs)
			{
				_instance = std::move(a_rhs);
				return *this;
			}


			Object ScrollingList::ScrollBar() const
			{
				return GetObject("scrollBar");
			}


			void ScrollingList::ScrollBar(const Object& a_scrollBar)
			{
				SetObject("scrollBar", a_scrollBar);
			}


			double ScrollingList::RowHeight() const
			{
				return GetNumber("rowHeight");
			}


			void ScrollingList::RowHeight(double a_rowHeight)
			{
				SetNumber("rowHeight", a_rowHeight);
			}


			double ScrollingList::ScrollPosition() const
			{
				return GetNumber("scrollPosition");
			}


			void ScrollingList::ScrollPosition(double a_scrollPosition)
			{
				SetNumber("scrollPosition", a_scrollPosition);
			}


			double ScrollingList::SelectedIndex() const
			{
				return GetNumber("selectedIndex");
			}


			void ScrollingList::SelectedIndex(double a_selectedIndex)
			{
				SetNumber("selectedIndex", a_selectedIndex);
			}


			bool ScrollingList::Disabled() const
			{
				return GetBoolean("disabled");
			}


			void ScrollingList::Disabled(bool a_disabled)
			{
				SetBoolean("disabled", a_disabled);
			}


			void ScrollingList::ScrollToIndex(double a_index)
			{
				enum
				{
					kIndex,
					kNumArgs
				};

				RE::GFxValue args[kNumArgs];

				args[kIndex] = a_index;
				assert(args[kIndex].IsNumber());

				[[maybe_unused]] auto success = _instance.Invoke("scrollToIndex", 0, args, kNumArgs);
				assert(success);
			}


			double ScrollingList::RowCount() const
			{
				return GetNumber("rowCount");
			}


			void ScrollingList::RowCount(double a_rowCount)
			{
				SetNumber("rowCount", a_rowCount);
			}


			void ScrollingList::InvalidateData()
			{
				[[maybe_unused]] auto success = _instance.Invoke("invalidateData");
				assert(success);
			}


			double ScrollingList::AvailableWidth() const
			{
				return GetNumber("availableWidth");
			}


			std::string_view ScrollingList::ToString()
			{
				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("toString", &str);
				assert(success);
				return str.GetString();
			}
		}
	}
}
