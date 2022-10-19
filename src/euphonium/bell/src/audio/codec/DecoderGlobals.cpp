#include "DecoderGlobals.h"

std::shared_ptr<bell::DecodersInstance> bell::decodersInstance;

void bell::createDecoders()
{
    bell::decodersInstance = std::make_shared<bell::DecodersInstance>();
}