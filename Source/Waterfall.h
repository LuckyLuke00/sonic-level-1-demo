#pragma once
#include "GameObject.h"

class Waterfall final :
	public GameObject
{
public:
	explicit Waterfall(const std::string& locationFilePath);
};
