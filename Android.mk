DVB_PATH:= $(call my-dir)
include $(call all-makefiles-under, $(DVB_PATH))

LOCAL_PATH := $(DVB_PATH)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := dvb-fe-tool.c


LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/libdvbv5/include

LOCAL_CFLAGS += -Wno-int-conversion -Wno-implicit-function-declaration

LOCAL_SHARED_LIBRARIES := libdvbv5

LOCAL_MODULE := dvb-fe-tool
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := dvbv5-zap.c


LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/libdvbv5/include

LOCAL_CFLAGS += -Wno-int-conversion -Wno-implicit-function-declaration

LOCAL_SHARED_LIBRARIES := libdvbv5

LOCAL_MODULE := dvbv5-zap
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)


