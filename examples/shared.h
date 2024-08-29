// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "red11.h"

struct GetDataResponseData
{
    unsigned int index;
    float pos[3];
    float rotation[4];
};

struct SetPositionRequestData
{
    float pos[3];
    float rotation[4];
};

struct SetUserPositionData
{
    unsigned int index;
    float pos[3];
    float rotation[4];
};

struct InputControl
{
    float move;
    float sideMove;
    float rotateX;
    float rotateY;
};

class ApiFunctions
{
public:
    ApiFunctions(NetworkApi &api)
    {
        getDataRequest = api.addFixedSizeApiCall(0);
        getDataResponse = api.addFixedSizeApiCall(sizeof(GetDataResponseData));
        setPositionRequest = api.addFixedSizeApiCall(sizeof(SetPositionRequestData));
        setUserPosition = api.addVolatileSizeApiCall();
    }

    NetworkApiCall getDataRequest;
    NetworkApiCall getDataResponse;
    NetworkApiCall setPositionRequest;
    NetworkApiCall setUserPosition;
};