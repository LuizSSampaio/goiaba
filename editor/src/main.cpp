#include <vulkan/vulkan.h>

#include <GE/Logger.hpp>
#include <GE/Renderer.hpp>
#include <QApplication>
#include <QByteArray>
#include <QByteArrayList>
#include <QTimer>
#include <QVersionNumber>
#include <QVulkanInstance>

#include "Widgets/Viewport/Viewport.hpp"

int main(int argc, char* argv[]) {
    GE::Logger::Init();

    QApplication app(argc, argv);

    QVulkanInstance qInstance;
    qInstance.setApiVersion(QVersionNumber(1, 3));
    qInstance.setExtensions(QByteArrayList{VK_EXT_DEBUG_UTILS_EXTENSION_NAME});
    if (!qInstance.create()) {
        GE::Logger::Shutdown();
        return 1;
    }

    GE::Render::Renderer renderer;

    GEditor::Widgets::Viewport viewport(&qInstance, &renderer);
    viewport.resize(800, 600);
    viewport.show();

    QTimer renderTimer;
    QObject::connect(&renderTimer, &QTimer::timeout,
                     [&renderer]() { renderer.Run(); });
    constexpr int frameMs = 16;
    renderTimer.start(frameMs);

    int ret = QApplication::exec();
    GE::Logger::Shutdown();
    return ret;
}
