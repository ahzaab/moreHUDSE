#include "PCH.h"
#include "CLIK/Object.h"


namespace CLIK
{
	Object::Object() :
		_instance()
	{}


	Object::Object(const Object& a_rhs) :
		_instance(a_rhs._instance)
	{}


	Object::Object(Object&& a_rhs) :
		_instance(std::move(a_rhs._instance))
	{}


	Object::Object(double a_val) :
		_instance(a_val)
	{}


	Object::Object(bool a_val) :
		_instance(a_val)
	{}


	Object::Object(const char* a_val) :
		_instance(a_val)
	{}


	Object::Object(std::string_view a_val) :
		_instance(a_val)
	{}


	Object::Object(const wchar_t* a_val) :
		_instance(a_val)
	{}


	Object::Object(std::wstring_view a_val) :
		_instance(a_val)
	{}


	Object::Object(const RE::GFxValue& a_val) :
		_instance(a_val)
	{
		assert(_instance.IsObject());
	}


	Object::Object(RE::GFxValue&& a_val) :
		_instance(std::move(a_val))
	{
		assert(_instance.IsObject());
	}


	Object::~Object()
	{}


	Object& Object::operator=(const Object& a_rhs)
	{
		_instance = a_rhs._instance;
		return *this;
	}


	Object& Object::operator=(Object&& a_rhs)
	{
		_instance = std::move(a_rhs._instance);
		return *this;
	}


	Object& Object::operator=(double a_val)
	{
		_instance = a_val;
		return *this;
	}


	Object& Object::operator=(bool a_val)
	{
		_instance = a_val;
		return *this;
	}


	Object& Object::operator=(const char* a_val)
	{
		_instance = a_val;
		return *this;
	}


	Object& Object::operator=(std::string_view a_val)
	{
		_instance = a_val;
		return *this;
	}


	Object& Object::operator=(const wchar_t* a_val)
	{
		_instance = a_val;
		return *this;
	}


	Object& Object::operator=(std::wstring_view a_val)
	{
		_instance = a_val;
		return *this;
	}


	Object& Object::operator=(const RE::GFxValue& a_rhs)
	{
		_instance = a_rhs;
		assert(_instance.IsObject());
		return *this;
	}


	Object& Object::operator=(RE::GFxValue&& a_rhs)
	{
		_instance = std::move(a_rhs);
		assert(_instance.IsObject());
		return *this;
	}


	RE::GFxValue& Object::GetInstance()
	{
		return _instance;
	}


	Object Object::Constructor() const
	{
		return GetObject("constructor");
	}


	Object Object::Prototype() const
	{
		return GetObject("__proto__");
	}


	Object Object::Resolve() const
	{
		return GetObject("__resolve");
	}


	void Object::Resolve(Object& a_resolve)
	{
		SetObject("__resolve", a_resolve);
	}


	bool Object::HasOwnProperty(std::string_view a_name)
	{
		enum
		{
			kName,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kName] = a_name;
		assert(args[kName].IsString());

		RE::GFxValue		  boolean;
		[[maybe_unused]] auto success = _instance.Invoke("hasOwnProperty", &boolean, args, kNumArgs);
		assert(success);

		return boolean.GetBool();
	}


	bool Object::IsPropertyEnumerable(std::string_view a_name)
	{
		enum
		{
			kName,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kName] = a_name;
		assert(args[kName].IsString());

		RE::GFxValue		  boolean;
		[[maybe_unused]] auto success = _instance.Invoke("isPropertyEnumerable", &boolean, args, kNumArgs);
		assert(success);

		return boolean.GetBool();
	}


	bool Object::IsPrototypeOf(Object& a_theClass)
	{
		enum
		{
			kTheClass,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kTheClass] = a_theClass._instance;
		assert(args[kTheClass].IsObject());

		RE::GFxValue		  boolean;
		[[maybe_unused]] auto success = _instance.Invoke("isPrototypeOf", &boolean, args, kNumArgs);
		assert(success);

		return boolean.GetBool();
	}


	bool Object::Unwatch(std::string_view a_name)
	{
		enum
		{
			kName,
			kNumArgs
		};

		RE::GFxValue args[kNumArgs];

		args[kName] = a_name;
		assert(args[kName].IsString());

		RE::GFxValue		  boolean;
		[[maybe_unused]] auto success = _instance.Invoke("unwatch", &boolean, args, kNumArgs);
		assert(success);

		return boolean.GetBool();
	}


	std::string Object::ToString()
	{
		RE::GFxValue		  str;
		[[maybe_unused]] auto success = _instance.Invoke("toString", &str);
		assert(success);
		return str.GetString();
	}


	Object Object::ValueOf()
	{
		RE::GFxValue		  object;
		[[maybe_unused]] auto success = _instance.Invoke("valueOf", &object);
		assert(success);
		return Object(object);
	}


	bool Object::GetBoolean(const char* a_path) const
	{
		RE::GFxValue		  boolean;
		[[maybe_unused]] auto success = _instance.GetMember(a_path, &boolean);
		assert(success);
		return boolean.GetBool();
	}


	double Object::GetNumber(const char* a_path) const
	{
		RE::GFxValue		  number;
		[[maybe_unused]] auto success = _instance.GetMember(a_path, &number);
		assert(success);
		return number.GetNumber();
	}


	Object Object::GetObject(const char* a_path) const
	{
		RE::GFxValue		  object;
		[[maybe_unused]] auto success = _instance.GetMember(a_path, &object);
		assert(success);
		return Object(object);
	}


	std::string_view Object::GetString(const char* a_path) const
	{
		RE::GFxValue		  str;
		[[maybe_unused]] auto success = _instance.GetMember(a_path, &str);
		assert(success);
		return str.GetString();
	}


	void Object::SetBoolean(const char* a_path, bool a_boolean)
	{
		RE::GFxValue		  boolean(a_boolean);
		[[maybe_unused]] auto success = _instance.SetMember(a_path, boolean);
		assert(success);
	}


	void Object::SetNumber(const char* a_path, double a_number)
	{
		RE::GFxValue		  number(a_number);
		[[maybe_unused]] auto success = _instance.SetMember(a_path, number);
		assert(success);
	}


	void Object::SetObject(const char* a_path, const Object& a_object)
	{
		[[maybe_unused]] auto success = _instance.SetMember(a_path, a_object._instance);
		assert(success);
	}


	void Object::SetString(const char* a_path, std::string_view a_string)
	{
		RE::GFxValue		  str(a_string);
		[[maybe_unused]] auto success = _instance.SetMember(a_path, str);
		assert(success);
	}
}
