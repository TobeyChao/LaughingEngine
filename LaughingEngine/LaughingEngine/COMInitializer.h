#pragma once
#include "PCH.h"

class COMInitializer
{
public:
	COMInitializer();
	~COMInitializer();

private:
	HRESULT hr;
};