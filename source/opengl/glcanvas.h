#ifndef GLCANVAS_H
#define GLCANVAS_H

#define GLFW_INCLUDE_ES3
#include <GLES2/gl2.h>

#include <GLFW/glfw3.h>
#include "../math/region.h"
#include "glwrap.h"
#include "shaders.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <string>
#else
#include <thread>
#endif

#include <functional>
#include <vector>

namespace emp {
  namespace opengl {

#ifdef __EMSCRIPTEN__
    unsigned int getWindowWidth() {
      return EM_ASM_INT({ return window.innerWidth; });
    }

    unsigned int getWindowHeight() {
      return EM_ASM_INT({ return window.innerHeight; });
    }
#endif

    class GLCanvas {
      private:
      unsigned int width, height;
      Region2D<int> region;

#ifdef __EMSCRIPTEN__
      std::string id;
      EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;
#else
      GLFWwindow* window = nullptr;
#endif

      std::vector<std::function<void(GLCanvas&, int, int)>> onresize;

      void resizeViewport(float width, float height) {
        // auto size = std::max(height, width);
        // glViewport((width - size) / 2, (height - size) / 2, size, size);
        // region.min = {0, 0};
        // region.max = {size, size};

        glViewport(0, 0, width, height);
        region.min = {0, 0};
        region.max = {width, height};

#ifdef __EMSCRIPTEN__
        EM_ASM_(
          {
            const id = Module.Pointer_stringify($0);
            const canvas = document.getElementById(id);
            canvas.width = $1;
            canvas.height = $2;
          },
          id.c_str(), size, size);
#endif
      }

      public:
      GLCanvas(int width, int height, const char* title = "empirical")
        : width(width),
          height(height)
#ifdef __EMSCRIPTEN__

          ,
          id(title)
#endif
      {
#ifdef __EMSCRIPTEN__
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.majorVersion = 2;
        attrs.minorVersion = 0;
        context = emscripten_webgl_create_context(title, &attrs);

#else
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 8);

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(
          window, [](GLFWwindow* window, int width, int height) {
            GLCanvas* me =
              static_cast<GLCanvas*>(glfwGetWindowUserPointer(window));

            me->width = width;
            me->height = height;
            me->resizeViewport(width, height);

            for (auto& callback : me->onresize) callback(*me, width, height);
          });
#endif
        makeCurrent();
        resizeViewport(width, height);
      }

#ifdef __EMSCRIPTEN__
      GLCanvas(const char* title = "empirical")
        : GLCanvas(getWindowWidth(), getWindowHeight(), title) {}
#else
      GLCanvas(const char* title = "empirical") : GLCanvas(800, 600, title) {}
#endif
      GLCanvas(const GLCanvas&) = delete;
      GLCanvas(GLCanvas&& other) = delete;
      GLCanvas& operator=(const GLCanvas&) = delete;
      GLCanvas& operator=(GLCanvas&& other) = delete;

      ~GLCanvas() {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#else
        if (window != nullptr) {
          glfwDestroyWindow(window);
        }
        glfwTerminate();
#endif
      }

      template <typename F>
      void onresized(F&& callback) {
        onresize.push_back(std::forward<F>(callback));
      }

      template <typename R>
      void runForever(R&& onUpdate, int fps = -1, bool forever = true) {
        makeCurrent();
#ifdef __EMSCRIPTEN__
        auto args = std::make_tuple(std::forward<R>(onUpdate), this);
        emscripten_set_main_loop_arg(
          [](void* argsUnsafe) {
            auto a = reinterpret_cast<decltype(args)*>(argsUnsafe);
            std::get<0> (*a)(*std::get<1>(*a));
          },
          &args, fps, forever);
#else
        if (fps <= 0) fps = 60;

        const int frameLength = 1000.f / fps;
        while (!glfwWindowShouldClose(window)) {
          onUpdate(*this);

          glfwSwapBuffers(window);
          glfwPollEvents();

          std::this_thread::sleep_for(std::chrono::milliseconds(frameLength));
        }
#endif
      }

      void makeCurrent() {
#ifdef __EMSCRIPTEN__
        emscripten_webgl_make_context_current(context);
#else
        glfwMakeContextCurrent(window);
#endif
      }

      template <typename... Args>
      VertexArrayObjectConfigurator makeVAO(Args&&... args) const {
        return {std::forward<Args>(args)...};
      }
      template <typename... Args>
      ShaderProgram makeShaderProgram(Args&&... args) const {
        return {std::forward<Args>(args)...};
      }

      auto getWidth() const { return width; }
      auto getHeight() const { return height; }

      auto getRegion() const { return region; }
    };

  }  // namespace opengl
}  // namespace emp

#endif