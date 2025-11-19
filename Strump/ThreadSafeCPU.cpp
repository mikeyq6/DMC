#include "ThreadSafeCPU.h"
#include <chrono>
#include <iostream>

void ThreadSafeCPU::GetStateSafe(uint8_t* state) {
    if (stateOperationInProgress.load()) {
        std::cout << "State operation already in progress, waiting..." << std::endl;
        // Wait for current operation to complete
        while (stateOperationInProgress.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    std::lock_guard<std::mutex> lock(stateMutex);
    stateOperationInProgress.store(true);
    
    try {
        CPU::GetState(state);
    } catch (const std::exception& e) {
        std::cout << "Exception in GetStateSafe: " << e.what() << std::endl;
    }
    
    stateOperationInProgress.store(false);
}

void ThreadSafeCPU::SetStateSafe(uint8_t* state) {
    if (stateOperationInProgress.load()) {
        std::cout << "State operation already in progress, waiting..." << std::endl;
        // Wait for current operation to complete
        while (stateOperationInProgress.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    std::lock_guard<std::mutex> lock(stateMutex);
    stateOperationInProgress.store(true);
    
    try {
        CPU::SetState(state);
    } catch (const std::exception& e) {
        std::cout << "Exception in SetStateSafe: " << e.what() << std::endl;
    }
    
    stateOperationInProgress.store(false);
}

bool ThreadSafeCPU::PauseSafe(int timeoutMs) {
    auto startTime = std::chrono::steady_clock::now();
    
    Pause();
    
    // Wait for pause to take effect with timeout
    while (!IsPaused()) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        
        if (elapsed.count() > timeoutMs) {
            std::cout << "Warning: CPU pause timeout after " << timeoutMs << "ms" << std::endl;
            return false;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return true;
}

void ThreadSafeCPU::UnpauseSafe() {
    Unpause();
}

bool ThreadSafeCPU::isStateOperationInProgress() const {
    return stateOperationInProgress.load();
}

void ThreadSafeCPU::GetState(uint8_t* state) {
    GetStateSafe(state);
}

void ThreadSafeCPU::SetState(uint8_t* state) {
    SetStateSafe(state);
}