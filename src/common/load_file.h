#ifndef FLINT_LOAD_FILE_H
#define FLINT_LOAD_FILE_H

#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

inline std::string load_file_as_string(const char *file_path) {
    std::string output;
    std::ifstream file;

    // Ensure ifstream object can throw exceptions.
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Open file.
        file.open(file_path);

        // Read file's buffer contents into stream.
        std::stringstream stream;
        stream << file.rdbuf();

        // Close file handler.
        file.close();

        // Convert stream into string.
        output = stream.str();
    } catch (std::ifstream::failure &e) {
        throw std::runtime_error(std::string("Failed to load string from disk: ") + std::string(file_path));
    }

    return output;
}

inline std::vector<char> load_file_as_bytes(const char *file_path) {
    std::ifstream input(file_path, std::ios::binary);

    std::vector<char> bytes((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));

    input.close();

    return bytes;
}

inline void split_path(const std::string &str, std::string &file_directory) {
    size_t found = str.find_last_of("/\\");

    //    std::cout << "Splitting: " << str << std::endl;
    //    std::cout << "Folder: " << str.substr(0, found) << std::endl;
    //    std::cout << "File: " << str.substr(found + 1) << std::endl;

    file_directory = str.substr(0, found);
}

#endif // FLINT_LOAD_FILE_H
