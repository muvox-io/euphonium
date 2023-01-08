#pragma once

#include "BellTask.h"
#include "EuphContext.h"
#include "BellDSP.h"
#include "EuphAudioOutput.h"

// Main Task responsible for receiving, processing and pushing audio out.
namespace euph {
class AudioTask : public bell::Task {
public:
  AudioTask(std::shared_ptr<euph::Context>, std::shared_ptr<euph::AudioOutput> euphAudioOutput);
  ~AudioTask();

  void runTask() override;
private:
  std::shared_ptr<euph::Context> ctx;
  std::shared_ptr<euph::AudioOutput> audioOutput;
  std::shared_ptr<bell::BellDSP> dsp;
};
}  // namespace euph
