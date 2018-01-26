################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Drivers/ECC/ECCROMCC26XX.obj: C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/common/cc26xx/ecc/ECCROMCC26XX.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" --cmd_file="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/config/build_components.opt" --cmd_file="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/config/factory_config.opt" --cmd_file="C:/ti/ccsv7workspace/CC2640v130_Launchpad/hid_emu_kbd_cc2640r2lp_stack_library/TOOLS/build_config.opt"  -mv7M3 --code_state=16 -me -Ooff --opt_for_speed=0 --include_path="C:/ti/ccsv7workspace/CC2640v130_Launchpad/hid_keyboard_cc2640r2lp_app" --include_path="C:/ti/ccsv7workspace/CC2640v130_Launchpad/hid_keyboard_cc2640r2lp_app/Application" --include_path="C:/ti/ccsv7workspace/CC2640v130_Launchpad/hid_keyboard_cc2640r2lp_app/Startup" --include_path="C:/ti/ccsv7workspace/CC2640v130_Launchpad/hid_keyboard_cc2640r2lp_app/PROFILES" --include_path="C:/ti/ccsv7workspace/CC2640v130_Launchpad/hid_keyboard_cc2640r2lp_app/Include" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/controller/cc26xx_r2/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/icall/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/batt/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/dev_info" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/hid_dev/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/hid_dev_kbd" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/hid_dev_kbd/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/roles" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/roles/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/scan_param" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/profiles/scan_param/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/target" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/hal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/heapmgr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/icall/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/osal/src/inc" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/services/src/saddr" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/blestack/services/src/sdata" --include_path="C:/ti/simplelink_cc2640r2_sdk_1_30_00_25/source/ti/devices/cc26x0r2" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=BOARD_DISPLAY_USE_LCD=0 --define=BOARD_DISPLAY_USE_UART=0 --define=BOARD_DISPLAY_USE_UART_ANSI=0 --define=CC2640R2_LAUNCHXLx --define=CC2640R2_BLE_KB --define=CC26XX --define=CC26XX_R2 --define=DEVICE_FAMILY=cc26x0r2 --define=Display_DISABLE_ALL --define=HEAPMGR_SIZE=0 --define=ICALL_EVENTS --define=ICALL_JT --define=ICALL_LITE --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_MAX_NUM_TASKS=4 --define=ICALL_STACK0_ADDR --define=POWER_SAVING --define=STACK_LIBRARY --define=USE_CORE_SDK --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="Drivers/ECC/ECCROMCC26XX.d" --obj_directory="Drivers/ECC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


