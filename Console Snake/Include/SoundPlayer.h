#pragma once
#ifndef SNAKE_SOUNDPLAYER_HEADER_
#define SNAKE_SOUNDPLAYER_HEADER_

#include "Modules.h"

enum struct Sounds :size_t
{
	About,
	Cancel,
	Confirm,
	Dead,
	Entrance,
	Food,
	Intro,
	Switch,
	Win,
};

class SoundPlayerBase
{
protected:
	SoundPlayerBase() = default;

public:
	void play(Sounds) noexcept;
	void mute(bool) noexcept;

private:
	bool is_muted = false;
};

using SoundPlayer = ModuleRegister<SoundPlayerBase>;

#endif // SNAKE_SOUNDPLAYER_HEADER_