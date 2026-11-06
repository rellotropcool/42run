#include "core/Audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <iostream>

namespace {
    constexpr int kVoicesPerSound = 4;
    constexpr int kSoundCount     = static_cast<int>(Audio::Sound::Count);
}

struct Audio::Impl {
    ma_engine engine;
    bool      engineInit = false;

    struct Slot {
        ma_sound voices[kVoicesPerSound];
        bool     loaded    = false;
        int      nextVoice = 0;
    };

    Slot slots[kSoundCount];

    ma_sound ambient;
    bool     ambientLoaded  = false;
    bool     ambientPlaying = false;
};

static_assert(kSoundCount == 5,
              "Update Sound enum, load() calls in Application, and slots[] if sounds change");

Audio::Audio() : _impl(std::make_unique<Impl>()) {
    ma_engine_config config = ma_engine_config_init();
    config.periodSizeInMilliseconds = 30;

    if (ma_engine_init(&config, &_impl->engine) != MA_SUCCESS) {
        std::cerr << "Audio: engine init failed, sound disabled\n";
        return;
    }
    _impl->engineInit = true;
}

Audio::~Audio() {
    if (!_impl->engineInit)
        return;

    if (_impl->ambientLoaded)
        ma_sound_uninit(&_impl->ambient);

    for (auto& slot : _impl->slots) {
        if (!slot.loaded)
            continue;
        for (auto& v : slot.voices)
            ma_sound_uninit(&v);
    }

    ma_engine_uninit(&_impl->engine);
}

void Audio::load(Sound sound, const std::string& path) {
    if (!_impl->engineInit)
        return;

    int index = static_cast<int>(sound);
    if (index < 0 || index >= kSoundCount)
        return;

    Impl::Slot& slot = _impl->slots[index];

    if (slot.loaded) {
        for (auto& v : slot.voices)
            ma_sound_uninit(&v);
        slot.loaded    = false;
        slot.nextVoice = 0;
    }

    int loadedCount = 0;

    for (auto& voice : slot.voices) {
        if (ma_sound_init_from_file(&_impl->engine, path.c_str(),
                                    MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                    nullptr, nullptr, &voice) == MA_SUCCESS) {
            ma_sound_set_fade_in_milliseconds(&voice, 0.0f, 1.0f, 3.0f);
            ++loadedCount;
        } else {
            break;
        }
    }

    if (loadedCount == kVoicesPerSound) {
        slot.loaded = true;
    } else {
        for (int i = 0; i < loadedCount; ++i)
            ma_sound_uninit(&slot.voices[i]);
        std::cerr << "Audio: could not load " << path
                  << ", sound disabled\n";
    }
}

void Audio::play(Sound sound) {
    if (!_impl->engineInit)
        return;

    int index = static_cast<int>(sound);
    if (index < 0 || index >= kSoundCount)
        return;

    Impl::Slot& slot = _impl->slots[index];
    if (!slot.loaded)
        return;

    ma_sound* voice = &slot.voices[slot.nextVoice];
    slot.nextVoice  = (slot.nextVoice + 1) % kVoicesPerSound;

    ma_sound_seek_to_pcm_frame(voice, 0);
    ma_sound_set_fade_in_milliseconds(voice, 0.0f, 1.0f, 3.0f);
    ma_sound_start(voice);
}

void Audio::startAmbient(const std::string& path) {
    if (!_impl->engineInit || _impl->ambientPlaying)
        return;

    if (!_impl->ambientLoaded) {
        if (ma_sound_init_from_file(&_impl->engine, path.c_str(),
                                    MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                    nullptr, nullptr, &_impl->ambient) != MA_SUCCESS) {
            std::cerr << "Audio: could not load ambient " << path << "\n";
            return;
        }
        _impl->ambientLoaded = true;
        ma_sound_set_looping(&_impl->ambient, MA_TRUE);
    }

    ma_sound_start(&_impl->ambient);
    _impl->ambientPlaying = true;
}

void Audio::stopAmbient() {
    if (!_impl->engineInit || !_impl->ambientPlaying)
        return;

    ma_sound_stop(&_impl->ambient);
    _impl->ambientPlaying = false;
}

void Audio::setMasterVolume(float volume) {
    if (!_impl->engineInit)
        return;
    ma_engine_set_volume(&_impl->engine, volume);
}