FROM ubuntu:22.10 AS base

RUN export DEBIAN_FRONTEND=noninteractive \
 && apt-get update \
 && apt-get install --no-install-recommends -y \
    git ca-certificates golang protobuf-compiler \
    libavahi-compat-libdnssd-dev libasound2-dev libasound2-plugins \
    cmake build-essential libssl-dev nodejs curl yarn python3-pip iproute2 \
    pulseaudio pulseaudio-utils sudo

# Install yarn
#RUN curl -sL https://dl.yarnpkg.com/debian/pubkey.gpg | sudo apt-key add - \
#    && echo "deb https://dl.yarnpkg.com/debian/ stable main" | sudo tee /etc/apt/sources.list.d/yarn.list \
#    && apt-get install yarn

## Make and install PortAudio
ADD http://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz /tmp/
RUN cd tmp \
    && tar zxvf pa_stable_v190700_20210406.tgz \
    && cd portaudio/ \
    && ./configure && make -j$(nproc) && make install

WORKDIR /project

ADD requirements.txt .

## install python dependencies
RUN pip install -r requirements.txt

FROM base AS builder

# Add everything to WORKDIR
ADD . .

#RUN git clone https://github.com/feelfreelinux/euphonium.git \
#    && cd euphonium \
#    && git submodule update --init --recursive \
    ## install python dependencies
#    && pip install -r requirements.txt \
#    && pip install python3-protobuf




# cmake fix
#RUN sed -i 's/VERSION 3.18/VERSION 3.16/' CMakeLists.txt
WORKDIR /project/targets/cli

ENV HTTP_PORT=8080
RUN mkdir -p build \
    && cd build \
    && cmake .. -D HTTP_SERVER_PORT=${HTTP_PORT} \
    && make -j$(nproc)

## Create runtime image from here

FROM builder AS runtime

ENV UNAME euphonium

RUN export UNAME=$UNAME UID=1000 GID=1000 && \
    mkdir -p "/home/${UNAME}" && \
    echo "${UNAME}:x:${UID}:${GID}:${UNAME} User,,,:/home/${UNAME}:/bin/bash" >> /etc/passwd && \
    echo "${UNAME}:x:${UID}:" >> /etc/group && \
    mkdir -p /etc/sudoers.d && \
    echo "${UNAME} ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/${UNAME} && \
    chmod 0440 /etc/sudoers.d/${UNAME} && \
    chown ${UID}:${GID} -R /home/${UNAME} && \
    gpasswd -a ${UNAME} audio

COPY docker/pulse-client.conf /etc/pulse/client.conf

USER $UNAME
ENV HOME /home/$UNAME

EXPOSE 8080
CMD ["/project/targets/cli/build/euphoniumcli"]
