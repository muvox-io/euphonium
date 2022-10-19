// Copyright (c) Kuba Szczodrzyński 2022-1-7.

#include "BaseContainer.h"

void BaseContainer::feed(const std::shared_ptr<bell::ByteStream> &stream, uint32_t position) {
	this->reader = std::make_unique<bell::BinaryReader>(stream);
	this->source = stream;
	this->pos = position;
}

// TODO move source stream reading here, and set closed = true when stream ends

uint8_t BaseContainer::readUint8() {
	pos += 1;
	return reader->readByte();
}

uint16_t BaseContainer::readUint16() {
	pos += 2;
	return reader->readShort();
}

uint32_t BaseContainer::readUint24() {
	uint8_t b[3];
	readBytes(b, 3);
	return static_cast<int32_t>((b[2]) | (b[1] << 8) | (b[0] << 16));
}

uint32_t BaseContainer::readUint32() {
	pos += 4;
	return reader->readUInt();
}

uint64_t BaseContainer::readUint64() {
	pos += 8;
	return reader->readLong();
}

uint32_t BaseContainer::readVarint32() {
	uint8_t b = readUint8();
	uint32_t result = b & 0x7f;
	while (b & 0b10000000) {
		b = readUint8();
		result <<= 7;
		result |= b & 0x7f;
	}
	return result;
}

uint32_t BaseContainer::readBytes(uint8_t *dst, uint32_t num) {
	if (!num)
		return 0;
	uint32_t len, total;
	do {
		if (dst) {
			len = source->read(dst, num);
			dst += len; // increment destination pointer
		} else {
			len = source->skip(num);
		}
		total += len; // increment total read count
		pos += len;	  // increment absolute source position
		num -= len;	  // decrement bytes left to read
	} while (len && num);
	if (!len) // source->read() returned 0, it's closed
		closed = true;
	return len;
}

uint32_t BaseContainer::skipBytes(uint32_t num) {
	return readBytes(nullptr, num);
}

uint32_t BaseContainer::skipTo(uint32_t offset) {
	if (offset <= pos)
		return 0;
	return readBytes(nullptr, offset - pos);
}
