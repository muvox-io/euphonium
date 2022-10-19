// Copyright (c) Kuba Szczodrzyński 2022-1-15.

#pragma once

#include "BaseContainer.h"

class AudioContainers {
  public:
	static std::unique_ptr<BaseContainer> create(const char *mimeType);
};
