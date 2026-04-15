LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Vkernel.sh

LOCAL_CFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions
LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all,-llog
LOCAL_CPPFLAGS += -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++17 -O0
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions
LOCAL_CFLAGS := -std=c++17



LOCAL_CFLAGS += -DVK_USE_PLATFORM_ANDROID_KHR
LOCAL_CPPFLAGS += -DVK_USE_PLATFORM_ANDROID_KHR


LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Touch
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/vulkan
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Kernel


FILE_LIST += $(wildcard $(LOCAL_PATH)/src/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ImGui/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Touch/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ue4math/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/vulkan/*.c*)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)



LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM -lGLESv2 -lGLESv3

LOCAL_LDFLAGS += -L$(LOCAL_PATH)/include/lib
LOCAL_LDLIBS  += -lcurl


include $(BUILD_EXECUTABLE)


# Android.mk 中添加
LOCAL_LDLIBS += -lc++  # 链接NDK的libc++库
LOCAL_CPPFLAGS += -stdlib=libc++ -std=c++11  # 指定C++标准和stdlib