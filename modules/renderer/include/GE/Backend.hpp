#pragma once

namespace GE::Render {
class IBackend {
public:
    virtual ~IBackend() = default;

    virtual void RenderPass() = 0;
    virtual void Resize() = 0;
};
}  // namespace GE::Render
