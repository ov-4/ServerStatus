#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <iostream>

// T must has YAML::convert<T>
template <typename T>
class ConfigLoader {
public:
    static ConfigLoader& Instance() {
        static ConfigLoader instance;
        return instance;
    }

    // get config snapshot
    // return copy of shared_ptr, increase reference count
    // even if Load() is triggered in another thread and replaces current_config_,
    // the caller's shared_ptr still points to the old valid memory until the caller releases it
    std::shared_ptr<T> Get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        // if current_config_ is null, return a new shared_ptr to a default-constructed T
        if (!current_config_) {
            return std::make_shared<T>();
        }
        return current_config_;
    }

    // load from file
    bool Load(const std::string& path) {
        try {
            // resolve config on stack
            YAML::Node node = YAML::LoadFile(path);
            
            // call YAML::convert<T>::decode
            T config_obj = node.as<T>(); 
            
            // construct new managed object
            auto new_config_ptr = std::make_shared<T>(std::move(config_obj));

            // atom/lock
            {
                std::lock_guard<std::mutex> lock(mutex_);

                // now, current_config_ points to new memory
                // if old memory is still in use by other threads (holding shared_ptr), keep
                // otherwise, the reference count will drop to zero, and it will be destructed
                current_config_ = new_config_ptr;
            }
            return true;
        } catch (const YAML::Exception& e) {
            std::cerr << "[ConfigLoader] YAML Error: " << e.what() << std::endl;
            return false;
        } catch (const std::exception& e) {
            std::cerr << "[ConfigLoader] Error: " << e.what() << std::endl;
            return false;
        }
    }

    // write to file
    bool Save(const std::string& path, const T& config) {
        try {
            YAML::Node node;
            // call YAML::convert<T>::encode
            node = config; 
            
            std::ofstream fout(path);
            fout << node;
            return fout.good();
        } catch (const std::exception& e) {
            std::cerr << "[ConfigLoader] Save Error: " << e.what() << std::endl;
            return false;
        }
    }

private:
    ConfigLoader() = default;
    ~ConfigLoader() = default;
    ConfigLoader(const ConfigLoader&) = delete;
    ConfigLoader& operator=(const ConfigLoader&) = delete;

    mutable std::mutex mutex_;
    std::shared_ptr<T> current_config_;
};