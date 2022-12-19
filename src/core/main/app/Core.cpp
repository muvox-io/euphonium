#include "Core.h"

using namespace euph;

Core::Core() {
  EUPH_LOG(info, TAG, "Initializing context");
  this->ctx = euph::Context::create();
  this->http = std::make_shared<euph::HTTPDispatcher>(this->ctx);
  this->pkgLoader = std::make_shared<euph::PackageLoader>(this->ctx);
  this->bindings = std::make_unique<euph::CoreBindings>(this->ctx);

  this->http->initialize();
  this->pkgLoader->loadValidPackages();

  // setup bindings
  this->bindings->setupBindings();
  this->http->setupBindings();

  this->pkgLoader->loadWithHook("system");
  this->pkgLoader->loadWithHook("plugin");

  this->handleEventLoop();
}

void Core::handleEventLoop() {
  // Subscribe self to the event bus
  auto subscriber = dynamic_cast<EventSubscriber*>(this);
  ctx->eventBus->addListener(EventType::VM_MAIN_EVENT, *subscriber);

  while (true) {
    ctx->eventBus->eventSemaphore->wait();
    ctx->eventBus->update();
  }
}

void Core::handleEvent(std::unique_ptr<Event>& event) {
  EUPH_LOG(debug, TAG, "Got event");
  // Load function
  ctx->vm->get_global("handle_event");

  // Arg 1
  ctx->vm->string(event->subType);

  // Arg 2
  ctx->vm->map(event->toBerry());

  ctx->vm->pcall(2);

  if (be_top(ctx->vm->raw_ptr()) > 0) {
    BELL_LOG(error, TAG, "Berry stack invalid, possible memory leak (%d > 0 !)",
             be_top(ctx->vm->raw_ptr()));
  }
}

Core::~Core() {}