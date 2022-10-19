// Copyright (c) Kuba Szczodrzyński 2021-12-21.

#include "BellSocket.h"
#include <cstring>

void bell::Socket::open(const std::string &url) {
	auto *urlStr = url.c_str();
	bool https = urlStr[4] == 's';
	uint16_t port = https ? 443 : 80;
	auto *hostname = urlStr + (https ? 8 : 7);
	auto *hostnameEnd = strchr(hostname, ':');
	auto *path = strchr(hostname, '/');
	if (hostnameEnd == nullptr) {
		hostnameEnd = path;
	} else {
		port = strtol(hostnameEnd + 1, nullptr, 10);
	}
	auto hostnameStr = std::string(hostname, (const char *)hostnameEnd);

	this->open(hostnameStr, port);
}
