// Copyright (c) Kuba Szczodrzyński 2022-1-12.

#pragma once

#include "BaseCodec.h"
#include "BaseContainer.h"
#include <memory>

enum class AudioCodec {
	UNKNOWN = 0,
	AAC = 1,
	MP3 = 2,
	VORBIS = 3,
	OPUS = 4,
	FLAC = 5,
};

class AudioCodecs {
  public:
	static std::shared_ptr<BaseCodec> getCodec(AudioCodec type);
	static std::shared_ptr<BaseCodec> getCodec(BaseContainer *container);
	static void addCodec(AudioCodec type, const std::shared_ptr<BaseCodec> &codec);
};
