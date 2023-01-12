#include "StatusLED.h"

using namespace euph;

StatusLED::StatusLED(std::shared_ptr<euph::EventBus> eventBus): bell::Task("StatusLED", 2 * 1024, 0, 0) { 
  this->eventBus = eventBus;
  this->setupTimer();
  this->updateLEDChannel(LEDC_CHANNEL_0, STATUS_R);
  this->updateLEDChannel(LEDC_CHANNEL_1, STATUS_G);
  this->updateLEDChannel(LEDC_CHANNEL_2, STATUS_B);

  this->currentStatus = this->definitions[StatusEvent::WIFI_NO_CONFIG];

  this->statusUpdated = std::make_unique<bell::WrappedSemaphore>(10);
  this->statusUpdated->give();

  ledc_fade_func_install(0);

  // Subscribe self to the event bus
  auto subscriber = static_cast<EventSubscriber*>(this);
  eventBus->addListener(EventType::CONNECTIVITY_EVENT, *subscriber);
  startTask();
}

void StatusLED::handleEvent(std::unique_ptr<Event>& event) {
  if (event->eventType == EventType::CONNECTIVITY_EVENT) {
    auto connectivityEvent = static_cast<Connectivity::ConnectivityEvent*>(event.get());
    auto connectivityData = connectivityEvent->data;

    switch (connectivityData.state) {
      case euph::Connectivity::State::CONNECTED: {
        this->currentStatus = this->definitions[StatusEvent::NORMAL_IDLE];
        break;
      }
      case euph::Connectivity::State::CONNECTED_NO_INTERNET: {
        this->currentStatus = this->definitions[StatusEvent::WIFI_NO_CONFIG];
        break;
      }
      case euph::Connectivity::State::DISCONNECTED: {

        break;
      }
      case euph::Connectivity::State::CONNECTING: {
        this->currentStatus = this->definitions[StatusEvent::WIFI_CONNECTING];
        break;
      }
      default:
        break;
    }
  }
  this->statusUpdated->give();
}

void StatusLED::setupTimer() {
  ledc_timer_config_t ledcTimer = {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .duty_resolution  = LEDC_TIMER_12_BIT,
    .timer_num        = LEDC_TIMER_0,
    .freq_hz          = 5000,  // Set output frequency at 5 kHz
    .clk_cfg          = LEDC_AUTO_CLK
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledcTimer));
}

void StatusLED::updateLEDChannel(ledc_channel_t channel, int gpioNum) {
  ledc_channel_config_t channelConf = {
    .gpio_num       = gpioNum,
    .speed_mode     = LEDC_LOW_SPEED_MODE,
    .channel        = channel,
    .intr_type      = LEDC_INTR_FADE_END,
    .timer_sel      = LEDC_TIMER_0,
    .duty           = 0
  };
  channelConf.flags.output_invert = 1;
  ESP_ERROR_CHECK(ledc_channel_config(&channelConf));
}

void StatusLED::runTask() {
  bool breathingInverted = false;

  while (true) {
    this->statusUpdated->twait();
    int fadeDutyR = this->currentStatus.r * (1<<12) / 255;
    int fadeDutyG = this->currentStatus.g * (1<<12) / 255;
    int fadeDutyB = this->currentStatus.b * (1<<12) / 255;

    if (this->currentStatus.behaviour == Behaviour::ON) {
      ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, fadeDutyR, 0);
      ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, fadeDutyG, 0);
      ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, fadeDutyB, 0);
      BELL_SLEEP_MS(100);
    }

    if (this->currentStatus.behaviour == Behaviour::BREATHING || this->currentStatus.behaviour == Behaviour::BREATHING_FAST) {
      int fadeTime = this->currentStatus.behaviour == Behaviour::BREATHING ? 2000 : 900;

      ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, breathingInverted ? 0 : fadeDutyR, fadeTime));
      ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, breathingInverted ? 0 : fadeDutyG, fadeTime));
      ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, breathingInverted ? 0 : fadeDutyB, fadeTime));

      ESP_ERROR_CHECK(ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT));
      ESP_ERROR_CHECK(ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT));
      ESP_ERROR_CHECK(ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT));
      BELL_SLEEP_MS(fadeTime);
      breathingInverted = !breathingInverted;
    }
    /*ESP_ERROR_CHECK( ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, fade_inverted ? 0 : (1<<12), 1000) );
    ESP_ERROR_CHECK( ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE) );
    fade_inverted = !fade_inverted;*/
  }
}
