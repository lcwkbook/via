#ifndef __VULKANUTILS_G_H__
#define __VULKANUTILS_G_H__

//#include <cstring>
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "vulkan_wrapper.h"


// A struct to manage data related to one image in vulkan
struct MyTextureData {
    VkDescriptorSet DS;         // Descriptor set: this is what you'll pass to Image()
    int Width;
    int Height;
    int Channels;

    // Need to keep track of these to properly cleanup
    VkImageView ImageView;
    VkImage Image;
    VkDeviceMemory ImageMemory;
    VkSampler Sampler;
    VkBuffer UploadBuffer;
    VkDeviceMemory UploadBufferMemory;

    MyTextureData() { memset(this, 0, sizeof(*this)); }
};

bool LoadTextureFromFile(const char *filename, MyTextureData *tex_data);

bool LoadTextureFromMemory(const void *filedata, int len, MyTextureData *tex_data);

MyTextureData createTexture_ALL_FromMem(const void *filedata, int len);

void RemoveTexture(MyTextureData *tex_data);

void SetupVulkan();

void SetupVulkanWindow(ANativeWindow *window, int width, int height);

void UploadFonts();

void SwapChainRebuild(int w, int h);

extern VkDevice g_Device;
extern ImGui_ImplVulkanH_Window* wd;
extern bool g_SwapChainRebuild;

// ★ 新增：暴露这些变量给 draw.cpp 使用
extern VkInstance g_Instance;
extern VkPhysicalDevice g_PhysicalDevice;
extern uint32_t g_QueueFamily;
extern VkAllocationCallbacks* g_Allocator;
extern int g_MinImageCount;
extern ImGui_ImplVulkanH_Window g_MainWindowData;

void FrameRender(ImDrawData *draw_data);

void FramePresent();

void DeviceWait();

void CleanupVulkanWindow();

void CleanupVulkan();

#endif // __VULKAN_WRAPPER_G_H__
