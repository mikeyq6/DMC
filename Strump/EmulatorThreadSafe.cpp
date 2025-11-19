#include "EmulatorThreadSafe.h"
#include <iostream>

EmulatorThreadSafe::EmulatorThreadSafe(const char* cartridgeFileName) 
    : Emulator(cartridgeFileName) {
    // gameStateManager will be initialized in Init() after CPU is created
}

bool EmulatorThreadSafe::Init() {
    // Call parent initialization first
    if (!Emulator::Init()) {
        return false;
    }
    
    // Now create the thread-safe game state manager
    try {
        gameStateManager = std::make_unique<ThreadSafeGameState>(cpu, saveFileName);
        std::cout << "Thread-safe game state manager initialized" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "Failed to initialize thread-safe game state manager: " << e.what() << std::endl;
        return false;
    }
}

void EmulatorThreadSafe::saveGameState() {
    if (!gameStateManager) {
        std::cout << "Game state manager not initialized" << std::endl;
        return;
    }
    
    if (gameStateManager->saveGameState()) {
        std::cout << "Game state saved successfully" << std::endl;
    } else {
        std::cout << "Failed to save game state" << std::endl;
    }
}

void EmulatorThreadSafe::loadGameState() {
    if (!gameStateManager) {
        std::cout << "Game state manager not initialized" << std::endl;
        return;
    }
    
    if (gameStateManager->loadGameState()) {
        std::cout << "Game state loaded successfully" << std::endl;
    } else {
        std::cout << "Failed to load game state" << std::endl;
    }
}

bool EmulatorThreadSafe::isSaveLoadInProgress() const {
    if (!gameStateManager) {
        return false;
    }
    return gameStateManager->isAnyOperationInProgress();
}