//
// Created by tannh on 2/13/2022.
//

#ifndef VULKAN_DEMO_APP_IO_H
#define VULKAN_DEMO_APP_IO_H

#include <vector>
#include <string>
#include <istream>
#include <fstream>

static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file!");
    }

    // The advantage of starting to read at the end of the file is that
    // we can use the read position to determine the size of the file and allocate a buffer
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

#endif //VULKAN_DEMO_APP_IO_H
