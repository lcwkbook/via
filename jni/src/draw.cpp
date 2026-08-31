
#include "辅助类.h"
#include "Font.h" 
extern 绘制 绘制; 

// Var
 
EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;

//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险
ANativeWindow *native_window;
//
int native_window_screen_x = 0;
int native_window_screen_y = 0;
android::ANativeWindowCreator::DisplayInfo displayInfo{0};
uint32_t orientation = 0;
bool g_Initialized = false;
ImGuiWindow *g_window = nullptr;
int Orientation;


//定义的全局声明
static std::atomic<bool> mirrorDisplayRunning(false);
static std::chrono::steady_clock::time_point lastMirrorCallTime = 
    std::chrono::steady_clock::now() - std::chrono::seconds(6); //延迟6s 减少占用
static std::mutex mirrorMutex; //定义互斥锁

void AsyncProcessMirrorDisplay() {
    //互斥锁
    std::lock_guard<std::mutex> lock(mirrorMutex);
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastMirrorCallTime).count();
    
    if (mirrorDisplayRunning) {
        //跳过运行 防止内存泄露
        return;
    }

    //设置运行标志
    mirrorDisplayRunning = true;
    
    //创建线程处理
    std::thread mirrorThread([]() {
        SURFACE_LOG_INFO("Starting mirror display processing in thread %lu", 
                         std::this_thread::get_id());
        
        try {
            android::ANativeWindowCreator::ProcessMirrorDisplay();
            SURFACE_LOG_INFO("Mirror display processing completed successfully");
        } catch (const std::exception& e) {
            SURFACE_LOG_ERROR("Mirror display error: %s", e.what());
        } catch (...) {
            SURFACE_LOG_ERROR("Unknown error in mirror display");
        }
        
        //更新状态
        {
            std::lock_guard<std::mutex> lock(mirrorMutex);
            lastMirrorCallTime = std::chrono::steady_clock::now();
            mirrorDisplayRunning = false;
        }
        
        SURFACE_LOG_INFO("Mirror display processing finished");
    });
    
    //后台运行
    mirrorThread.detach();
}

bool initGUI_draw(uint32_t _screen_x, uint32_t _screen_y, bool log) {
  orientation = displayInfo.orientation;
  InitVulkan();
  SetupVulkan();
  if (绘制.防录屏 == 1)
  {
    ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y,true);
  } else {
    ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y);
  }
  SetupVulkanWindow(::native_window, (int) _screen_x, (int) _screen_y);        
  if (!ImGui_init()) {
    return false;
  }
  UploadFonts();
  return true;
}

bool init_egl(uint32_t _screen_x, uint32_t _screen_y, bool log) {
  if (绘制.防录屏 == 1)
  {
    ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y,true);
  } else {
    ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y);
  }
    ANativeWindow_acquire(native_window);
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        return false;
    }
    if (eglInitialize(display, 0, 0) != EGL_TRUE) {
        return false;
    }
    EGLint num_config = 0;
    const EGLint attribList[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 5,   //-->delete
            EGL_GREEN_SIZE, 6,  //-->delete
            EGL_RED_SIZE, 5,    //-->delete
            EGL_BUFFER_SIZE, 32,  //-->new field
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    const EGLint attrib_list[] = {
            EGL_CONTEXT_CLIENT_VERSION,
            3,
            EGL_NONE
    };

    if (eglChooseConfig(display, attribList, &config, 1, &num_config) != EGL_TRUE) {
        return false;
    }
    EGLint egl_format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT) {
        return false;
    }
    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        return false;
    }
    if (!eglMakeCurrent(display, surface, surface, context)) {
        return false;
    }
    return true;
}

/*
void screen_config() {
    displayInfo = android::ANativeWindowCreator::GetDisplayInfo();
    Orientation = displayInfo.orientation;
}*/

void screen_config()
{
    displayInfo = android::ANativeWindowCreator::GetDisplayInfo();//本来就有的
    Orientation = displayInfo.orientation;//本来就有的
    // 根据防录屏设置决定是否启动镜像处理线程
    if (绘制.防录屏 == 0) // 防录屏关闭时调用
    {
        AsyncProcessMirrorDisplay(); // 启动线程处理镜像显示
    }
    // 防录屏开启时则不调用线程
}
bool loadSystemFont() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryTTF((void*)OPPOSans_H, OPPOSans_H_size, 28.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());    
    return true;
}

bool ImGui_init() {
    if (g_Initialized) {
        return true;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplAndroid_Init(native_window);
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;
    bool success = loadSystemFont();
    if (!success) {
        return false;
    }
    ImGui::GetStyle().ScaleAllSizes(3.0f);
    ImGui::GetStyle().ScrollbarSize = 25.0f;
    ::g_Initialized = true;
    return true;
}

int 布局::初始化程序()
{
  // 获取屏幕信息
  screen_config();
  ::abs_ScreenX = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
  ::abs_ScreenY = (displayInfo.height < displayInfo.width ? displayInfo.height : displayInfo.width);

  ::native_window_screen_x = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
  ::native_window_screen_y = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
  // 初始化imgui
  if (!initGUI_draw(native_window_screen_x, native_window_screen_y, true))
  {
    return -1;
  }
  Touch_Init(displayInfo.width, displayInfo.height, displayInfo.orientation, true);
  return 0;
}

void drawBegin()
{
  screen_config();

  if (::orientation != displayInfo.orientation)
  {
    ::orientation = displayInfo.orientation;
    UpdateScreenData(displayInfo.width, displayInfo.height, displayInfo.orientation);
  }
  ImGui_ImplVulkan_NewFrame();

  ImGui_ImplAndroid_NewFrame(native_window_screen_x, native_window_screen_y);
  ImGui::NewFrame();
}

// draw.cpp 的 drawEnd()
void drawEnd() {
    ImGui::Render();
    FrameRender(ImGui::GetDrawData());
    FramePresent();
    
    int targetFps = 绘制.按钮.当前帧率;
    if (targetFps > 0)
    {
        static auto lastTime = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count();
        
        const long long FRAME_TIME = 1000000 / targetFps;
        
        // ★ 核心修复：如果实际帧时间已经超过目标，不再sleep！
        //   否则降频时 sleep 会加剧掉帧
        if (elapsed < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::microseconds(FRAME_TIME - elapsed));
        }
        // ★ 如果 elapsed >= FRAME_TIME，说明CPU已经跑不动了
        //   直接跳过sleep，全力跑，不额外增加延迟
        lastTime = now;
    }
}



