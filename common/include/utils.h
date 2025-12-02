#pragma once
#include <string>

class Utils {
public:
    // UUID v4
    static std::string GenerateUUID();
    
    static std::string GenerateRandomPassword(int length = 32);
};