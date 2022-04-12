#ifndef FLINT_IO_H
#define FLINT_IO_H

#include <vector>
#include <string>
#include <istream>
#include <fstream>
#include <iostream>

inline std::vector<char> readFile(const std::string &filename) {
    std::ifstream input(filename, std::ios::binary);

    std::vector<char> bytes((std::istreambuf_iterator<char>(input)),
                            (std::istreambuf_iterator<char>()));

    input.close();

    return bytes;
}

inline void split_filename(const std::string &str, std::string &file_directory) {
    size_t found = str.find_last_of("/\\");

//    std::cout << "Splitting: " << str << std::endl;
//    std::cout << "Folder: " << str.substr(0, found) << std::endl;
//    std::cout << "File: " << str.substr(found + 1) << std::endl;

    file_directory = str.substr(0, found);
}

#endif //FLINT_IO_H
