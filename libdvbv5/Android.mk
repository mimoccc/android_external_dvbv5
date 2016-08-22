LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	compat-soname.c \
	crc32.c \
	countries.c \
	dvb-legacy-channel-format.c \
	dvb-zap-format.c \
	dvb-vdr-format.c \
	dvb-v5.c	 \
	parse_string.c	 \
	dvb-demux.c	 \
	dvb-fe.c	 \
	dvb-log.c	 \
	dvb-file.c	 \
	dvb-v5-std.c	 \
	dvb-sat.c	 \
	dvb-scan.c	 \
	descriptors.c	 \
	tables/header.c		\
	tables/pat.c		\
	tables/pmt.c		\
	tables/nit.c		\
	tables/sdt.c		\
	tables/vct.c		\
	tables/mgt.c		\
	tables/eit.c		\
	tables/cat.c		\
	tables/atsc_eit.c	\
	tables/mpeg_ts.c	\
	tables/mpeg_pes.c	\
	tables/mpeg_es.c	\
	descriptors/desc_language.c		\
	descriptors/desc_network_name.c		\
	descriptors/desc_cable_delivery.c	\
	descriptors/desc_sat.c			\
	descriptors/desc_terrestrial_delivery.c \
	descriptors/desc_t2_delivery.c		\
	descriptors/desc_service.c		\
	descriptors/desc_frequency_list.c	\
	descriptors/desc_event_short.c		\
	descriptors/desc_event_extended.c	\
	descriptors/desc_atsc_service_location.c \
	descriptors/desc_hierarchy.c		\
	descriptors/desc_extension.c		\
	descriptors/desc_isdbt_delivery.c	\
	descriptors/desc_logical_channel.c	\
	descriptors/desc_ts_info.c		\
	descriptors/desc_partial_reception.c	\
	descriptors/desc_ca.c			\
	descriptors/desc_ca_identifier.c

LOCAL_CFLAGS := \
		-Wno-missing-field-initializers \
		-Wno-unused-parameter \
		-Wno-sign-compare \
		-Wno-initializer-overrides \
		-Wno-absolute-value

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
LOCAL_MODULE := libdvbv5
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
