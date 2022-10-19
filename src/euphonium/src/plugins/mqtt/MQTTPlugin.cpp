#include "MQTTPlugin.h"
#include <stdio.h>
#include <sys/types.h>
#include <thread>
#if !defined(WIN32)
#include <netdb.h>
#include <sys/socket.h>
#else
#include <ws2tcpip.h>
#endif
#if defined(__VMS)
#include <ioctl.h>
#endif
#include <BellUtils.h>
#include <fcntl.h>

void publish_callback(void **unused, struct mqtt_response_publish *published) {
    std::string topic((const char *)published->topic_name,
                      published->topic_name_size);
    std::string msg((const char *)published->application_message,
                    published->application_message_size);
    mainEventBus->postEvent(
        std::move(std::make_unique<MQTTPublishReceivedEvent>(topic, msg)));
}

MQTTPlugin::MQTTPlugin() : bell::Task("mqtt", 2048, 0, 0) { name = "mqtt"; }

void MQTTPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {}

void MQTTPlugin::setupBindings() {
    berry->export_this("connect", this, &MQTTPlugin::connectToBroker, "mqtt");
    berry->export_this("publish", this, &MQTTPlugin::publish, "mqtt");
    berry->export_this("subscribe", this, &MQTTPlugin::subscribe, "mqtt");
}

void MQTTPlugin::configurationUpdated() {}

void MQTTPlugin::shutdown() {}

void MQTTPlugin::publish(std::string topic, std::string msg) {
    MQTTMessage message = {
        .type = MQTTMessageType::PUBLISH, .topic = topic, .message = msg};
    mqttQueue.push(message);
}

void MQTTPlugin::subscribe(std::string topic) {
    MQTTMessage message = {
        .type = MQTTMessageType::SUBSCRIBE,
        .topic = topic,
    };
    mqttQueue.push(message);
}

bool MQTTPlugin::connectToBroker(std::string url, std::string port,
                                 std::string user, std::string password) {
    this->status = ModuleStatus::SHUTDOWN;
    std::lock_guard lock(connectedMutex);

    EUPH_LOG(info, "mqtt", "Connecting to MQTT broker");

    try {

        struct addrinfo hints = {0};

        hints.ai_family = AF_UNSPEC;     /* IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
        int sockfd = -1;
        int rv;
        struct addrinfo *p, *servinfo;

        /* get address information */
        rv = getaddrinfo(url.c_str(), port.c_str(), &hints, &servinfo);
        if (rv != 0) {
            EUPH_LOG(error, "mqtt", "Failed to open the address");
            return false;
        }

        /* open the first possible socket */
        for (p = servinfo; p != NULL; p = p->ai_next) {
            sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (sockfd == -1)
                continue;
            /* make non-blocking */
            if (sockfd != -1)
                fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);

            EUPH_LOG(info, "mqtt", "Trying connect");
            /* connect to server */
            fd_set myset;
            struct timeval tv;
            int valopt;
            socklen_t lon;

            rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
            if (rv < 0) {
                if (errno == EINPROGRESS) {
                    do {
                        tv.tv_sec = 3;
                        tv.tv_usec = 0;
                        FD_ZERO(&myset);
                        FD_SET(sockfd, &myset);
                        rv = select(sockfd + 1, NULL, &myset, NULL, &tv);
                        if (rv < 0 && errno != EINTR) {
                            EUPH_LOG(error, "mqtt",
                                     "Failed to open the address");
                            close(sockfd);
                            sockfd = -1;
                            return false;
                        } else if (rv > 0) {
                            // Socket selected for write
                            lon = sizeof(int);
                            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR,
                                           (void *)(&valopt), &lon) < 0) {
                                EUPH_LOG(error, "mqtt",
                                         "Failed to open the address");

                                close(sockfd);
                                sockfd = -1;
                                return false;
                            }
                            // Check the value returned...
                            if (valopt) {
                                EUPH_LOG(error, "mqtt",
                                         "Failed to open the address");

                                close(sockfd);
                                sockfd = -1;
                                return false;
                            }
                            break;
                        } else {
                            EUPH_LOG(error, "mqtt",
                                     "Failed to open the address");

                            close(sockfd);
                            sockfd = -1;
                            return false;
                        }
                    } while (1);
                } else {
                    close(sockfd);
                    sockfd = -1;
                    continue;
                }
            }
        }

        /* free servinfo */
        freeaddrinfo(servinfo);
        EUPH_LOG(info, "mqtt", "Got addr info");

        if (sockfd == -1) {
            EUPH_LOG(error, "mqtt", "Failed to open the socket");
            return false;
        }
        EUPH_LOG(info, "mqtt", "Init called");

        if (mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf,
                      sizeof(recvbuf), publish_callback) != MQTT_OK) {
            EUPH_LOG(error, "mqtt", "Cannot initialize MQTT structure");
            return false;
        }
        EUPH_LOG(info, "mqtt", "Connect called");

        const char *client_id = NULL;
        uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
        if (mqtt_connect(&client, client_id, NULL, NULL, 0, user.c_str(),
                         password.c_str(), connect_flags, 400) != MQTT_OK) {
            EUPH_LOG(info, "mqtt", "mqtt_connect error");
            return false;
        }

        EUPH_LOG(info, "mqtt", "MQTT Connected");

        startAudioThread();
    } catch (...) {
        EUPH_LOG(error, "mqtt", "error occured in connect");
        return false;
    }
    return true;
}

void MQTTPlugin::runTask() {
    status = ModuleStatus::RUNNING;
    std::lock_guard lock(connectedMutex);

    MQTTMessage message;
    mqtt_sync(&client);

    while (status == ModuleStatus::RUNNING) {

        if (this->mqttQueue.pop(message)) {
            if (message.type == MQTTMessageType::PUBLISH) {
                mqtt_publish(&client, message.topic.c_str(),
                             message.message.c_str(), message.message.size(),
                             MQTT_PUBLISH_QOS_0);
            }

            if (message.type == MQTTMessageType::SUBSCRIBE) {
                mqtt_subscribe(&client, message.topic.c_str(),
                               MQTT_PUBLISH_QOS_0);
            }
        } else {
            mqtt_sync(&client);
            BELL_SLEEP_MS(100);
        }
    }
}

void MQTTPlugin::startAudioThread() { startTask(); }
