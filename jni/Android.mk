LOCAL_PATH := $(call my-dir)

# ========== 预编译 driver.a 静态库 ==========
include $(CLEAR_VARS)
LOCAL_MODULE    := driver_prebuilt
LOCAL_SRC_FILES := include/driver.a
include $(PREBUILT_STATIC_LIBRARY)

# ========== 预编译 paradise 静态库（新增） ==========
include $(CLEAR_VARS)
LOCAL_MODULE    := paradise_prebuilt
LOCAL_SRC_FILES := include/paradise/libparadise_api.a
include $(PREBUILT_STATIC_LIBRARY)

# ========== 主模块 ==========
include $(CLEAR_VARS)

LOCAL_MODULE := Aurakernel.sh

LOCAL_CFLAGS   := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions
LOCAL_CFLAGS   += -std=c++17
LOCAL_CFLAGS   += -DVK_USE_PLATFORM_ANDROID_KHR

LOCAL_CPPFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions
LOCAL_CPPFLAGS += -Werror -std=c++20 -O3
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -Wall
LOCAL_CPPFLAGS += -DVK_USE_PLATFORM_ANDROID_KHR

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Touch
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/vulkan
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Kernel
LOCAL_C_INCLUDES += $(LOCAL_C_INCLUDES:$(LOCAL_PATH)/%:=%)

# 用 filter-out 排除 .bak 文件
FILE_LIST := $(filter-out %.bak, $(wildcard $(LOCAL_PATH)/src/*.c*))
FILE_LIST += $(filter-out %.bak, $(wildcard $(LOCAL_PATH)/src/ImGui/*.c*))
FILE_LIST += $(filter-out %.bak, $(wildcard $(LOCAL_PATH)/src/Touch/*.c*))
FILE_LIST += $(filter-out %.bak, $(wildcard $(LOCAL_PATH)/src/ue4math/*.c*))
FILE_LIST += $(filter-out %.bak, $(wildcard $(LOCAL_PATH)/src/vulkan/*.c*))
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM -lGLESv2 -lGLESv3
LOCAL_LDLIBS += -lz

LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/*.a
LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all,-llog
LOCAL_LDFLAGS += -lEGL -lGLESv2 -lGLESv3 -landroid -llog
LOCAL_LDFLAGS += -L$(LOCAL_PATH)/include/lib


# 链接 driver 静态库
LOCAL_STATIC_LIBRARIES := driver_prebuilt paradise_prebuilt

include $(BUILD_EXECUTABLE)