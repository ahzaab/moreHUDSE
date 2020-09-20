#include "PCH.h"
#include "CLIK/MovieClip.h"


namespace CLIK
{
	MovieClip::MovieClip() :
		super()
	{}


	MovieClip::MovieClip(const MovieClip& a_rhs) :
		super(a_rhs)
	{}


	MovieClip::MovieClip(MovieClip&& a_rhs) :
		super(std::move(a_rhs))
	{}


	MovieClip::MovieClip(const Object& a_rhs) :
		super(a_rhs)
	{}


	MovieClip::MovieClip(Object&& a_rhs) :
		super(std::move(a_rhs))
	{}


	MovieClip::MovieClip(const RE::GFxValue& a_val) :
		super(a_val)
	{}


	MovieClip::MovieClip(RE::GFxValue&& a_val) :
		super(std::move(a_val))
	{}


	MovieClip::~MovieClip()
	{}


	MovieClip& MovieClip::operator=(const MovieClip& a_rhs)
	{
		super::operator=(a_rhs);
		return *this;
	}


	MovieClip& MovieClip::operator=(MovieClip&& a_rhs)
	{
		super::operator=(std::move(a_rhs));
		return *this;
	}


	MovieClip& MovieClip::operator=(const Object& a_rhs)
	{
		super::operator=(a_rhs);
		return *this;
	}


	MovieClip& MovieClip::operator=(Object&& a_rhs)
	{
		super::operator=(std::move(a_rhs));
		return *this;
	}


	MovieClip& MovieClip::operator=(const RE::GFxValue& a_rhs)
	{
		_instance = a_rhs;
		return *this;
	}


	MovieClip& MovieClip::operator=(RE::GFxValue&& a_rhs)
	{
		_instance = std::move(a_rhs);
		return *this;
	}


	double MovieClip::Alpha() const
	{
		return GetNumber("_alpha");
	}


	void MovieClip::Alpha(double a_alpha)
	{
		SetNumber("_alpha", a_alpha);
	}


	Object MovieClip::BlendMode() const
	{
		return GetObject("blendMode");
	}


	void MovieClip::BlendMode(const Object& a_blendMode)
	{
		SetObject("blendMode", a_blendMode);
	}


	bool MovieClip::CacheAsBitmap() const
	{
		return GetBoolean("cacheAsBitmap");
	}


	void MovieClip::CacheAsBitmap(bool a_cacheAsBitmap)
	{
		SetBoolean("cacheAsBitmap", a_cacheAsBitmap);
	}


	double MovieClip::Currentframe() const
	{
		return GetNumber("_currentframe");
	}


	std::string_view MovieClip::Droptarget() const
	{
		return GetString("_droptarget");
	}


	bool MovieClip::Enabled() const
	{
		return GetBoolean("enabled");
	}


	void MovieClip::Enabled(bool a_enabled)
	{
		SetBoolean("enabled", a_enabled);
	}


	bool MovieClip::FocusEnabled() const
	{
		return GetBoolean("focusEnabled");
	}


	void MovieClip::FocusEnabled(bool a_focusEnabled)
	{
		SetBoolean("focusEnabled", a_focusEnabled);
	}


	bool MovieClip::Focusrect() const
	{
		return GetBoolean("_focusrect");
	}


	void MovieClip::Focusrect(bool a_focusrect)
	{
		SetBoolean("_focusrect", a_focusrect);
	}


	double MovieClip::Framesloaded() const
	{
		return GetNumber("_framesloaded");
	}


	double MovieClip::Height() const
	{
		return GetNumber("_height");
	}


	void MovieClip::Height(double a_height)
	{
		SetNumber("_height", a_height);
	}


	double MovieClip::HighQuality() const
	{
		return GetNumber("_highquality");
	}


	void MovieClip::HighQuality(double a_highQuality)
	{
		SetNumber("_highquality", a_highQuality);
	}


	Object MovieClip::HitArea() const
	{
		return GetObject("hitArea");
	}


	void MovieClip::HitArea(const Object& a_hitArea)
	{
		SetObject("hitArea", a_hitArea);
	}


	double MovieClip::LockRoot() const
	{
		return GetNumber("_lockroot");
	}


	void MovieClip::LockRoot(double a_lockRoot)
	{
		SetNumber("_lockroot", a_lockRoot);
	}


	std::string_view MovieClip::Name() const
	{
		return GetString("_name");
	}


	void MovieClip::Name(std::string_view a_name)
	{
		SetString("_name", a_name);
	}


	double MovieClip::OpaqueBackground() const
	{
		return GetNumber("opaqueBackground");
	}


	void MovieClip::OpaqueBackground(double a_opaqueBackground)
	{
		SetNumber("opaqueBackground", a_opaqueBackground);
	}


	MovieClip MovieClip::Parent() const
	{
		return GetObject("_parent");
	}


	void MovieClip::Parent(const MovieClip& a_parent)
	{
		SetObject("_parent", a_parent);
	}


	std::string_view MovieClip::Quality() const
	{
		return GetString("_quality");
	}


	void MovieClip::Quality(std::string_view a_quality)
	{
		SetString("_quality", a_quality);
	}


	double MovieClip::Rotation() const
	{
		return GetNumber("_rotation");
	}


	void MovieClip::Rotation(double a_rotation)
	{
		SetNumber("_rotation", a_rotation);
	}


	Object MovieClip::ScrollRect() const
	{
		return GetObject("scrollRect");
	}


	void MovieClip::ScrollRect(const Object& a_scrollRect)
	{
		SetObject("scrollRect", a_scrollRect);
	}


	double MovieClip::SoundBufTime() const
	{
		return GetNumber("_soundbuftime");
	}


	void MovieClip::SoundBufTime(double a_soundBufTime)
	{
		SetNumber("_soundbuftime", a_soundBufTime);
	}


	bool MovieClip::TabChildren() const
	{
		return GetBoolean("tabChildren");
	}


	void MovieClip::TabChildren(bool a_tabChildren)
	{
		SetBoolean("tabChildren", a_tabChildren);
	}


	bool MovieClip::TabEnabled() const
	{
		return GetBoolean("tabEnabled");
	}


	void MovieClip::TabEnabled(bool a_tabEnabled)
	{
		SetBoolean("tabEnabled", a_tabEnabled);
	}


	double MovieClip::TabIndex() const
	{
		return GetNumber("tabIndex");
	}


	void MovieClip::TabIndex(double a_tabIndex)
	{
		SetNumber("tabIndex", a_tabIndex);
	}


	std::string_view MovieClip::Target() const
	{
		return GetString("_target");
	}


	double MovieClip::TotalFrames() const
	{
		return GetNumber("_totalframes");
	}


	bool MovieClip::TrackAsMenu() const
	{
		return GetBoolean("trackAsMenu");
	}


	void MovieClip::TrackAsMenu(bool a_trackAsMenu)
	{
		SetBoolean("trackAsMenu", a_trackAsMenu);
	}


	std::string_view MovieClip::URL() const
	{
		return GetString("_url");
	}

	bool MovieClip::UseHandCursor() const
	{
		return GetBoolean("useHandCursor");
	}


	void MovieClip::UseHandCursor(bool a_useHandCursor)
	{
		SetBoolean("useHandCursor", a_useHandCursor);
	}

	bool MovieClip::Visible() const
	{
		return GetBoolean("_visible");
	}


	void MovieClip::Visible(bool a_visible)
	{
		SetBoolean("_visible", a_visible);
	}


	double MovieClip::Width() const
	{
		return GetNumber("_width");
	}


	void MovieClip::Width(double a_width)
	{
		SetNumber("_width", a_width);
	}


	double MovieClip::X() const
	{
		return GetNumber("_x");
	}


	void MovieClip::X(double a_x)
	{
		SetNumber("_x", a_x);
	}


	double MovieClip::XMouse() const
	{
		return GetNumber("_xmouse");
	}


	double MovieClip::XScale() const
	{
		return GetNumber("_xscale");
	}


	void MovieClip::XScale(double a_xScale)
	{
		SetNumber("_xscale", a_xScale);
	}


	double MovieClip::Y() const
	{
		return GetNumber("_y");
	}


	void MovieClip::Y(double a_y)
	{
		SetNumber("_y", a_y);
	}


	double MovieClip::YMouse() const
	{
		return GetNumber("_ymouse");
	}


	double MovieClip::YScale() const
	{
		return GetNumber("_yscale");
	}


	void MovieClip::YScale(double a_yScale)
	{
		SetNumber("_yscale", a_yScale);
	}
}
