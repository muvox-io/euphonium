#include "MQTTPlugin.h"
#include <thread>
#include <stdio.h>
#include <sys/types.h>
#if !defined(WIN32)
#include <sys/socket.h>
#include <netdb.h>
#else
#include <ws2tcpip.h>
#endif
#if defined(__VMS)
#include <ioctl.h>
#endif
#include <fcntl.h>
#include <BellUtils.h>

void publish_callback(void** unused, struct mqtt_response_publish *published) {
    std::string topic((const char*) published->topic_name, published->topic_name_size);
    std::string msg((const char*) published->application_message, published->application_message_size);
    mainEventBus->postEvent(std::move(std::make_unique<MQTTPublishReceivedEvent>(topic, msg)));
}


MQTTPlugin::MQTTPlugin() : bell::Task("mqtt",  2048, 0, 0) {
    name = "mqtt";
}

void MQTTPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
}

void MQTTPlugin::setupBindings() {
    berry->export_this("connect", this, &MQTTPlugin::connectToBroker, "mqtt");
    berry->export_this("publish", this, &MQTTPlugin::publish, "mqtt");
    berry->export_this("subscribe", this, &MQTTPlugin::subscribe, "mqtt");
}

void MQTTPlugin::configurationUpdated() {}


void MQTTPlugin::shutdown() {
}

void MQTTPlugin::publish(std::string topic, std::string msg) {
    MQTTMessage message = {
        .type = MQTTMessageType::PUBLISH,
        .topic = topic,
        .message = msg
    };
    mqttQueue.push(message);
}

void MQTTPlugin::subscribe(std::string topic) {
    MQTTMessage message = {
        .type = MQTTMessageType::SUBSCRIBE,
        .topic = topic,
    };
    mqttQueue.push(message);
}

void MQTTPlugin::connectToBroker(std::string url, std::string port, std::string user, std::string password) {
    EUPH_LOG(info, "mqtt", "Connecting to MQTT broker");
    
    struct addrinfo hints = {0};

    hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int sockfd = -1;
    int rv;
    struct addrinfo *p, *servinfo;

    /* get address information */
    rv = getaddrinfo(url.c_str(), port.c_str(), &hints, &servinfo);
    if(rv != 0) {
        EUPH_LOG(error, "mqtt", "Failed to open the address");
    }

    /* open the first possible socket */
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) continue;

        /* connect to server */
        rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if(rv == -1) {
          close(sockfd);
          sockfd = -1;
          continue;
        }
        break;
    }  

    /* free servinfo */
    freeaddrinfo(servinfo);

    /* make non-blocking */
    if (sockfd != -1) fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);

    if (sockfd == -1) {
        EUPH_LOG(error, "mqtt", "Failed to open the socket");
        return;
    }

    if (mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback) != MQTT_OK) {
        EUPH_LOG(error, "mqtt", "Cannot initialize MQTT structure");
        return;
    }

    const char* client_id = NULL;
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    if (mqtt_connect(&client, client_id, NULL, NULL, 0, user.c_str(), password.c_str(), connect_flags, 400) != MQTT_OK) {
        EUPH_LOG(info, "mqtt", "mqtt_connect error");
    }

    startAudioThread();
}

void MQTTPlugin::runTask() {
    status = ModuleStatus::RUNNING;

    MQTTMessage message;
    while (status == ModuleStatus::RUNNING) {
        if (this->mqttQueue.pop(message)) {
            if (message.type == MQTTMessageType::PUBLISH) {
                mqtt_publish(&client, message.topic.c_str(), message.message.c_str(), message.message.size(), MQTT_PUBLISH_QOS_0);
            }

            if (message.type == MQTTMessageType::SUBSCRIBE) {
                mqtt_subscribe(&client, message.topic.c_str(), MQTT_PUBLISH_QOS_0);
            }
        } else {
            mqtt_sync(&client);
            BELL_SLEEP_MS(100);
        }
    }
}

void MQTTPlugin::startAudioThread() { startTask(); }
