# BoardConfigVendor.mk
# Qualcomm Technologies proprietary product specific compile-time definitions.

BOARD_USES_GENERIC_AUDIO := false
USE_CAMERA_STUB := false

BOARD_USES_QCOM_HARDWARE := true
DYNAMIC_SHARED_LIBV8SO := true
BOARD_USES_ADRENO := true
HAVE_ADRENO_SOURCE := true
HAVE_ADRENO_SC_SOURCE := true
HAVE_ADRENO_FIRMWARE := true
BOARD_USES_QCOM_AUDIO_V2 := true
BOARD_USE_QCOM_TESTONLY := true
TARGET_USES_SF_BYPASS := true
TARGET_USES_ION := true
TARGET_USES_C2D_COMPOSITION := true
BOARD_USES_QCNE := true
TARGET_AVOID_DRAW_TEXTURE_EXTENSION := true
TARGET_HAS_S3D_SUPPORT := true
USE_OPENGL_RENDERER := true
TARGET_HAVE_BYPASS := true
TARGET_MAX_BYPASS := 3
PROTEUS_DEVICE_API := false
ENABLE_WEBGL := true
BOARD_USE_QCOM_LLVM_CLANG_RS := true
TARGET_USES_QCOM_MM_AUDIO := true
MM_AUDIO_OMX_ADEC_EVRC_DISABLED := false
MM_AUDIO_OMX_ADEC_QCELP13_DISABLED := false
MM_AUDIO_FTM_DISABLED := false
MM_AUDIO_MEASUREMENT_DISABLED := false
MM_AUDIO_VOEM_DISABLED := false
TARGET_NO_RPC := false
TARGET_HAVE_TSLIB := false
TARGET_USES_ASHMEM := true

ifneq ($(BUILD_TINY_ANDROID), true)
  BOARD_VENDOR_QCOM_GPS_LOC_API_AMSS_VERSION := 50001
  BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := default
  BOARD_CAMERA_LIBRARIES := libcamera
  BOARD_HAVE_BLUETOOTH := true
  BOARD_HAVE_QCOM_FM := true
  BOARD_HAS_QCOM_WLAN := true
  CONFIG_EAP_PROXY := qmi
  BOARD_WPA_SUPPLICANT_DRIVER := NL80211
  BOARD_HOSTAPD_DRIVER := NL80211
  WPA_SUPPLICANT_VERSION := VER_0_8_X
  HOSTAPD_VERSION := VER_0_8_X
  WIFI_SDIO_IF_DRIVER_MODULE_PATH := "/system/lib/modules/librasdioif.ko"
  WIFI_SDIO_IF_DRIVER_MODULE_NAME := "librasdioif"
  WIFI_SDIO_IF_DRIVER_MODULE_ARG  := ""
  WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/wlan.ko"
  WIFI_DRIVER_MODULE_NAME := "wlan"
  WIFI_DRIVER_MODULE_ARG := ""
  HAVE_QC_TIME_SERVICES := true
  WIFI_DRIVER_FW_PATH_STA := "sta"
  WIFI_DRIVER_FW_PATH_AP  := "ap"
  WIFI_DRIVER_FW_PATH_P2P := "p2p"
  BOARD_WLAN_DEVICE := qcwcn
endif   # !BUILD_TINY_ANDROID
