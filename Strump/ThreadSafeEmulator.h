#pragma once
#include "Emulator.h"
#include <mutex>
#include <atomic>

class ThreadSafeEmulator : public Emulator {
private:
    mutable std::mutex saveLoadMutex;
    std::atomic<bool> saveInProgress{false};
    std::atomic<bool> loadInProgress{false};
    
public:
    ThreadSafeEmulator(const char* cartridgeFileName) : Emulator(cartridgeFileName) {}
    
    // Thread-safe save operation
    void saveGameStateSafe();
    
    // Thread-safe load operation  
    void loadGameStateSafe();
    
    // Check if save/load operations are in progress
    bool isSaveLoadInProgress() const;
    
protected:
    // Override the original save/load methods to make them thread-safe
    void saveGameState() override;
    void loadGameState() override;
};