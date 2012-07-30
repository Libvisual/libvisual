LOCAL_PATH := $(call my-dir)

# include the profiler snippet
#-include android-ndk-profiler.mk

# this is your shared library
include $(CLEAR_VARS)


LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= visual
#LOCAL_SHARED_LIBRARIES := lua 

LOCAL_CFLAGS    += $(ARCH_CFLAGS) -Iprivate/

LOCAL_LDLIBS    := -lm -ldl -llog 

#profiling
#LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/$(TARGET_ARCH_ABI) -landprof
#LOCAL_CFLAGS += -pg -DVISUAL_HAVE_PROFILING -fno-omit-frame-pointer -fno-function-sections

PRIV := private/lv_video_convert.c  private/lv_video_fill.c  private/lv_video_scale.c

LOCAL_SRC_FILES := $(PRIV) $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_CFLAGS    += $(ARCH_CFLAGS)

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_ARM_NEON  := true
endif

LOCAL_SHARED_LIBRARIES := cpufeatures

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)
$(call import-module,cpufeatures) 
