#pragma once
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <chrono>

// Forward declarations
class CPU;

/**
 * Thread-safe wrapper for game state save/load operations
 * Provides proper locking around critical sections to prevent race conditions
 */
class ThreadSafeGameState {
private:
    mutable std::mutex saveLoadMutex;
    std::atomic<bool> saveInProgress{false};
    std::atomic<bool> loadInProgress{false};
    
    CPU* cpu;
    std::string saveFileName;
    
    // Constants for state sizes
    static constexpr uint32_t TIMER_STATE_SIZE = 12;
    static constexpr uint32_t REGISTERS_STATE_SIZE = 18;
    static constexpr uint32_t RAM_STATE_SIZE = 100;
    static constexpr uint32_t PALETTE_SIZE = 128; // Approximate size
    
    // Helper methods
    bool waitForCPUPause(int timeoutMs = 1000);
    void logOperation(const std::string& operation, bool success, const std::string& details = "");
    
public:
    ThreadSafeGameState(CPU* cpuInstance, const std::string& fileName);
    ~ThreadSafeGameState() = default;
    
    // Main thread-safe operations
    bool saveGameState();
    bool loadGameState();
    
    // Status checking
    bool isSaveInProgress() const { return saveInProgress.load(); }
    bool isLoadInProgress() const { return loadInProgress.load(); }
    bool isAnyOperationInProgress() const { return saveInProgress.load() || loadInProgress.load(); }
    
    // Configuration
    void setSaveFileName(const std::string& fileName) { saveFileName = fileName; }
    std::string getSaveFileName() const { return saveFileName; }
};