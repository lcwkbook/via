
#include "辅助类.h"
#include "Font.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
extern 绘制 绘制;
// Var

EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;

// 本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险
ANativeWindow *native_window;
//
int native_window_screen_x = 0;
int native_window_screen_y = 0;
android::ANativeWindowCreator::DisplayInfo displayInfo{0};
uint32_t orientation = 0;
bool g_Initialized = false;
ImGuiWindow *g_window = nullptr;
int Orientation;

// 定义的全局声明
static std::atomic<bool> mirrorDisplayRunning(false);
static std::chrono::steady_clock::time_point lastMirrorCallTime =
    std::chrono::steady_clock::now() - std::chrono::seconds(6); // 延迟6s 减少占用
static std::mutex mirrorMutex;                                  // 定义互斥锁

// ========== 屏幕状态检测（息屏保活核心） ==========
// 多来源检测：内核背光亮度(首选，无 fork 无 binder) → dumpsys display → dumpsys power → 保持上次状态。
// 只有来源确凿时 g_screenConfirmed=true，允许据此做"连续失败→重启进程"的决定；
// 全部来源失败时保持上次状态（首次默认亮屏，避免启动后悬浮窗不显示），但不允许据此重启。
static bool g_screenConfirmed = false;

// 重建连续失败次数上限（约 15 秒），达到后请求看门狗重启换全新环境
static const int MAX_REBUILD_FAILS = 15;

bool IsScreenOn()
{
    static bool lastKnown = true;
    char buf[128] = {0};

    // 方式1（首选）：直接读内核背光亮度——无 fork、无 binder、毫秒级返回。
    //   >0 = 亮屏；读到 0 = 息屏（决定性）。
    //   ColorOS 常见路径（可先 adb shell "ls /sys/class/backlight/" 确认实际路径）
    static const char *backlightPaths[] = {
        "/sys/class/backlight/panel0/brightness",
        "/sys/class/backlight/panel1/brightness",
        "/sys/class/backlight/backlight/brightness",
        "/sys/class/leds/lcd-backlight/brightness",
    };
    for (const char *path : backlightPaths)
    {
        int fd = open(path, O_RDONLY);
        if (fd >= 0)
        {
            ssize_t n = read(fd, buf, sizeof(buf) - 1);
            close(fd);
            if (n > 0)
            {
                buf[n] = 0;
                int v = atoi(buf);
                g_screenConfirmed = true;
                lastKnown = (v > 0);
                return lastKnown;
            }
        }
    }

    // 方式2：dumpsys display 的 mScreenState（ON=亮屏，OFF/DOZE=息屏，Android 10+ 稳定）
    FILE *fp = popen("dumpsys display 2>/dev/null | grep -m1 'mScreenState='", "r");
    if (fp)
    {
        if (fgets(buf, sizeof(buf), fp))
        {
            pclose(fp);
            g_screenConfirmed = true;
            lastKnown = (strstr(buf, "=ON") != nullptr);
            return lastKnown;
        }
        pclose(fp);
    }

    // 方式3：dumpsys power 的 mWakefulness（备用）
    fp = popen("dumpsys power 2>/dev/null | grep -m1 'mWakefulness='", "r");
    if (fp)
    {
        if (fgets(buf, sizeof(buf), fp))
        {
            pclose(fp);
            g_screenConfirmed = true;
            lastKnown = (strstr(buf, "Awake") != nullptr);
            return lastKnown;
        }
        pclose(fp);
    }

    // 检测失败：保持上次已知状态（首次失败默认亮屏，避免启动后悬浮窗不显示）
    g_screenConfirmed = false; // 本次是猜的，不允许据此做"重启进程"的决定
    return lastKnown;
}

void AsyncProcessMirrorDisplay()
{
    // 互斥锁
    std::lock_guard<std::mutex> lock(mirrorMutex);

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastMirrorCallTime).count();

    // ★ 恢复 6 秒节流：原代码节流判断丢失（elapsed 成了死代码），
    //   导致镜像线程每帧尝试启动、ProcessMirrorDisplay 每 ~1 秒跑一次
    //   popen dumpsys + SurfaceFlinger 事务，反复干扰 AImGui 图层
    //   （present 返回 SURFACE_LOST，触发无限重建循环）。
    if (elapsed < 6)
    {
        return;
    }

    if (mirrorDisplayRunning)
    {
        // 跳过运行 防止内存泄露
        return;
    }

    // 设置运行标志
    mirrorDisplayRunning = true;
    printf("[调试] 镜像线程启动(每6秒最多一次)\n");

    // 创建线程处理
    std::thread mirrorThread([]()
                             {
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
        
        SURFACE_LOG_INFO("Mirror display processing finished"); });

    // 后台运行
    mirrorThread.detach();
}

bool initGUI_draw(uint32_t _screen_x, uint32_t _screen_y, bool log)
{
    orientation = displayInfo.orientation;
    InitVulkan();
    SetupVulkan();
    if (绘制.防录屏 == 1)
    {
        ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y, true);
    }
    else
    {
        ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y);
    }
    if (::native_window == nullptr)
    {
        printf("[错误] 创建悬浮窗窗口失败\n");
        return false;
    }
    SetupVulkanWindow(::native_window, (int)_screen_x, (int)_screen_y);
    if (!ImGui_init())
    {
        return false;
    }
    UploadFonts();
    return true;
}

bool init_egl(uint32_t _screen_x, uint32_t _screen_y, bool log)
{
    if (绘制.防录屏 == 1)
    {
        ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y, true);
    }
    else
    {
        ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y);
    }
    ANativeWindow_acquire(native_window);
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY)
    {
        return false;
    }
    if (eglInitialize(display, 0, 0) != EGL_TRUE)
    {
        return false;
    }
    EGLint num_config = 0;
    const EGLint attribList[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 5,    //-->delete
        EGL_GREEN_SIZE, 6,   //-->delete
        EGL_RED_SIZE, 5,     //-->delete
        EGL_BUFFER_SIZE, 32, //-->new field
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE};
    const EGLint attrib_list[] = {
        EGL_CONTEXT_CLIENT_VERSION,
        3,
        EGL_NONE};

    if (eglChooseConfig(display, attribList, &config, 1, &num_config) != EGL_TRUE)
    {
        return false;
    }
    EGLint egl_format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT)
    {
        return false;
    }
    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE)
    {
        return false;
    }
    if (!eglMakeCurrent(display, surface, surface, context))
    {
        return false;
    }
    return true;
}


void screen_config()
{
    displayInfo = android::ANativeWindowCreator::GetDisplayInfo();
    Orientation = displayInfo.orientation;


    // 根据防录屏设置决定是否启动镜像处理线程
    if (绘制.防录屏 == 0)
    {
        AsyncProcessMirrorDisplay();
    }
}


bool loadSystemFont()
{
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryTTF((void *)OPPOSans_H, OPPOSans_H_size, 28.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    return true;
}

bool ImGui_init()
{
    if (g_Initialized)
    {
        return true;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplAndroid_Init(native_window);
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;
    bool success = loadSystemFont();
    if (!success)
    {
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

    // ★ 如果 swapchain 需要重建
    if (g_SwapChainRebuild)
    {
        if (!IsScreenOn())
        {
            // 息屏时不重建：等亮屏后再重建（保留标志，下次进入时重试）
            return;
        }

        // ★ 重建节流：最多每秒尝试一次，避免息屏→亮屏抖动期间每帧狂试
        static auto lastRebuildAttempt = std::chrono::steady_clock::now() - std::chrono::seconds(2);
        auto rebuildNow = std::chrono::steady_clock::now();
        if (rebuildNow - lastRebuildAttempt < std::chrono::seconds(1))
            return;
        lastRebuildAttempt = rebuildNow;

        // ★ 重建连续失败计数（达到上限且屏幕确认亮着 → 重启进程换全新环境）
        static int rebuildFailCount = 0;

        // ★ 重建前检测 device 是否有效（显式判空：vkDeviceWaitIdle(NULL) 在部分 loader 上会返回成功）
        if (g_Device == VK_NULL_HANDLE || vkDeviceWaitIdle(g_Device) != VK_SUCCESS)
        {
            printf("[错误] Vulkan device 无效，重启进程恢复\n");
            fflush(stdout);
            _exit(42); // 42=请求看门狗重启
        }

        // ★ 重建前先验证 surface 是否还活着：
        //   息屏时 ColorOS 会销毁 AImGui 图层的 BufferQueue，native_window 已死，
        //   此时任何 surface/swapchain 操作（含 vkCreateSwapchainKHR）都会失败；
        //   重建窗口在息屏态有段错误风险，故不就地恢复：保留标志重试，连续失败后重启。
        VkSurfaceCapabilitiesKHR cap;
        if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_PhysicalDevice, wd->Surface, &cap) != VK_SUCCESS)
        {
            printf("[错误] Surface 已失效，重建重试中 (%d/%d)\n", rebuildFailCount + 1, MAX_REBUILD_FAILS);
            if (g_screenConfirmed && ++rebuildFailCount >= MAX_REBUILD_FAILS)
            {
                printf("[错误] Surface 持续失效，重启进程恢复\n");
                fflush(stdout);
                _exit(42);
            }
            return; // 保留 g_SwapChainRebuild 标志，下次再试
        }

        printf("[调试] 正在重建 Vulkan Swapchain...\n");

        // ★ 重建 surface（销毁旧 surface 重新创建）：即使 surface 句柄还活着，
        //   其 BufferQueue 连接可能已断（present 报 SURFACE_LOST），重建 surface
        //   可重新建立连接；若底层窗口已死，vkCreateSwapchainKHR 会失败走重试/重启。
        if (wd->Surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(g_Instance, wd->Surface, nullptr);
            wd->Surface = VK_NULL_HANDLE;
        }
        VkAndroidSurfaceCreateInfoKHR createInfo{
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .window = native_window,
        };
        VkResult surfaceErr = vkCreateAndroidSurfaceKHR(g_Instance, &createInfo, nullptr, &wd->Surface);
        if (surfaceErr != VK_SUCCESS || wd->Surface == VK_NULL_HANDLE)
        {
            printf("[错误] 创建 Surface 失败: %d，跳过重建\n", (int)surfaceErr);
            return; // 不清除标志，下次重试
        }

        ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
        ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device,
                                               wd,
                                               g_QueueFamily, g_Allocator,
                                               native_window_screen_x, native_window_screen_y,
                                               g_MinImageCount);
        wd->FrameIndex = 0;
        wd->SemaphoreIndex = 0; // 重建后帧同步索引复位，防止 ImageCount 变化后越界

        if (wd->Swapchain == VK_NULL_HANDLE)
        {
            printf("[错误] Swapchain 重建失败，重试中 (%d/%d)\n", rebuildFailCount + 1, MAX_REBUILD_FAILS);
            if (g_screenConfirmed && ++rebuildFailCount >= MAX_REBUILD_FAILS)
            {
                printf("[错误] Swapchain 持续重建失败，重启进程恢复\n");
                fflush(stdout);
                _exit(42); // 42=请求看门狗重启
            }
            return; // 保留 g_SwapChainRebuild 标志，稍后重试（不再原地崩溃）
        }

        rebuildFailCount = 0;
        g_SwapChainRebuild = false;
        printf("[调试] Vulkan Swapchain 重建成功\n");
    }

    if (g_SwapChainRebuild)
    {
        return;
    }

    if (::orientation != displayInfo.orientation)
    {
        ::orientation = displayInfo.orientation;
        UpdateScreenData(displayInfo.width, displayInfo.height, displayInfo.orientation);
        SwapChainRebuild(native_window_screen_x, native_window_screen_y);
    }

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplAndroid_NewFrame(native_window_screen_x, native_window_screen_y);
    ImGui::NewFrame();
}





void drawEnd()
{

    ImGui::Render();
    FrameRender(ImGui::GetDrawData());
    FramePresent();

    // ★ 修复息屏崩溃：息屏后 Vulkan 驱动内部的 swapchain/device 对象会失效，
    //   此时 wd->Swapchain 仍是旧句柄（非 NULL），再调用 vkGetSwapchainImagesKHR
    //   会因句柄查表失败解引用空指针而 SIGSEGV（tombstone 已证实崩溃在此处）。
    //   帧失效检测已由 FrameRender()/FramePresent() 的返回码完成（会置 g_SwapChainRebuild），
    //   这里不能再主动查询 swapchain 状态。
    if (g_SwapChainRebuild)
    {
        return; // 本帧已失效，跳过剩余逻辑，等 drawBegin() 亮屏后重建
    }

    int targetFps = 绘制.按钮.当前帧率;
    if (targetFps > 0)
    {
        static auto lastTime = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count();

        const long long FRAME_TIME = 1000000 / targetFps;

        if (elapsed < FRAME_TIME)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(FRAME_TIME - elapsed));
        }
        lastTime = now;
    }
}
