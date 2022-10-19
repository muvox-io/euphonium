// Copyright (c) Kuba Szczodrzyński 2022-1-16.

#include "WebmContainer.h"

uint32_t WebmContainer::readVarNum32(bool raw) {
	uint32_t result = readUint8();
	if (!result) {
		closed = true;
		return 0;
	}
	uint8_t len = 0;
	for (; !(result >> (7 - len)); len++) {}
	if (!raw)
		result &= ~(1 << (7 - len));
	for (uint8_t i = 0; i < len; i++) {
		result <<= 8;
		result |= readUint8();
	}
	return result;
}

uint64_t WebmContainer::readVarNum64() {
	uint64_t result = readUint8();
	if (!result) {
		closed = true;
		return 0;
	}
	uint8_t len = 0;
	for (; !(result >> (7 - len)); len++) {}
	result &= ~(1 << (7 - len));
	for (uint8_t i = 0; i < len; i++) {
		result <<= 8;
		result |= readUint8();
	}
	return result;
}

uint32_t WebmContainer::readUint(uint8_t len) {
	if (len >= 4) {
		skipBytes(len - 4);
		return readUint32();
	}
	if (len == 3)
		return readUint24();
	if (len == 2)
		return readUint16();
	return readUint8();
}

uint64_t WebmContainer::readUlong(uint8_t len) {
	if (len == 8)
		return readUint64();
	return readUint(len);
}

float WebmContainer::readFloat(uint8_t len) {
	double result = 0;
	auto *b = (uint8_t *)&result;
	for (uint8_t i = 0; i < len; i++)
		b[len - i - 1] = readUint8();
	return (float)result;
}

void WebmContainer::readElem() {
	eid = (ElementId)readVarNum32(true);
	esize = readVarNum32();
}
