#pragma once

#include "BellTask.h"
#include "EuphContext.h"
#include "AudioSink.h"
#include "BellDSP.h"

// Main Task responsible for receiving, processing and pushing audio out.
namespace euph {
class AudioTask : public bell::Task {
public:
  AudioTask(std::shared_ptr<euph::Context>);
  ~AudioTask();

  void runTask() override;
private:
  std::shared_ptr<euph::Context> ctx;
  std::shared_ptr<AudioSink> audioSink;
  std::shared_ptr<bell::BellDSP> dsp;
};
}  // namespace euph
