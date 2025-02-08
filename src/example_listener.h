// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.

#pragma once

#include <endstone/event/server/server_load_event.h>
#include <endstone/plugin/plugin.h>

class ExampleListener {
public:
    explicit ExampleListener() = default;

    void onServerLoad(endstone::ServerLoadEvent& event);
};