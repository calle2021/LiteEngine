#pragma once
#include "GameSystem.h"

class Game : public GameApp 
{
public:
	Game();
	~Game();
	void init() override;
	void update() override;
};