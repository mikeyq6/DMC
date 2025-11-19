#include "ThreadSafeEmulator.h"
#include <iostream>

void ThreadSafeEmulator::saveGameStateSafe() {
    // Check if another save/load operation is in progress
    if (saveInProgress.load() || loadInProgress.load()) {
        std::cout << "Save/Load operation already in progress, skipping save request" << std::endl;
        return;
    }
    
    // Acquire lock and set atomic flag
    std::lock_guard<std::mutex> lock(saveLoadMutex);
    saveInProgress.store(true);
    
    try {
        // Pause CPU to ensure consistent state
        cpu->Pause();
        
        uint32_t size = cpu->GetMemory()->GetMemorySize() + TIMER_STATE_SIZE + REGISTERS_STATE_SIZE + RAM_STATE_SIZE;
        uint8_t* gameState = (uint8_t*)malloc(sizeof(uint8_t) * size);
        
        if (!gameState) {
            std::cout << "Failed to allocate memory for save state" << std::endl;
            cpu->Unpause();
            saveInProgress.store(false);
            return;
        }
        
        // Initialize memory
        memset(gameState, 0, size);
        
        // Get current state
        cpu->GetState(gameState);
        
        // Write to file
        std::cout << "Saving to: " << saveFileName << std::endl;
        FILE* sfp = fopen(saveFileName.c_str(), "wb");
        if (sfp == NULL) {
            std::cout << "Couldn't open save file: " << saveFileName << std::endl;
        } else {
            size_t written = fwrite(gameState, sizeof(uint8_t), size, sfp);
            if (written != size) {
                std::cout << "Warning: Only wrote " << written << " of " << size << " bytes" << std::endl;
            }
            fclose(sfp);
            std::cout << "Save completed successfully" << std::endl;
        }
        
        free(gameState);
        cpu->Unpause();
        
    } catch (const std::exception& e) {
        std::cout << "Exception during save: " << e.what() << std::endl;
        cpu->Unpause();
    }
    
    saveInProgress.store(false);
}

void ThreadSafeEmulator::loadGameStateSafe() {
    // Check if another save/load operation is in progress
    if (saveInProgress.load() || loadInProgress.load()) {
        std::cout << "Save/Load operation already in progress, skipping load request" << std::endl;
        return;
    }
    
    // Acquire lock and set atomic flag
    std::lock_guard<std::mutex> lock(saveLoadMutex);
    loadInProgress.store(true);
    
    try {
        // Pause CPU to ensure consistent state
        cpu->Pause();
        
        uint32_t size = cpu->GetMemory()->GetMemorySize() + PALETTE_SIZE + TIMER_STATE_SIZE + REGISTERS_STATE_SIZE + RAM_STATE_SIZE;
        uint8_t* gameState = (uint8_t*)malloc(sizeof(uint8_t) * size);
        
        if (!gameState) {
            std::cout << "Failed to allocate memory for load state" << std::endl;
            cpu->Unpause();
            loadInProgress.store(false);
            return;
        }
        
        // Initialize memory
        memset(gameState, 0, size);
        
        // Read from file
        std::cout << "Loading from: " << saveFileName << std::endl;
        FILE* sfp = fopen(saveFileName.c_str(), "rb");
        if (sfp == NULL) {
            std::cout << "Couldn't open save file: " << saveFileName << std::endl;
            free(gameState);
            cpu->Unpause();
            loadInProgress.store(false);
            return;
        }
        
        size_t read = fread(gameState, sizeof(uint8_t), size, sfp);
        if (read != size) {
            std::cout << "Warning: Only read " << read << " of " << size << " bytes" << std::endl;
        }
        fclose(sfp);
        
        // Set the loaded state
        cpu->SetState(gameState);
        
        free(gameState);
        cpu->Unpause();
        
        std::cout << "Load completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Exception during load: " << e.what() << std::endl;
        cpu->Unpause();
    }
    
    loadInProgress.store(false);
}

bool ThreadSafeEmulator::isSaveLoadInProgress() const {
    return saveInProgress.load() || loadInProgress.load();
}

void ThreadSafeEmulator::saveGameState() {
    saveGameStateSafe();
}

void ThreadSafeEmulator::loadGameState() {
    loadGameStateSafe();
}