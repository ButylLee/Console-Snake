#include "SoundPlayer.h"
#include "WinHeader.h"
#include <cassert>

namespace
{
	constexpr const char* SoundResource[] =
	{
{
#include "Sounds/SoundAbout.inl"
},
{
#include "Sounds/SoundCancel.inl"
},
{
#include "Sounds/SoundConfirm.inl"
},
{
#include "Sounds/SoundDead.inl"
},
{
#include "Sounds/SoundEntrance.inl"
},
{
#include "Sounds/SoundFood.inl"
},
{
#include "Sounds/SoundIntro.inl"
},
{
#include "Sounds/SoundSwitch.inl"
},
{
#include "Sounds/SoundWin.inl"
},
	};
}

void SoundPlayerBase::play(Sounds sound) noexcept
{
	if (is_muted)
		return;
	[[maybe_unused]] BOOL success;
	success = PlaySoundW((LPCWSTR)SoundResource[static_cast<size_t>(sound)], NULL,
						 SND_ASYNC | SND_MEMORY | SND_NODEFAULT);
	assert(success);
}

void SoundPlayerBase::mute(bool is_mute) noexcept
{
	is_muted = is_mute;
}