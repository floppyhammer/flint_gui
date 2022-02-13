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
    std::ifstream input(filename, std::ios::binary);

    std::vector<char> bytes((std::istreambuf_iterator<char>(input)),
                            (std::istreambuf_iterator<char>()));

    input.close();

    return bytes;
}

#endif //VULKAN_DEMO_APP_IO_H
