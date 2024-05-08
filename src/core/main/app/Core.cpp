#include "Core.h"
#include <memory>
#include "BerryBind.h"
#include "CoreEvents.h"
#include "EmergencyMode.h"
#include "EventBus.h"
#include "LocalOTAEndpoints.h"
#include "OTAHandler.h"
#include "URLParser.h"
#include "X509Bundle.h"
#include "berry.h"

using namespace euph;

Core::Core(std::shared_ptr<euph::Connectivity> connectivity,
           std::shared_ptr<euph::EventBus> eventBus,
           std::shared_ptr<euph::AudioOutput> euphAudioOutput) {
  bell::createDecoders();

  this->eventBus = eventBus;
  this->connectivity = connectivity;
  this->audioOutput = euphAudioOutput;
}

void Core::initialize() {
  if (this->ctx != nullptr) {
    EUPH_LOG(info, TAG, "Context already initialized");
    return;
  }

  EUPH_LOG(info, TAG, "Initializing context");
  this->ctx = euph::Context::createWithBus(this->eventBus);
  ctx->connectivity = this->connectivity;

  this->http = std::make_shared<euph::HTTPDispatcher>(this->ctx);
  this->pkgLoader = std::make_shared<euph::PackageLoader>(this->ctx);
  this->bindings = std::make_unique<euph::CoreBindings>(this->ctx);
  this->otaHandler = std::make_unique<euph::OTAHandler>(this->ctx);
  this->audioTask =
      std::make_shared<euph::AudioTask>(this->ctx, this->audioOutput);

  // Register sources
  this->registerAudioSource(std::make_unique<RadioPlugin>(ctx));
  this->registerAudioSource(std::make_unique<CSpotPlugin>(ctx));
  this->registerAudioSource(std::make_unique<SnapcastPlugin>(ctx));

  // Register system plugins
  this->registerSystemPlugin(std::make_unique<MQTTPlugin>(ctx));

  this->audioOutput->setupBindings(ctx);

  // Register HTTP handlers for OTA, update packages if necessary
  this->otaHandler->initialize(this->http->getServer());

  registerLocalOTAEndpoints(*this->http->getServer(), this->ctx);

  // Check if contains X509 SSL bundle

  std::ifstream bundleFile(this->ctx->rootPath +
                           "/pkgs/system/x509_crt_bundle");

  if (bundleFile.is_open()) {
    EUPH_LOG(info, this->TAG, "SSL bundle read successfully, attaching");
    std::vector<uint8_t> bundleContents(
        (std::istreambuf_iterator<char>(bundleFile)),
        std::istreambuf_iterator<char>());

    bell::X509Bundle::init(bundleContents.data(), bundleContents.size());
  } else {
    EUPH_LOG(info, this->TAG,
             "WARNING: SSL bundle not found, booting without SSL verification");
  }

  // For handling source switching
  this->ctx->playbackController->playbackLockedHandler =
      [this](const std::string& source) {
        for (auto& audioSource : this->audioSources) {
          // Signal every audio source, except for the origin to shutdown playback
          if (audioSource->getName() != source) {
            audioSource->shutdownAudio();
          }
        }
      };

  try {
    this->pkgLoader->loadValidPackages();
  } catch (...) {
    EUPH_LOG(
        error, TAG,
        "Cannot access file system, make sure the device is flashed properly.");
  }

  // Allow connectivity to register HTTP handlers
  this->connectivity->registerHandlers(this->http->getServer());

  // setup bindings
  this->bindings->setupBindings();
  this->http->setupBindings();

  if (this->exportPlatformBindings != nullptr) {
    this->exportPlatformBindings(this->ctx);
  }

  for (auto& plugin : this->systemPlugins) {
    plugin->initializeBindings();
  }

  for (auto& source : this->audioSources) {
    source->initializeBindings();
  }

  // Register an 'system' endpoint in HTTPDispatcher
  this->http->getServer()->registerGet(
      "/system/info", [this](struct mg_connection* conn) {
        nlohmann::json state = {
            {"connectivity", this->connectivity->getData().toJson()},
            {"version", "0.1.0"},
            {"onboarding", false},
            {"emergencyMode", ctx->emergencyMode->getJsonStatus()},
        };
        return this->http->getServer()->makeJsonResponse(state.dump());
      });

  try {

    // Load system packages
    this->pkgLoader->loadWithHook("system", true);
    this->pkgLoader->loadWithHook("plugin");

#ifdef ESP_PLATFORM
    this->pkgLoader->loadWithHook("plugin_esp32");
    this->pkgLoader->loadWithHook("platform_esp32", true);
#else
    this->pkgLoader->loadWithHook("platform_desktop", true);
#endif

  } catch (berry::BerryErrorException& e) {
    this->ctx->emergencyMode->trip(EmergencyModeReason::BERRY_INIT_ERROR,
                                   e.what());
  } catch (PackageLoaderFileNotFoundException& e) {
    this->ctx->emergencyMode->trip(
        EmergencyModeReason::LOADING_BERRY_HOOK_FAILED, e.what());
  }

  // Initialize HTTP
  this->http->initialize();

  initializePlugins();
}

void Core::initializePlugins() {
  connectivity->displayNameLoaded(this->ctx->displayName);

  // Only initialize plugins when connected completely
  if ((connectivity->data.state == Connectivity::State::CONNECTED ||
       connectivity->data.state ==
           Connectivity::State::CONNECTED_NO_INTERNET) &&
      !pluginsInitialized) {
    // Initialize plugins
    auto event = std::make_unique<GenericVmEvent>("plugins_ready");
    eventBus->postEvent(std::move(event));
    pluginsInitialized = true;
  }
}

void Core::handleEventLoop() {
  // Subscribe self to the event bus
  auto subscriber = static_cast<EventSubscriber*>(this);
  eventBus->addListener(EventType::VM_MAIN_EVENT, *subscriber);
  eventBus->addListener(EventType::CONNECTIVITY_EVENT, *subscriber);
  eventBus->addListener(EventType::PLAYBACK_EVENT, *subscriber);
  eventBus->addListener(EventType::VM_RAW_COMMAND_EVENT, *subscriber);

  while (true) {
    eventBus->eventSemaphore->wait();
    eventBus->update();
  }
}

void Core::registerSystemPlugin(std::unique_ptr<euph::SystemPlugin> plugin) {
  this->systemPlugins.push_back(std::move(plugin));
}

void Core::registerAudioSource(
    std::unique_ptr<euph::AudioSourcePlugin> source) {
  this->audioSources.push_back(std::move(source));
}

void Core::handleEvent(std::unique_ptr<Event>& event) {
  EUPH_LOG(debug, TAG, "EventBus message received");

  switch (event->eventType) {
    case EventType::VM_MAIN_EVENT: {
      // Load function
      ctx->vm->get_global("handle_event");

      // Arg 1
      ctx->vm->string(event->subType);

      // Arg 2
      ctx->vm->map(event->toBerry());

      ctx->vm->pcall(2);

      if (be_top(ctx->vm->raw_ptr()) > 0) {
        BELL_LOG(error, TAG,
                 "Berry stack invalid, possible memory leak (%d > 0 !)",
                 be_top(ctx->vm->raw_ptr()));
      }
      break;
    }
    case EventType::VM_RAW_COMMAND_EVENT: {
      auto rawEvent = static_cast<VmRawCommandEvent*>(event.get());
      this->ctx->vm->execute_string(rawEvent->command, "repl");
      if (be_top(ctx->vm->raw_ptr()) > 0) {
        be_pop(ctx->vm->raw_ptr(), be_top(ctx->vm->raw_ptr()));
      }
      break;
    }
    case EventType::PLAYBACK_EVENT: {
      if (event->subType == "contextURI") {
        auto uriEvent = static_cast<ContextURIEvent*>(event.get());

        // Parse URI
        auto schemaIndex = uriEvent->uri.find_first_of("://");
        if (schemaIndex == std::string::npos ||
            schemaIndex + 3 >= uriEvent->uri.size()) {
          EUPH_LOG(error, TAG, "Invalid Context URI: %s",
                   uriEvent->uri.c_str());
          break;
        }

        // Find name of the player
        std::string playerName = uriEvent->uri.substr(0, schemaIndex);
        std::string playerUri =
            URLParser::urlDecode(uriEvent->uri.substr(schemaIndex + 3));
        EUPH_LOG(info, TAG, "Received context URI {%s}", uriEvent->uri.c_str());

        // Find audio source to query
        for (auto& source : this->audioSources) {
          if (source->getName() == playerName &&
              source->supportsContextPlayback) {
            source->queryContextURI(playerUri);
            break;
          }
        }
      }
      break;
    }
    case EventType::CONNECTIVITY_EVENT: {
      auto connectivityEvent =
          static_cast<Connectivity::ConnectivityEvent*>(event.get());
      auto connectivityData = connectivityEvent->data;

      switch (connectivityData.state) {
        case euph::Connectivity::State::CONNECTED: {
          this->initialize();
          EUPH_LOG(info, TAG, "Connected to %s", connectivityData.ssid.c_str());
          break;
        }
        case euph::Connectivity::State::CONNECTED_NO_INTERNET: {
          this->initialize();
          EUPH_LOG(info, TAG, "Connected without internet connection");
          break;
        }
        case euph::Connectivity::State::DISCONNECTED: {
          EUPH_LOG(info, TAG, "Disconnected");
          break;
        }
        case euph::Connectivity::State::CONNECTING: {
          EUPH_LOG(info, TAG, "Connecting to %s",
                   connectivityData.ssid.c_str());
          break;
        }
        default:
          break;
      }

      // Broadcast event to websocket
      if (this->http != nullptr) {
        auto connectivityDataJson = this->connectivity->getData().toJson();
        nlohmann::json websocketEvent = {{"type", "connectivity"},
                                         {"data", connectivityDataJson}};
        this->http->broadcastWebsocket(websocketEvent.dump());
      }
      break;
    }
    default:
      break;
  }
}

Core::~Core() {}
