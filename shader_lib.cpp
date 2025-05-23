
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

#pragma pack(push, 1) // Ensure no padding

struct BMPFileHeader {
    uint16_t bfType { 0x4D42 }; // 'BM'
    uint32_t bfSize; // Size of the file in bytes
    uint16_t bfReserved1 { 0 };
    uint16_t bfReserved2 { 0 };
    uint32_t bfOffBits { 54 }; // Pixel data offset (14 + 40)
};

struct BMPInfoHeader {
    uint32_t biSize { 40 }; // Size of this header (40 bytes)
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes { 1 };
    uint16_t biBitCount { 24 }; // 24-bit
    uint32_t biCompression { 0 }; // BI_RGB (no compression)
    uint32_t biSizeImage { 0 }; // Can be 0 for BI_RGB
    int32_t biXPelsPerMeter { 2835 }; // 72 DPI
    int32_t biYPelsPerMeter { 2835 };
    uint32_t biClrUsed { 0 };
    uint32_t biClrImportant { 0 };
};

#pragma pack(pop)

void WriteBMP(const char* filename, int width, int height, const uint8_t* pixelData)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
        return;

    int rowPadding = (4 - (width * 3) % 4) % 4;
    int paddedRowSize = width * 3 + rowPadding;
    int dataSize = paddedRowSize * height;

    BMPFileHeader fileHeader;
    fileHeader.bfSize = 54 + dataSize;

    BMPInfoHeader infoHeader;
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biSizeImage = dataSize;

    file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    uint8_t padding[3] = { 0, 0, 0 };

    // BMP stores pixels from bottom to top
    for (int y = height - 1; y >= 0; --y) {
        const uint8_t* row = &pixelData[y * width * 3];
        file.write(reinterpret_cast<const char*>(row), width * 3);
        file.write(reinterpret_cast<const char*>(padding), rowPadding);
    }

    file.close();
}

void makeSwizzlers(uint thisVecSize, uint outVecSize)
{
    assert(thisVecSize <= 4 && outVecSize <= 4);
    const char lut_xyzw[] = "xyzw";
    const char lut_rgba[] = "rgba";
    const char lut_stpq[] = "stpq";

    std::stringstream ss;

    const uint iterations[4] { thisVecSize,
        outVecSize >= 2 ? thisVecSize : 1,
        outVecSize >= 3 ? thisVecSize : 1,
        outVecSize >= 4 ? thisVecSize : 1 };

    ss << "iterations: " << iterations[0] << ", " << iterations[1]
       << ", " << iterations[2] << ", " << iterations[3] << '\n';

    for (int a = 0; a < iterations[0]; ++a) {
        for (int b = 0; b < iterations[1]; ++b) {
            for (int c = 0; c < iterations[2]; ++c) {
                for (int d = 0; d < iterations[3]; ++d) {
                    int buf[4] { a, b, c, d };

                    ss << "Swiz" << outVecSize << "<" << thisVecSize;
                    for (int i = 0; i < outVecSize; ++i)
                        ss << ", " << buf[i];
                    ss << "> ";
                    for (int i = 0; i < outVecSize; ++i)
                        ss << lut_xyzw[buf[i]];
                    ss << ", ";
                    for (int i = 0; i < outVecSize; ++i)
                        ss << lut_rgba[buf[i]];
                    ss << ", ";
                    for (int i = 0; i < outVecSize; ++i)
                        ss << lut_stpq[buf[i]];
                    ss << ";\n";
                }
            }
        }
    }

    std::cout << ss.str() << std::endl;

    exit(0);
}
