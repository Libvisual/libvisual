LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := ./include
LOCAL_MODULE	:= scriptable
LOCAL_SRC_FILES := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_CFLAGS    += $(ARCH_CFLAGS) -fexceptions -frtti
LOCAL_SHARED_LIBRARIES := gnustl luascript visual json
LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY)



