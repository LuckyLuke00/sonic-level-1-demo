#pragma once
#include "GameObject.h"

class Mayflower final :
	public GameObject
{
public:
	explicit Mayflower(const std::string& locationFilePath);
};
