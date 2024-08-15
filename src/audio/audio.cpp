// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "audio.h"

void Audio::process(float delta)
{
    if (bIsReady)
    {
        auto it = sources.begin();
        while (it != sources.end())
        {
            (*it)->process(delta);
            if ((*it)->isDestroyed())
            {
                delete (*it);
                it = sources.erase(it);
            }
            else
                ++it;
        }
        fillBuffers();
    }
}

AudioSource *Audio::createAudioSource()
{
    auto newSource = new AudioSource(format, freq, channels);
    sources.push_back(newSource);
    return newSource;
}

void Audio::playSound(Sound *sound, float volume)
{
    AudioSource *source = createAudioSource();
    source->setVolume(volume);
    source->playAndDie(sound);
}

void Audio::playSound3d(Sound *sound, const Vector3 &position, float volume, float maxDistance, float refDistance)
{
    AudioSource *source = createAudioSource();
    source->setVolume(volume);
    source->setReferenceDistance(refDistance);
    source->setMaxDistance(maxDistance);
    source->play3dAndDie(sound, position);
}

void Audio::syncPosition(Entity *entity)
{
    const Matrix4 &m = entity->getModelMatrix();
    vPosition = Vector3(m * Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    vDirection = glm::normalize(Matrix3(m) * Vector3(0.0f, 0.0f, 1.0f));
}
