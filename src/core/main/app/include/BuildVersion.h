#pragma once

namespace euph::build {
constexpr bool isDebug() {
#ifdef EUPH_DEBUG
  return true;
#else
  return false;
#endif
}
}  // namespace euph::build