#pragma once

#include <mutex>
#include <cJSON.h>

#include "BellTask.h"

// DSP-specific
#include "BellDSP.h"
#include "Compressor.h"
#include "AudioMixer.h"
#include "BiquadCombo.h"
#include "Biquad.h"
#include "JSONTransformConfig.h"

#include "EuphContext.h"
#include "EuphAudioOutput.h"

// Main Task responsible for receiving, processing and pushing audio out.
namespace euph {
class AudioTask : public bell::Task {
public:
  AudioTask(std::shared_ptr<euph::Context>, std::shared_ptr<euph::AudioOutput> euphAudioOutput);
  ~AudioTask();

  void exportBindings();
  void loadDSPFromString(const std::string& str);
  void _configureDSP(std::string dspPreset);
  void runTask() override;
private:
  std::string pipelinePath = "/cfg/dsp_pipeline.json";
  std::mutex dspMutex; 
  std::shared_ptr<euph::Context> ctx;
  std::shared_ptr<euph::AudioOutput> audioOutput;
  std::shared_ptr<bell::AudioPipeline> currentPipeline;
  std::shared_ptr<bell::BellDSP> dsp;
};
}  // namespace euph
