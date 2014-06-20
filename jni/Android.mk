LOCAL_PATH:=$(call my-dir)/..
IRRPP_LIB_PATH := $(LOCAL_PATH)/lib/android


include $(CLEAR_VARS)

LOCAL_MODULE:=irrPP
IRRPP_LIB_NAME := lib$(LOCAL_MODULE).a

LOCAL_CFLAGS:=-D_IRR_ANDROID_PLATFORM_ -pipe -fno-exceptions -fno-rtti -fstrict-aliasing

ifndef NDEBUG
LOCAL_CFLAGS+=-g -D_DEBUG
else
LOCAL_CFLAGS+=-fexpensive-optimizations -O3
endif

ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS+=-fno-stack-protector
endif

LOCAL_C_INCLUDES:=../irrlicht-ogl-es/include include

LOCAL_SRC_FILES:=source/irrPP.cpp source/IQuadSceneNode.cpp source/CPostProcessingEffect.cpp source/CPostProcessingEffectChain.cpp source/IShaderDefaultPostProcessCallback.cpp

LOCAL_STATIC_LIBRARIES:=android_native_app_glue

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)

all: $(IRRPP_LIB_PATH)
$(IRRPP_LIB_PATH) : $(TARGET_OUT)/$(IRRPP_LIB_NAME)
	cp $< $@
