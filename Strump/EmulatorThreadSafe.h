#pragma once
#include "Emulator.h"
#include "ThreadSafeGameState.h"
#include <memory>

/**
 * Thread-safe version of the Emulator class
 * Uses ThreadSafeGameState for proper locking around save/load operations
 */
class EmulatorThreadSafe : public Emulator {
private:
    std::unique_ptr<ThreadSafeGameState> gameStateManager;
    
public:
    EmulatorThreadSafe(const char* cartridgeFileName);
    virtual ~EmulatorThreadSafe() = default;
    
    // Override to use thread-safe implementations
    void saveGameState() override;
    void loadGameState() override;
    
    // Additional thread-safe methods
    bool isSaveLoadInProgress() const;
    
    // Initialize after CPU is created
    bool Init() override;
};