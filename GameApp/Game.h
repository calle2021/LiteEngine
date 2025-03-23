#pragma once
#include "GameSystem.h"

class Game : public GameApp 
{
public:
	Game();
	~Game();
	void Init() override;
	void Update() override;
};