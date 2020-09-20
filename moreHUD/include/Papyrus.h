#pragma once

#include "SKSE/RegistrationSet.h"


namespace Skywind
{
	class Papyrus
	{
	public:
		static bool Register();
	};
}

class OnBirthSignAcceptHandler : public SKSE::RegistrationSet<uint32_t>
{
public:
	static OnBirthSignAcceptHandler* GetSingleton();

private:
	using Base = SKSE::RegistrationSet<std::uint32_t>;


	OnBirthSignAcceptHandler();
	OnBirthSignAcceptHandler(const OnBirthSignAcceptHandler&) = delete;
	OnBirthSignAcceptHandler(OnBirthSignAcceptHandler&&) = delete;
	~OnBirthSignAcceptHandler() = default;

	OnBirthSignAcceptHandler& operator=(const OnBirthSignAcceptHandler&) = delete;
	OnBirthSignAcceptHandler& operator=(OnBirthSignAcceptHandler&&) = delete;


	static constexpr char EVN_NAME[] = "OnBirthSignAccept";
};
