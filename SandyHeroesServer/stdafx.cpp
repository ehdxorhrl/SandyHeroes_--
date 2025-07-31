#include "stdafx.h"

std::random_device kRandomDevice;
std::mt19937 kRandomGenerator(kRandomDevice());