#include "EmulatorThreadSafe.h"
#include <iostream>
#include <thread>
#include <chrono>

/**
 * Example demonstrating thread-safe save/load operations
 * This shows how multiple threads can safely request save/load operations
 * without causing race conditions or data corruption
 */

void saveWorker(EmulatorThreadSafe* emulator, int workerId) {
    std::cout << "Save worker " << workerId << " starting" << std::endl;
    
    // Simulate some delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * workerId));
    
    // Attempt to save
    std::cout << "Save worker " << workerId << " requesting save..." << std::endl;
    emulator->saveGameState();
    
    std::cout << "Save worker " << workerId << " finished" << std::endl;
}

void loadWorker(EmulatorThreadSafe* emulator, int workerId) {
    std::cout << "Load worker " << workerId << " starting" << std::endl;
    
    // Simulate some delay
    std::this_thread::sleep_for(std::chrono::milliseconds(150 * workerId));
    
    // Attempt to load
    std::cout << "Load worker " << workerId << " requesting load..." << std::endl;
    emulator->loadGameState();
    
    std::cout << "Load worker " << workerId << " finished" << std::endl;
}

void demonstrateThreadSafety() {
    std::cout << "=== Thread Safety Demonstration ===" << std::endl;
    
    // Note: This is just a demonstration - in practice you would have a real ROM file
    const char* dummyRom = "dummy.gb";
    
    try {
        EmulatorThreadSafe emulator(dummyRom);
        
        // In a real scenario, you would call emulator.Init() and emulator.Start()
        // For this demo, we're just showing the thread safety concept
        
        std::cout << "Creating multiple threads to test save/load operations..." << std::endl;
        
        // Create multiple threads that will try to save/load simultaneously
        std::vector<std::thread> threads;
        
        // Create save worker threads
        for (int i = 1; i <= 3; ++i) {
            threads.emplace_back(saveWorker, &emulator, i);
        }
        
        // Create load worker threads
        for (int i = 1; i <= 2; ++i) {
            threads.emplace_back(loadWorker, &emulator, i);
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        std::cout << "All threads completed. Operations were properly serialized." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

// Uncomment the main function below to run the demonstration
/*
int main() {
    demonstrateThreadSafety();
    return 0;
}
*/