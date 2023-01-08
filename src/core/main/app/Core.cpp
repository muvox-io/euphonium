#include "Core.h"
#include "DecoderGlobals.h"

using namespace euph;

Core::Core(std::shared_ptr<euph::Connectivity> connectivity, std::shared_ptr<euph::EventBus> eventBus) {
  bell::createDecoders();
  this->eventBus = eventBus;
  this->connectivity = connectivity;

  // Start main event loop
  this->handleEventLoop();
}

void Core::initialize() {
  if (this->ctx != nullptr) {
    EUPH_LOG(info, TAG, "Context already initialized");
    return;
  }

  EUPH_LOG(info, TAG, "Initializing context");
  this->ctx = euph::Context::createWithBus(this->eventBus);

  this->http = std::make_shared<euph::HTTPDispatcher>(this->ctx);
  this->pkgLoader = std::make_shared<euph::PackageLoader>(this->ctx);
  this->bindings = std::make_unique<euph::CoreBindings>(this->ctx);
  this->audioTask = std::make_shared<euph::AudioTask>(this->ctx);

  // Register sources
  this->audioSources.push_back(std::make_unique<RadioPlugin>(ctx));

  this->http->initialize();

  try {
    this->pkgLoader->loadValidPackages();
  } catch (...) {
    EUPH_LOG(error, TAG, "Cannot access file system, make sure the device is flashed properly.");
  }

  for (auto& source : this->audioSources) {
    source->initializeBindings();
  }

  // Allow connectivity to register HTTP handlers
  this->connectivity->registerHandlers(this->http->getServer());

  // setup bindings
  this->bindings->setupBindings();
  this->http->setupBindings();

  // Register an 'system' endpoint in HTTPDispatcher
  this->http->getServer()->registerGet(
      "/system/info", [this](struct mg_connection* conn) {
        nlohmann::json state = {
          { "connectivity", this->connectivity->getData().toJson() },
          { "version", "0.1.0" },
          { "onboarding", false }
        };
        return this->http->getServer()->makeJsonResponse(state.dump());
      });

  // Load system packages
  this->pkgLoader->loadWithHook("system");
  this->pkgLoader->loadWithHook("plugin");

  // Initialize plugins
  auto event = std::make_unique<GenericVmEvent>("plugins_ready");
  eventBus->postEvent(std::move(event));
}

void Core::handleEventLoop() {
  // Subscribe self to the event bus
  auto subscriber = static_cast<EventSubscriber*>(this);
  eventBus->addListener(EventType::VM_MAIN_EVENT, *subscriber);
  eventBus->addListener(EventType::CONNECTIVITY_EVENT, *subscriber);

  while (true) {
    eventBus->eventSemaphore->wait();
    eventBus->update();
  }
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
        nlohmann::json websocketEvent = {
          { "type", "connectivity" },
          { "data", connectivityDataJson }          
        };
        this->http->broadcastWebsocket(websocketEvent.dump());
      }
      break;
    }
    default:
      break;
  }
}

Core::~Core() {}
