#ifndef FLINT_IO_H
#define FLINT_IO_H

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

#endif //FLINT_IO_H
