#ifndef PICOAUDIO_JSON_H
#define PICOAUDIO_JSON_H

#include <iostream>
#include <iomanip>
#include <optional>

#include "protobuf.h"

void writeJSON(std::ostream &o, AnyRef any);

#endif