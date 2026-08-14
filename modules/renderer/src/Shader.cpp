#include "GE/Shader.hpp"

#include <fstream>
#include <memory>
#include <stdexcept>

using namespace GE::Render;

Shader::Shader(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file");
    }

    this->data_ = std::make_unique<std::vector<char>>(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(this->data_->data(),
              static_cast<std::streamsize>(this->data_->size()));
    file.close();
}

Shader::Shader(const std::vector<char>& data) {
    this->data_ = std::make_unique<std::vector<char>>(data);
}

std::unique_ptr<std::vector<char>>& Shader::data() { return this->data_; }
