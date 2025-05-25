#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

namespace Utils {
void WriteBMP(const char* filename, int width, int height, const uint8_t* pixelData);
void makeSwizzlers(uint32_t thisVecSize, uint32_t outVecSize);
}
#endif // UTILS_H
