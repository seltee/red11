// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "utils/utils.h"
#include "utils/primitives.h"
#include "utils/destroyable.h"
#include "data/sound.h"
#include "audioHelper.h"

class AudioSource : public Destroyable
{
public:
    EXPORT AudioSource(AudioFormat reqFormat, int freq, int channels);
    virtual ~AudioSource();

    EXPORT void fillBuffer(unsigned char *buffer, int lengthInSamples);

    EXPORT void process(float delta);

    inline void setPosition(const Vector3 &vPosition) { this->vPosition = vPosition; }
    inline Vector3 getPosition() { return this->vPosition; }

    inline void setVolume(float volume) { this->volume = volume; }
    inline float getVolume() { return this->volume; }

    inline void setMaxDistance(float maxDistance) { this->maxDistance = maxDistance; }
    inline float getMaxDistance() { return this->maxDistance; }
    inline void setReferenceDistance(float referenceDistance) { this->refDistance = referenceDistance; }
    inline float getReferenceDistance() { return this->refDistance; }

    inline bool isLooping() { return this->bIsLooping; }

    inline AudioSourceState getState() { return state; }
    inline Sound *getSound() { return this->sound; }

    EXPORT void play(Sound *sound);
    EXPORT void playAndDie(Sound *sound);
    EXPORT void play3d(Sound *sound, const Vector3 &vPosition);
    EXPORT void play3dAndDie(Sound *sound, const Vector3 &vPosition);
    EXPORT void loop(Sound *sound);
    EXPORT void stop();

    inline bool is3dPositioned() { return bIs3dPositioned; }

protected:
    void setupBuffers(int bufferSize);

    bool bIsLooping = false;
    bool bDestroyAfterPlaying = false;

    AudioSourceState state = AudioSourceState::Stopped;
    Sound *sound = nullptr;

    static std::vector<unsigned int> buffers;

    SoundStream *soundStream = nullptr;
    float playPosition = 0.0f;
    float playStep = 0.0f;

    AudioFormat reqFormat = AudioFormat::UNKNOWN;
    int freq = 0;
    int channels = 0;

    float volume = 1.0f;

    float maxDistance = 10.0f;
    float refDistance = 1.0f;

    bool bIs3dPositioned = false;

    Vector3 vPosition = Vector3(0.0f, 0.0f, 0.0f);

    unsigned char *streamBuffers[2] = {nullptr, nullptr};
    int bufferAudioBytes[2] = {0, 0};

    int bufferSize = 0;
    int currentBuffer = 0;
    int lastLoadedBuffer = 0;
};
