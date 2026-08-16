#pragma once

#include <memory>
#include <string>
#include <vector>

namespace GE::Render {
class Shader {
public:
    Shader() = delete;
    // TODO: Remove filepath constructor - Must be made at Asset Manager
    Shader(const std::string& filePath);
    Shader(const std::vector<char>& data);
    ~Shader() = default;

    [[nodiscard]] std::unique_ptr<std::vector<char>>& data();

private:
    std::unique_ptr<std::vector<char>> data_;
};
}  // namespace GE::Render
