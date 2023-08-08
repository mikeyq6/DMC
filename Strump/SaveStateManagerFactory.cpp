#include "SaveStateManagerFactory.h"

SaveStateManagerFactory::SaveStateManagerFactory() { }

SaveStateManager* SaveStateManagerFactory::GetStateManager(int8_t version) {
    switch(version) {
        case LATEST:
            return new SaveStateManager_v1();
        case 1:
            return new SaveStateManager_v1();
        default:
            return new SaveStateManager();
    }
}