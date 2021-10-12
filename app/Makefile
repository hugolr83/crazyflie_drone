
# enable app support
APP=1
APP_STACKSIZE=300

VPATH += src/
PROJ_OBJ += $(patsubst src/%.c,%.o,$(wildcard src/*.c))

CRAZYFLIE_BASE=../crazyflie-firmware
include $(CRAZYFLIE_BASE)/Makefile
