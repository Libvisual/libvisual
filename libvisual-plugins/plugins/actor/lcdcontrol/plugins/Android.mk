LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= plugcpuinfo
LOCAL_SRC_FILES := PluginCpuinfo.cpp ../Hash.cpp ../debug.c
LOCAL_CFLAGS    += $(ARCH_CFLAGS) -fexceptions
LOCAL_SHARED_LIBRARIES := libvisual gnustl

LOCAL_LDLIBS += -L$(call host-path, $(LOCAL_PATH))/../$(TARGET_ARCH_ABI) -lluajit
#include $(BUILD_SHARED_LIBRARY)

