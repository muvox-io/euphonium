#ifndef BELL_DISABLE_CODECS
#ifndef DECODER_GLOBALS_H
#define DECODER_GLOBALS_H

#define AAC_READBUF_SIZE (4 * AAC_MAINBUF_SIZE * AAC_MAX_NCHANS)
#define MP3_READBUF_SIZE (2 * 1024);

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include "aacdec.h"
#include "mp3dec.h"

namespace bell
{
    class DecodersInstance
    {
    public:
        DecodersInstance(){};
        ~DecodersInstance()
        {
            MP3FreeDecoder(mp3Decoder);
            AACFreeDecoder(aacDecoder);
        };

        HAACDecoder aacDecoder = NULL;
        HMP3Decoder mp3Decoder = NULL;

        void ensureAAC()
        {
            if (aacDecoder == NULL)
            {
                aacDecoder = AACInitDecoder();
            }
        }

        void ensureMP3()
        {
            if (mp3Decoder == NULL)
            {
                mp3Decoder = MP3InitDecoder();
            }
        }
    };

    extern std::shared_ptr<bell::DecodersInstance> decodersInstance;

    void createDecoders();
}

#endif
#endif
