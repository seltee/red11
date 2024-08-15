// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "audio/audioConversion.h"
#include "audio/audioSource.h"

AudioSource::AudioSource(AudioFormat reqFormat, int freq, int channels)
{
    this->reqFormat = reqFormat;
    this->freq = freq;
    this->channels = channels;
}

AudioSource::~AudioSource()
{
    if (streamBuffers[0])
        delete streamBuffers[0];
    if (streamBuffers[1])
        delete streamBuffers[1];
}

void AudioSource::fillBuffer(unsigned char *buffer, int lengthInSamples)
{
    float *fbuffer = reinterpret_cast<float *>(buffer);
    bool isFinished = false;
    int sample = 0;

    if (reqFormat == AudioFormat::STEREO_32F && sound)
    {
        unsigned char *dataSource = soundStream ? streamBuffers[currentBuffer] : sound->getData();
        int dataSize = soundStream ? bufferAudioBytes[currentBuffer] : sound->getDataSize();

        while (sample < lengthInSamples)
        {
            switch (sound->getAudioFormat())
            {
            case AudioFormat::MONO_8:
                isFinished = fillFloatStereoBufferFromMono8(dataSource, dataSize, playPosition, sample, playStep, fbuffer, lengthInSamples, volume);
                break;
            case AudioFormat::MONO_16:
                isFinished = fillFloatStereoBufferFromMono16(dataSource, dataSize, playPosition, sample, playStep, fbuffer, lengthInSamples, volume);
                break;
            case AudioFormat::MONO_32:
                isFinished = fillFloatStereoBufferFromMono32(dataSource, dataSize, playPosition, sample, playStep, fbuffer, lengthInSamples, volume);
                break;
            case AudioFormat::MONO_32F:
                isFinished = fillFloatStereoBufferFromMono32Float(dataSource, dataSize, playPosition, sample, playStep, fbuffer, lengthInSamples, volume);
                break;
            case AudioFormat::STEREO_8:
                isFinished = fillFloatStereoBufferFromStereo8(dataSource, dataSize, playPosition, sample, playStep, fbuffer, lengthInSamples, volume);
                break;
            case AudioFormat::STEREO_16:
            {
                isFinished = fillFloatStereoBufferFromStereo16(dataSource, dataSize, playPosition, sample, playStep, fbuffer, lengthInSamples, volume);
                break;
            }
            case AudioFormat::STEREO_32:
                isFinished = fillFloatStereoBufferFromStereo32(dataSource, dataSize, playPosition, sample, playStep, fbuffer, lengthInSamples, volume);
                break;
            case AudioFormat::STEREO_32F:
                isFinished = fillFloatStereoBufferFromStereo32Float(dataSource, dataSize, playPosition, sample, playStep, fbuffer, lengthInSamples, volume);
                break;

            default:
                state = AudioSourceState::Stopped;
                break;
            }

            if (isFinished)
            {
                if (soundStream)
                {
                    if (bufferSize == dataSize)
                    {
                        // Switch buffer
                        playPosition = 0.0f;
                        currentBuffer = lastLoadedBuffer;
                        dataSource = streamBuffers[currentBuffer];
                        dataSize = bufferAudioBytes[currentBuffer];
                        isFinished = false;
                    }
                    else
                    {
                        // Main process will restart the song
                        state = AudioSourceState::Stopped;
                        break;
                    }
                }
                else
                {
                    if (bIsLooping)
                    {
                        playPosition = 0.0f;
                    }
                    else
                    {
                        state = AudioSourceState::Stopped;
                        break;
                    }
                }
                if (bDestroyAfterPlaying)
                    destroy();
            }
        }

        // feel the rest with zeros, both channels
        lengthInSamples <<= 1;
        sample <<= 1;
        while (sample < lengthInSamples)
        {
            fbuffer[sample] = 0;
            fbuffer[sample + 1] = 0;
            sample += 2;
        }
    }
}

void AudioSource::process(float delta)
{
    if (sound && sound->isStreamable() && state == AudioSourceState::Playing)
    {
        if (currentBuffer == lastLoadedBuffer)
        {
            lastLoadedBuffer = lastLoadedBuffer == 0 ? 1 : 0;
            bufferAudioBytes[lastLoadedBuffer] = sound->processStream(soundStream, streamBuffers[lastLoadedBuffer], bufferSize);
        }
    }
    if (sound && sound->isStreamable() && soundStream && state == AudioSourceState::Stopped)
    {
        if (bIsLooping)
        {
            sound->closeStream(soundStream);
            if (sound->setupStream(soundStream))
            {
                currentBuffer = 0;
                lastLoadedBuffer = 0;
                playPosition = 0.0f;

                bufferAudioBytes[currentBuffer] = sound->processStream(soundStream, streamBuffers[currentBuffer], bufferSize);
                state = AudioSourceState::Playing;
            }
            else
            {
                delete soundStream;
                soundStream = nullptr;
            }
        }
        else
        {
            sound->closeStream(soundStream);
            delete soundStream;
            soundStream = nullptr;
        }
    }
}

void AudioSource::play(Sound *sound)
{
    this->sound = sound;
    sound->load();
    state = AudioSourceState::Stopped;

    if (sound->isReady() && !sound->isStreamable())
    {
        playPosition = 0;
        playStep = (float)sound->getSampleRate() / (float)freq;
        bIsLooping = false;
        soundStream = nullptr;
        state = AudioSourceState::Playing;
    }

    if (sound->isReady() && sound->isStreamable())
    {
        soundStream = new SoundStream();

        if (sound->setupStream(soundStream))
        {
            playPosition = 0;
            playStep = (float)sound->getSampleRate() / (float)freq;
            bIsLooping = false;

            setupBuffers(1024 * 256);

            currentBuffer = 0;
            lastLoadedBuffer = 0;

            bufferAudioBytes[currentBuffer] = sound->processStream(soundStream, streamBuffers[currentBuffer], bufferSize);
            state = AudioSourceState::Playing;
        }
    }

    bDestroyAfterPlaying = false;
}

void AudioSource::playAndDie(Sound *sound)
{
    play(sound);
    bDestroyAfterPlaying = true;
}

void AudioSource::play3d(Sound *sound, const Vector3 &vPosition)
{
    play(sound);
    this->vPosition = vPosition;
    bIs3dPositioned = true;
}

void AudioSource::play3dAndDie(Sound *sound, const Vector3 &vPosition)
{
    play(sound);
    this->vPosition = vPosition;
    bIs3dPositioned = true;
    bDestroyAfterPlaying = true;
}

void AudioSource::loop(Sound *sound)
{
    this->sound = sound;
    sound->load();

    if (sound->isReady() && !sound->isStreamable())
    {
        playPosition = 0;
        playStep = (float)sound->getSampleRate() / (float)freq;
        bIsLooping = true;
        soundStream = nullptr;
        state = AudioSourceState::Playing;
    }

    if (sound->isReady() && sound->isStreamable())
    {
        soundStream = new SoundStream();

        if (sound->setupStream(soundStream))
        {
            playPosition = 0;
            playStep = (float)sound->getSampleRate() / (float)freq;
            bIsLooping = true;

            setupBuffers(1024 * 256);

            currentBuffer = 0;
            lastLoadedBuffer = 0;

            bufferAudioBytes[currentBuffer] = sound->processStream(soundStream, streamBuffers[currentBuffer], bufferSize);
            state = AudioSourceState::Playing;
        }
    }

    bDestroyAfterPlaying = false;
}

void AudioSource::stop()
{
    state = AudioSourceState::Stopped;
    if (soundStream)
    {
        sound->closeStream(soundStream);
        delete soundStream;
        soundStream = nullptr;
    }

    bDestroyAfterPlaying = false;
}

void AudioSource::setupBuffers(int bufferSize)
{
    if (this->bufferSize != bufferSize)
    {
        if (streamBuffers[0])
            delete streamBuffers[0];
        if (streamBuffers[1])
            delete streamBuffers[1];

        this->bufferSize = bufferSize;
        streamBuffers[0] = new unsigned char[bufferSize * 2];
        streamBuffers[1] = new unsigned char[bufferSize * 2];
    }
}