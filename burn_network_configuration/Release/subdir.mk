################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../sloeber.ino.cpp 

LINK_OBJ += \
./sloeber.ino.cpp.o 

CPP_DEPS += \
./sloeber.ino.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
sloeber.ino.cpp.o: ../sloeber.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/eric/Sloeber/arduinoPlugin/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r2-patch3/bin/xtensa-esp32-elf-g++" -DHAVE_CONFIG_H "-DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\"" -DUNITY_INCLUDE_CONFIG_H -DWITH_POSIX -D_GNU_SOURCE -DIDF_VER="v4.4.2" -DESP_PLATFORM -D_POSIX_READER_WRITER_LOCKS   "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/newlib/platform_include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/freertos/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/freertos/include/esp_additions/freertos" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/freertos/port/xtensa/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/freertos/include/esp_additions" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_hw_support/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_hw_support/include/soc" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_hw_support/include/soc/esp32" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_hw_support/port/esp32" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_hw_support/port/esp32/private_include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/heap/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/log/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/lwip/include/apps" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/lwip/include/apps/sntp" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/lwip/lwip/src/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/lwip/port/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/lwip/port/esp32/include/arch" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/soc/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/soc/esp32" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/soc/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/hal/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/hal/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/hal/platform_port/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_rom/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_rom/include/esp32" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_rom/esp32" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_common/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_system/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_system/port/soc" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_system/port/public_compat" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/xtensa/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/xtensa/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/driver/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/driver/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_pm/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_ringbuf/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/efuse/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/efuse/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/vfs/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_wifi/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_event/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_netif/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_eth/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/tcpip_adapter/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_phy/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_phy/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_ipc/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/app_trace/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_timer/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/mbedtls/port/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/mbedtls/mbedtls/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/mbedtls/esp_crt_bundle/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/app_update/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/spi_flash/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bootloader_support/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/nvs_flash/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/pthread/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_gdbstub/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_gdbstub/xtensa" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_gdbstub/esp32" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/espcoredump/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/espcoredump/include/port/xtensa" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/wpa_supplicant/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/wpa_supplicant/port/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/wpa_supplicant/esp_supplicant/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/ieee802154/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/console" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/asio/asio/asio/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/asio/port/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/common/osi/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/include/esp32/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/common/api/include/api" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/common/btc/profile/esp/blufi/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/common/btc/profile/esp/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/host/bluedroid/api/include/api" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/mesh_common/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/mesh_common/tinycrypt/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/mesh_core" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/mesh_core/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/mesh_core/storage" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/btc/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/mesh_models/common/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/mesh_models/client/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/mesh_models/server/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/api/core/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/api/models/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/bt/esp_ble_mesh/api" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/cbor/port/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/unity/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/unity/unity/src" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/cmock/CMock/src" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/coap/port/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/coap/libcoap/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/nghttp/port/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/nghttp/nghttp2/lib/includes" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-tls" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-tls/esp-tls-crypto" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_adc_cal/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_hid/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/tcp_transport/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_http_client/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_http_server/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_https_ota/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_https_server/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_lcd/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_lcd/interface" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/protobuf-c/protobuf-c" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/protocomm/include/common" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/protocomm/include/security" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/protocomm/include/transports" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/mdns/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_local_ctrl/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/sdmmc/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_serial_slave_link/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_websocket_client/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/expat/expat/expat/lib" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/expat/port/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/wear_levelling/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/fatfs/diskio" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/fatfs/vfs" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/fatfs/src" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/freemodbus/common/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/idf_test/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/idf_test/include/esp32" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/jsmn/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/json/cJSON" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/libsodium/libsodium/src/libsodium/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/libsodium/port_include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/mqtt/esp-mqtt/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/openssl/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/perfmon/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/spiffs/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/ulp/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/wifi_provisioning/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/rmaker_common/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/json_parser/upstream/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/json_parser/upstream" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/json_generator/upstream" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_schedule/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_rainmaker/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/gpio_button/button/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/qrcode/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/ws2812_led" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/dotprod/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/support/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/windows/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/windows/hann/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/windows/blackman/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/windows/blackman_harris/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/windows/blackman_nuttall/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/windows/nuttall/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/windows/flat_top/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/iir/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/fir/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/math/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/math/add/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/math/sub/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/math/mul/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/math/addc/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/math/mulc/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/math/sqrt/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/matrix/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/fft/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/dct/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/conv/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/common/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/kalman/ekf/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dsp/modules/kalman/ekf_imu13states/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp_littlefs/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include/tool" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include/typedef" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include/image" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include/math" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include/nn" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include/layer" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include/detect" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-dl/include/model_zoo" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-sr/src/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-sr/esp-tts/esp_tts_chinese/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp-sr/include/esp32" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp32-camera/driver/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/esp32-camera/conversions/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/include/fb_gfx/include" "-I/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/tools/sdk/esp32/dio_qspi/include" -mlongcalls -Wno-frame-address -ffunction-sections -fdata-sections -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wno-unused-parameter -Wno-sign-compare -ggdb -Os -freorder-blocks -Wwrite-strings -fstack-protector -fstrict-volatile-bitfields -Wno-error=unused-but-set-variable -fno-jump-tables -fno-tree-switch-conversion -std=gnu++11 -fexceptions -fno-rtti  -MMD -c -w -DF_CPU=240000000L -DARDUINO=10812 -DARDUINO_Node32s -DARDUINO_ARCH_ESP32 "-DARDUINO_BOARD=\"Node32s\"" -DARDUINO_VARIANT="node32s" -DARDUINO_PARTITION_default  -DESP32 -DCORE_DEBUG_LEVEL=0    -DARDUINO_USB_CDC_ON_BOOT=0   -I"/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/variants/node32s" -I"/home/eric/Sloeber/arduinoPlugin/packages/esp32/hardware/esp32/2.0.5/cores/esp32" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"

	@echo 'Finished building: $<'
	@echo ' '


