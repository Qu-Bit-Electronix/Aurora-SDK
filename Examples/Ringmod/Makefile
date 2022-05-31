# Project Name
TARGET = Ringmod

# Build Project for Daisy Bootloader
APP_TYPE = BOOT_SRAM

# Sources
CPP_SOURCES = Ringmod.cpp

# Path to the root of the Aurora-SDK
# When building custom applications outside of this repo
# update this to point to the Aurora-SDK/ folder
AURORA_SDK_PATH = ../..

# Location of Hardware Support File within the SDK
C_INCLUDES += -I$(AURORA_SDK_PATH)/include/

# Library Locations
LIBDAISY_DIR = $(AURORA_SDK_PATH)/libs/libDaisy/
DAISYSP_DIR = $(AURORA_SDK_PATH)/libs/DaisySP/

# To DEBUG the project with an ST-Link Probe:
# 1. Compile the program with the below lines uncommented
# 2. Load the firmware via the USB drive
# 3. Make sure your .vscode/launch.json points to the 
#	 build/*.elf for the desired program
# 4. Navigate and run the "Cortex Debug" Run and Debug configuration
#    or simply press F5 in VS Code.

# DEBUG = 1
# OPT = -O0

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
