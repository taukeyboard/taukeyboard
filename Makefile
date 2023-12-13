WCHEVT_SDK_BASE = .
export WCHEVT_SDK_BASE

DEBUG?=n
BOARD?=ch582evt
DEMO_TYPE?=device
PORT_SPEED?=hs

CMAKE = cmake

# avoid 'Entering|Leaving directory' messages
MAKEFLAGS += --no-print-directory

RM = $(CMAKE) -E remove_directory

#cmake definition config
cmake_definition+= -DBOARD=${BOARD}
cmake_definition+= -DDEBUG=${DEBUG}
cmake_definition+= -DDEMO_TYPE=${DEMO_TYPE}
cmake_definition+= -DPORT_SPEED=${PORT_SPEED}

build:
	@$(CMAKE) -S . -B build -G "Unix Makefiles" $(cmake_definition)
	@cd build && make -j4

clean:
	@$(RM) build

.PHONY:build clean menuconfig

