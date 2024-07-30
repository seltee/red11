// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once

class Usable
{
public:
    inline void addUser() { users++; }
    inline void removeUser() { users--; };
    inline unsigned int getAmountOfUsers() { return users; }
    virtual bool isLoaded() = 0;
    virtual void load() = 0;
    virtual void unload() = 0;

protected:
    unsigned int users = 0;
};