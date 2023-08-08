#pragma once

#define LATEST -1

#include <cstdint>

#include "SaveStateManager.h"
#include "SaveStateManager_v1.h"

class SaveStateManagerFactory {
    public:
        SaveStateManagerFactory();
        SaveStateManager* GetStateManager(int8_t version);
};