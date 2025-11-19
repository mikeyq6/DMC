#include "ThreadSafeGameState.h"
#include "CPU.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <cstring>

ThreadSafeGameState::ThreadSafeGameState(CPU* cpuInstance, const std::string& fileName) 
    : cpu(cpuInstance), saveFileName(fileName) {
    if (!cpu) {
        throw std::invalid_argument("CPU instance cannot be null");
    }
}

bool ThreadSafeGameState::waitForCPUPause(int timeoutMs) {
    auto startTime = std::chrono::steady_clock::now();
    
    cpu->Pause();
    
    // Wait for pause to take effect with timeout
    while (!cpu->IsPaused()) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        
        if (elapsed.count() > timeoutMs) {
            logOperation("CPU Pause", false, "Timeout after " + std::to_string(timeoutMs) + "ms");
            return false;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return true;
}

void ThreadSafeGameState::logOperation(const std::string& operation, bool success, const std::string& details) {
    std::cout << "[ThreadSafeGameState] " << operation << ": " 
              << (success ? "SUCCESS" : "FAILED");
    if (!details.empty()) {
        std::cout << " - " << details;
    }
    std::cout << std::endl;
}

bool ThreadSafeGameState::saveGameState() {
    // Check if another operation is in progress
    if (isAnyOperationInProgress()) {
        logOperation("Save", false, "Another save/load operation is already in progress");
        return false;
    }
    
    // Acquire exclusive lock
    std::lock_guard<std::mutex> lock(saveLoadMutex);
    saveInProgress.store(true);
    
    bool success = false;
    uint8_t* gameState = nullptr;
    
    try {
        // Pause CPU with timeout
        if (!waitForCPUPause()) {
            logOperation("Save", false, "Failed to pause CPU");
            saveInProgress.store(false);
            return false;
        }
        
        // Calculate required memory size
        uint32_t size = cpu->GetMemory()->GetMemorySize() + TIMER_STATE_SIZE + REGISTERS_STATE_SIZE + RAM_STATE_SIZE;
        
        // Allocate memory for state
        gameState = static_cast<uint8_t*>(std::malloc(size));
        if (!gameState) {
            logOperation("Save", false, "Failed to allocate " + std::to_string(size) + " bytes");
            cpu->Unpause();
            saveInProgress.store(false);
            return false;
        }
        
        // Initialize memory to zero
        std::memset(gameState, 0, size);
        
        // Get current CPU state
        cpu->GetState(gameState);
        
        // Write to file using C++ streams for better error handling
        std::ofstream saveFile(saveFileName, std::ios::binary);
        if (!saveFile.is_open()) {
            logOperation("Save", false, "Cannot open file: " + saveFileName);
        } else {
            saveFile.write(reinterpret_cast<const char*>(gameState), size);
            if (saveFile.good()) {
                success = true;
                logOperation("Save", true, "Saved " + std::to_string(size) + " bytes to " + saveFileName);
            } else {
                logOperation("Save", false, "Write error to file: " + saveFileName);
            }
            saveFile.close();
        }
        
    } catch (const std::exception& e) {
        logOperation("Save", false, "Exception: " + std::string(e.what()));
    } catch (...) {
        logOperation("Save", false, "Unknown exception occurred");
    }
    
    // Cleanup
    if (gameState) {
        std::free(gameState);
    }
    
    cpu->Unpause();
    saveInProgress.store(false);
    
    return success;
}

bool ThreadSafeGameState::loadGameState() {
    // Check if another operation is in progress
    if (isAnyOperationInProgress()) {
        logOperation("Load", false, "Another save/load operation is already in progress");
        return false;
    }
    
    // Acquire exclusive lock
    std::lock_guard<std::mutex> lock(saveLoadMutex);
    loadInProgress.store(true);
    
    bool success = false;
    uint8_t* gameState = nullptr;
    
    try {
        // Check if save file exists
        std::ifstream checkFile(saveFileName, std::ios::binary);
        if (!checkFile.is_open()) {
            logOperation("Load", false, "Save file does not exist: " + saveFileName);
            loadInProgress.store(false);
            return false;
        }
        checkFile.close();
        
        // Pause CPU with timeout
        if (!waitForCPUPause()) {
            logOperation("Load", false, "Failed to pause CPU");
            loadInProgress.store(false);
            return false;
        }
        
        // Calculate required memory size
        uint32_t size = cpu->GetMemory()->GetMemorySize() + PALETTE_SIZE + TIMER_STATE_SIZE + REGISTERS_STATE_SIZE + RAM_STATE_SIZE;
        
        // Allocate memory for state
        gameState = static_cast<uint8_t*>(std::malloc(size));
        if (!gameState) {
            logOperation("Load", false, "Failed to allocate " + std::to_string(size) + " bytes");
            cpu->Unpause();
            loadInProgress.store(false);
            return false;
        }
        
        // Initialize memory to zero
        std::memset(gameState, 0, size);
        
        // Read from file using C++ streams
        std::ifstream loadFile(saveFileName, std::ios::binary);
        if (!loadFile.is_open()) {
            logOperation("Load", false, "Cannot open file: " + saveFileName);
        } else {
            loadFile.read(reinterpret_cast<char*>(gameState), size);
            std::streamsize bytesRead = loadFile.gcount();
            
            if (loadFile.good() || loadFile.eof()) {
                // Set the loaded state
                cpu->SetState(gameState);
                success = true;
                logOperation("Load", true, "Loaded " + std::to_string(bytesRead) + " bytes from " + saveFileName);
            } else {
                logOperation("Load", false, "Read error from file: " + saveFileName);
            }
            loadFile.close();
        }
        
    } catch (const std::exception& e) {
        logOperation("Load", false, "Exception: " + std::string(e.what()));
    } catch (...) {
        logOperation("Load", false, "Unknown exception occurred");
    }
    
    // Cleanup
    if (gameState) {
        std::free(gameState);
    }
    
    cpu->Unpause();
    loadInProgress.store(false);
    
    return success;
}