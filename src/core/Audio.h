#ifndef AUDIO_H
#define AUDIO_H

#include <memory>
#include <string>

class Audio {
    public:
        enum class Sound {
            Jump,
            Land,
            Coin,
            Powerup,
            Hit,
            Count,
        };

    private:
        struct Impl;
        std::unique_ptr<Impl> _impl;

    public:
        Audio();
        ~Audio();

        Audio(const Audio&)            = delete;
        Audio& operator=(const Audio&) = delete;

        void load(Sound sound, const std::string& path);
        void play(Sound sound);

        void startAmbient(const std::string& path);
        void stopAmbient();

        void setMasterVolume(float volume);
};

#endif