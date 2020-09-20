#include "PCH.h"
#include "CLIK/CoreList.h"

#include "CLIK/Array.h"


namespace CLIK
{
	namespace GFx
	{
		namespace Controls
		{
			CoreList::CoreList() :
				super()
			{}


			CoreList::CoreList(const CoreList& a_rhs) :
				super(a_rhs)
			{}


			CoreList::CoreList(CoreList&& a_rhs) :
				super(std::move(a_rhs))
			{}


			CoreList::CoreList(const Core::UIComponent& a_rhs) :
				super(a_rhs)
			{}


			CoreList::CoreList(Core::UIComponent&& a_rhs) :
				super(std::move(a_rhs))
			{}


			CoreList::CoreList(const RE::GFxValue& a_val) :
				super(a_val)
			{}


			CoreList::CoreList(RE::GFxValue&& a_val) :
				super(std::move(a_val))
			{}


			CoreList::~CoreList()
			{}


			CoreList& CoreList::operator=(const CoreList& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			CoreList& CoreList::operator=(CoreList&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			CoreList& CoreList::operator=(const Core::UIComponent& a_rhs)
			{
				super::operator=(a_rhs);
				return *this;
			}


			CoreList& CoreList::operator=(Core::UIComponent&& a_rhs)
			{
				super::operator=(std::move(a_rhs));
				return *this;
			}


			CoreList& CoreList::operator=(const RE::GFxValue& a_rhs)
			{
				_instance = a_rhs;
				return *this;
			}


			CoreList& CoreList::operator=(RE::GFxValue&& a_rhs)
			{
				_instance = std::move(a_rhs);
				return *this;
			}


			std::string_view CoreList::ItemRenderer() const
			{
				return GetString("itemRenderer");
			}


			void CoreList::ItemRenderer(std::string_view a_itemRenderer)
			{
				SetString("itemRenderer", a_itemRenderer);
			}


			Object CoreList::DataProvider() const
			{
				return GetObject("dataProvider");
			}


			void CoreList::DataProvider(const Object& a_dataProvider)
			{
				SetObject("dataProvider", a_dataProvider);
			}


			double CoreList::SelectedIndex() const
			{
				return GetNumber("selectedIndex");
			}


			void CoreList::SelectedIndex(double a_selectedIndex)
			{
				SetNumber("selectedIndex", a_selectedIndex);
			}


			void CoreList::ScrollToIndex(double a_index)
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


			std::string_view CoreList::LabelField() const
			{
				return GetString("labelField");
			}


			void CoreList::LabelField(std::string_view a_labelField)
			{
				SetString("labelField", a_labelField);
			}


			std::string_view CoreList::ItemToLabel(Object& a_item)
			{
				enum
				{
					kItem,
					kNumArgs
				};

				RE::GFxValue args[kNumArgs];

				args[kItem] = a_item.GetInstance();
				assert(args[kItem].IsObject());

				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("itemToLabel", &str, args, kNumArgs);
				assert(success);

				return str.GetString();
			}


			void CoreList::InvalidateData()
			{
				[[maybe_unused]] auto success = _instance.Invoke("invalidateData");
				assert(success);
			}


			double CoreList::AvailableWidth() const
			{
				return GetNumber("availableWidth");
			}


			double CoreList::AvailableHeight() const
			{
				return GetNumber("availableHeight");
			}


			void CoreList::SetRendererList(Array& a_value)
			{
				enum
				{
					kValue,
					kNumArgs
				};

				RE::GFxValue args[kNumArgs];

				args[kValue] = a_value.GetInstance();
				assert(args[kValue].IsArray());

				[[maybe_unused]] auto success = _instance.Invoke("setRendererList", 0, args, kNumArgs);
				assert(success);
			}


			std::string_view CoreList::RendererInstanceName() const
			{
				return GetString("rendererInstanceName");
			}


			void CoreList::RendererInstanceName(std::string_view a_rendererInstanceName)
			{
				SetString("rendererInstanceName", a_rendererInstanceName);
			}


			std::string_view CoreList::ToString()
			{
				RE::GFxValue		  str;
				[[maybe_unused]] auto success = _instance.Invoke("toString", &str);
				assert(success);
				return str.GetString();
			}
		}
	}
}
