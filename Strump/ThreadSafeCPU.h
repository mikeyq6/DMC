#pragma once
#include "CPU.h"
#include <mutex>
#include <atomic>

class ThreadSafeCPU : public CPU {
private:
    mutable std::mutex stateMutex;
    std::atomic<bool> stateOperationInProgress{false};
    
public:
    ThreadSafeCPU(bool willUseRealTimeCPU) : CPU(willUseRealTimeCPU) {}
    
    // Thread-safe state operations
    void GetStateSafe(uint8_t* state);
    void SetStateSafe(uint8_t* state);
    
    // Thread-safe pause/unpause with timeout
    bool PauseSafe(int timeoutMs = 1000);
    void UnpauseSafe();
    
    // Check if state operation is in progress
    bool isStateOperationInProgress() const;
    
protected:
    // Override original methods to add thread safety
    void GetState(uint8_t* state) override;
    void SetState(uint8_t* state) override;
};