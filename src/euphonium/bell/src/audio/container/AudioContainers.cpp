// Copyright (c) Kuba Szczodrzyński 2022-1-15.

#include "AudioContainers.h"
#include "Mpeg4Container.h"

std::unique_ptr<BaseContainer> AudioContainers::create(const char *mimeType) {
	char *type = strchr((char *)mimeType, '/');
	if (!type || *(++type) == '\0')
		return nullptr;

	if (strncasecmp(type, "mp4", 3) == 0)
		return std::make_unique<Mpeg4Container>();

	return nullptr;
}
