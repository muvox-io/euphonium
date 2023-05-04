#include "CSpotPlugin.h"
#include <any>
#include <iostream>
#include <mutex>
#include "ConstantParameters.h"
#include "CoreEvents.h"
#include "SpircHandler.h"
#include "TrackPlayer.h"
#include "URLParser.h"

using namespace euph;

CSpotPlugin::CSpotPlugin(std::shared_ptr<euph::Context> ctx)
    : bell::Task("cspot", 32 * 1024, 2, 1) {
  this->ctx = ctx;

  // Can handle context playback via URIs
  this->supportsContextPlayback = true;
  this->authenticatedSemaphore = std::make_unique<bell::WrappedSemaphore>(5);
  this->hashFn = std::hash<std::string_view>();

  // Subscribe to events
  auto subscriber = static_cast<EventSubscriber*>(this);
  ctx->eventBus->addListener(EventType::PLAYBACK_EVENT, *subscriber);
}

CSpotPlugin::~CSpotPlugin() {}

void CSpotPlugin::handleEvent(std::unique_ptr<Event>& event) {
  if (event->subType == "trackHashChange") {
    auto hashEvent = static_cast<TrackHashChangeEvent*>(event.get());
    if (this->expectedTrackHash == hashEvent->hash) {

      // Next track has been reached
      if (this->spircHandler != nullptr) {
        this->spircHandler->notifyAudioReachedPlayback();

        // Post the next track info
        this->ctx->eventBus->postEvent(
            std::make_unique<TrackInfoEvent>(this->nextTrackInfo));
      }
    }
  }
}

void CSpotPlugin::queryContextURI(std::string uri) {
  // Handle context playback
  if (this->spircHandler == nullptr) {
    if (!authenticationLoaded) {
      // No saved authentication data, can't do anything
      return;
    }

    // Authenticate with spotify
    this->requestedTrackId = uri;
    this->authenticatedSemaphore->give();
  } else {

  }
}

void CSpotPlugin::_authenticateSaved(std::string authData) {
  if (this->spircHandler == nullptr && this->loginBlob != nullptr) {
    this->loginBlob->loadJson(authData);
    this->authenticationLoaded = true;
  }
}

void CSpotPlugin::_startPlugin(std::string name) {
  // Ensure not already running
  if (this->loginBlob != nullptr) {
    return;
  }

  this->loginBlob = std::make_shared<cspot::LoginBlob>(name);
  this->startTask();
}

std::string CSpotPlugin::_authenticateZeroconf(berry::map info) {
  std::map<std::string, std::string> authRequestBody;

  for (auto& [key, value] : info) {
    if (std::any_cast<std::string>(&value) != nullptr) {
      std::string castValue = std::any_cast<std::string>(value);

      // Formencoded data is url encoded
      authRequestBody[key] = bell::URLParser::urlDecode(castValue);
    }
  }

  // At this point we have received the login blob from spotify
  loginBlob->loadZeroconfQuery(authRequestBody);
  this->authenticatedSemaphore->give();

  return loginBlob->toJson();
}

std::string CSpotPlugin::_getSpotifyInfo() {
  if (this->loginBlob == nullptr) {
    return "";
  }

  // Return spotify's status, used for zeroconf auto discovery
  return this->loginBlob->buildZeroconfInfo();
}

std::string CSpotPlugin::getName() {
  return "cspot";
}

void CSpotPlugin::shutdownAudio() {
  isRunning = false;
  std::scoped_lock lock(runningMutex);
}

size_t CSpotPlugin::handleAudioData(uint8_t* data, size_t len,
                                    std::string_view trackId) {
  // Calculate the hash of the trackId
  auto hash = hashFn(trackId);
  if (this->expectedTrackHash != hash) {
    this->expectedTrackHash = hash;
  }

  // Write the PCM data to the audio buffer
  return this->ctx->audioBuffer->writePCM(data, len, hash);
}

void CSpotPlugin::handleCSpotEvent(
    std::unique_ptr<cspot::SpircHandler::Event> event) {
  switch (event->eventType) {
    case cspot::SpircHandler::EventType::PLAY_PAUSE:
      if (std::get<bool>(event->data)) {
        this->ctx->playbackController->pause();
      } else {
        this->ctx->playbackController->play();
      }
      break;
    case cspot::SpircHandler::EventType::DISC:
      this->ctx->audioBuffer->clearBuffer();
      this->ctx->playbackController->pause();
      break;
    case cspot::SpircHandler::EventType::VOLUME: {
      int volume = std::get<int>(event->data) * 100 / MAX_VOLUME;
      auto volumeEvent =
          std::make_unique<AudioVolumeEvent>(volume, AudioVolumeEvent::REMOTE);

      this->ctx->eventBus->postEvent(std::move(volumeEvent));
      break;
    }
    case cspot::SpircHandler::EventType::SEEK:
      this->ctx->audioBuffer->clearBuffer();
      break;
    case cspot::SpircHandler::EventType::TRACK_INFO: {
      // auto spotifyTrack =
      //     std::get<cspot::CDNTrackStream::TrackInfo>(event->data);
      // // Prepare the track info event
      // this->nextTrackInfo =
      //     TrackInfoEvent::TrackInfo{.uri = "cspot://" + spotifyTrack.trackId,
      //                               .title = spotifyTrack.name,
      //                               .artist = spotifyTrack.artist,
      //                               .album = spotifyTrack.album,
      //                               .iconURL = spotifyTrack.imageUrl,
      //                               .canPlay = true,
      //                               .dontCache = false};
      break;
    }
    case cspot::SpircHandler::EventType::PLAYBACK_START:
      this->ctx->audioBuffer->clearBuffer();
      break;
    default:
      break;
  }
}

void CSpotPlugin::initializeBindings() {
  EUPH_LOG(info, TASK, "Initializing bindings");
  ctx->vm->export_this("_run_plugin", this, &CSpotPlugin::_startPlugin,
                       "cspot");
  ctx->vm->export_this("_get_spotify_info", this, &CSpotPlugin::_getSpotifyInfo,
                       "cspot");
  ctx->vm->export_this("_authenticate_zeroconf", this,
                       &CSpotPlugin::_authenticateZeroconf, "cspot");
  ctx->vm->export_this("_authenticate_json", this,
                       &CSpotPlugin::_authenticateSaved, "cspot");
}

void CSpotPlugin::runTask() {
  while (true) {
    this->authenticatedSemaphore->wait();
    isRunning = true;
    std::scoped_lock lock(this->runningMutex);
    auto cspotCtx = cspot::Context::createFromBlob(loginBlob);

    // Ensure access to audio buffer, disable other sources
    this->ctx->playbackController->lockPlayback("cspot");
    this->ctx->audioBuffer->clearBuffer();

    EUPH_LOG(info, TASK, "Creating player");
    // Start spotify session
    cspotCtx->session->connectWithRandomAp();

    // get auth token
    auto token = cspotCtx->session->authenticate(loginBlob);

    // Auth successful
    if (token.size() > 0) {
      this->spircHandler = std::make_shared<cspot::SpircHandler>(cspotCtx);

      // Handle audio data
      this->spircHandler->getTrackPlayer()->setDataCallback(
          [this](uint8_t* data, size_t len, std::string_view trackId) {
            return this->handleAudioData(data, len, trackId);
          });

      // Handle control events
      this->spircHandler->setEventHandler(
          [this](auto event) { this->handleCSpotEvent(std::move(event)); });
      
      // Update volume on remote
      this->spircHandler->setRemoteVolume(this->ctx->playbackController->currentVolume * MAX_VOLUME / 100);

      // Start handling mercury messages
      cspotCtx->session->startTask();

      // If we wanted to handle multiple devices, we would halt this loop
      // when a new zeroconf login is requested, and reinitialize the session
      while (isRunning) {
        cspotCtx->session->handlePacket();
      }
      this->spircHandler->disconnect();
      this->spircHandler = nullptr;
    }

    // Unlock access to audio buffer
    this->ctx->audioBuffer->unlockAccess();
    this->ctx->playbackController->unlockPlayback();
  }
}
