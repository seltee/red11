// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "audioWindows.h"
#include <windows.h>
#include <dsound.h>
#include <stdio.h>
#include <iostream>
#include <string>

#ifdef WINDOWS_ONLY

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

#define FREQ (88200)
#define FREQ_PART (FREQ / 20)
#define CHANNELS_AMOUNT 2
#define BYTES_PER_SAMPLE 4
#define BUFFER_SIZE (FREQ_PART * CHANNELS_AMOUNT * BYTES_PER_SAMPLE)

const Vector3 vUpDirection = Vector3(0.0f, 1.0f, 0.0f);

// Callback function to handle each enumerated device
BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCWSTR lpcstrDescription, LPCWSTR lpcstrModule, LPVOID lpContext)
{
    std::vector<AudioDeviceWindows> *list = (std::vector<AudioDeviceWindows> *)lpContext;
    list->push_back({convertToString(lpcstrDescription), lpGuid});
    return true;
}

AudioWindows::AudioWindows()
{
}

std::vector<AudioDeviceWindows> AudioWindows::getWindowsDevicesList()
{
    std::vector<AudioDeviceWindows> list;
    DirectSoundEnumerateW(DSEnumCallback, &list);
    return list;
}

std::vector<AudioDevice> AudioWindows::getDevicesList()
{
    std::vector<AudioDevice> list;
    std::vector<AudioDeviceWindows> intenralList = getWindowsDevicesList();
    for (auto &item : intenralList)
    {
        list.push_back({item.description, item.lpGuid ? false : true});
    }
    return list;
}

bool AudioWindows::setup()
{
    return setup("");
}

bool AudioWindows::setup(std::string deviceName)
{
    if (bIsReady)
        return true;

    LPCGUID lpGuid = nullptr;
    if (deviceName.length() > 0)
    {
        auto list = getWindowsDevicesList();
        for (auto &item : list)
        {
            if (item.description == deviceName)
            {
                lpGuid = item.lpGuid;
                break;
            }
        }
    }

    HRESULT result = DirectSoundCreate8(lpGuid, &lpds, NULL);
    if (result == DS_OK)
    {
        HWND hWnd = GetConsoleWindow();
        lpds->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);

        DSBUFFERDESC dsbd;
        ZeroMemory(&dsbd, sizeof(dsbd));
        dsbd.dwSize = sizeof(dsbd);
        dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
        LPDIRECTSOUNDBUFFER pPrimaryBuffer;
        lpds->CreateSoundBuffer(&dsbd, &pPrimaryBuffer, NULL);

        bIsReady = setupPinPongBuffers();
        return bIsReady;
    }
    else
    {
        printf("%s\n", getAudioError("Audio setup failed", result).c_str());
        return false;
    }
}

void AudioWindows::fillBuffers()
{
    unsigned long playPos;
    pDXBuffer->GetCurrentPosition(&playPos, NULL);

    if (bFirstPartPlaying && playPos > BUFFER_SIZE)
    {
        bFirstPartPlaying = false;
        fillBuffer(0);
    }

    if (!bFirstPartPlaying && playPos < BUFFER_SIZE)
    {
        bFirstPartPlaying = true;
        fillBuffer(BUFFER_SIZE);
    }
}

bool AudioWindows::setupPinPongBuffers()
{
    // Initialize WAVEFORMATEX structure
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nChannels = CHANNELS_AMOUNT;
    wfx.nSamplesPerSec = FREQ;
    wfx.wBitsPerSample = BYTES_PER_SAMPLE * 8;
    wfx.nBlockAlign = BYTES_PER_SAMPLE * CHANNELS_AMOUNT;
    wfx.nAvgBytesPerSec = FREQ * BYTES_PER_SAMPLE * CHANNELS_AMOUNT;
    wfx.cbSize = 0;

    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = BUFFER_SIZE * 2;
    dsbd.lpwfxFormat = &wfx;

    bUsePcm = false;
    HRESULT result = lpds->CreateSoundBuffer(&dsbd, &pDXBuffer, NULL);
    if (result != DS_OK)
    {
        bUsePcm = true;
        wfx.wFormatTag = WAVE_FORMAT_PCM;

        HRESULT result = lpds->CreateSoundBuffer(&dsbd, &pDXBuffer, NULL);
        if (result != DS_OK)
        {
            printf("%s\n", getAudioError("Audio buffer setup failed", result).c_str());
            return false;
        }
    }

    pDataBuffer = new unsigned char[BUFFER_SIZE];
    memset(pDataBuffer, 0, BUFFER_SIZE);
    pShiftBuffer = new unsigned char[BUFFER_SIZE];
    memset(pShiftBuffer, 0, BUFFER_SIZE);

    freq = FREQ;
    channels = CHANNELS_AMOUNT;
    bufferSize = BUFFER_SIZE;
    format = AudioFormat::STEREO_32F;

    result = pDXBuffer->Play(0, 0, DSBPLAY_LOOPING);
    if (result != DS_OK)
    {
        printf("%s\n", getAudioError("Failed to loop buffer", result).c_str());
        return false;
    }

    return true;
}

void AudioWindows::fillBuffer(unsigned int bufferPosition)
{
    float *pfDataBuffer = reinterpret_cast<float *>(pDataBuffer);
    int *pnDataBuffer = reinterpret_cast<int *>(pDataBuffer);
    float *pfShiftBuffer = reinterpret_cast<float *>(pShiftBuffer);

    memset(pfDataBuffer, 0, BUFFER_SIZE);

    for (auto &it : sources)
    {
        if (it->getState() == AudioSourceState::Playing)
        {
            if (it->is3dPositioned())
            {
                // Check if source is in sound range
                Vector3 localPosition = it->getPosition() - vPosition;
                float maxDistance = it->getMaxDistance();
                float distance = glm::length(localPosition);

                if (distance > maxDistance)
                    continue;

                // Calc left/right relations
                Vector3 vSide = glm::cross(vDirection, vUpDirection);

                float sideProjection = glm::dot(vSide, glm::normalize(localPosition));
                float mainValue = 1.0f - fabsf(sideProjection);
                float lVolume, rVolume;
                if (sideProjection < 0.0f)
                {
                    lVolume = mainValue;
                    rVolume = mainValue + fabsf(sideProjection);
                }
                else
                {
                    lVolume = mainValue + fabsf(sideProjection);
                    rVolume = mainValue;
                }

                // Volume based on distance
                float distanceVolume = 1.0f;
                float refDistance = it->getReferenceDistance();

                if (distance > refDistance)
                {
                    distanceVolume = 1.0f - (distance - refDistance) / (maxDistance - refDistance);
                }
                lVolume *= distanceVolume;
                rVolume *= distanceVolume;

                it->fillBuffer(pShiftBuffer, FREQ_PART);
                for (int i = 0; i < BUFFER_SIZE / 4; i += 2)
                {
                    pfDataBuffer[i] += pfShiftBuffer[i] * lVolume;
                    pfDataBuffer[i + 1] += pfShiftBuffer[i + 1] * rVolume;
                }
            }
            else
            {
                it->fillBuffer(pShiftBuffer, FREQ_PART);
                for (int i = 0; i < BUFFER_SIZE / 4; i += 2)
                {
                    pfDataBuffer[i] += pfShiftBuffer[i];
                    pfDataBuffer[i + 1] += pfShiftBuffer[i + 1];
                }
            }
        }
    }

    // Limiter
    for (int i = 0; i < BUFFER_SIZE / 4; i++)
        pfDataBuffer[i] = fminf(fmaxf(pfDataBuffer[i], -1.0f), 1.0f);

    // Compatability of floats are not supported
    if (bUsePcm)
    {
        for (int i = 0; i < BUFFER_SIZE / 4; i++)
            pnDataBuffer[i] = static_cast<unsigned int>((pfDataBuffer[i] + 1.0f) * 1073741823.5);
    }

    void *pLockedBuffer = NULL;
    unsigned long lockedBufferSize = 0;
    HRESULT hr = pDXBuffer->Lock(bufferPosition, BUFFER_SIZE, &pLockedBuffer, &lockedBufferSize, NULL, NULL, 0);
    if (SUCCEEDED(hr))
    {
        memcpy(pLockedBuffer, pDataBuffer, lockedBufferSize);
        pDXBuffer->Unlock(pLockedBuffer, lockedBufferSize, NULL, 0);
    }
}

#endif