#pragma once
#include <memory>
#include "GameApp.h"

namespace GameSystem {
	class Application
	{
	public:
		static int Launch(std::unique_ptr<GameApp> game);
	};
}