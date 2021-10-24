#include "App.h"
#include <sstream>
#include <iomanip>
#include<cmath>
App::App()
    :
    wnd(800, 600, "The Donkey Fart Box") {
}

int App::Go() {
    while (true) {
        // process all messages pending, but to not block for new messages
        if (const auto ecode = Window::ProcessMessages()) {
            // if return optional has value, means we're quitting so return exit code
            return *ecode;
        }
        DoFrame();
    }
}

void App::DoFrame() {
    while (const auto e = wnd.mouse.Read()) {
        switch (e->GetType()) {
        case Mouse::Event::Type::Move:
        {
            std::ostringstream oss;
            oss << "Mouse moved to: (" << e->GetPosX() << "," << e->GetPosY() << ")\n";
            wnd.SetTitle(oss.str());
            break;
        }
        }
    }
    /*const float r = abs(sin(timer.Peek()));
    const float g = abs(cos(timer.Peek()));
    const float b = abs(sin(timer.Peek()));
    wnd.Gfx().ClearBuffer(r, g, b);*/
    wnd.Gfx().DrawTestTriangle();
    wnd.Gfx().EndFrame();
}