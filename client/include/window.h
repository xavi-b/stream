#ifndef WINDOW_H
#define WINDOW_H

struct GLFWwindow;

namespace ST::UI
{

class Window
{
public:
    Window();
    virtual ~Window();
    int          exec();
    virtual void render() = 0;
    virtual void renderBackground();
    virtual void onClose() = 0;

protected:
    GLFWwindow* window_;
};

} // namespace ST::UI

#endif