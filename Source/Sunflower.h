#pragma once
#include "GameObject.h"

class Sunflower final :
	public GameObject
{
public:
	explicit Sunflower(const std::string& locationFilePath);
};
