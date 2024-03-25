ZIGBEE ?= OFF

CC ?= gcc
CXX ?= g++
OBJEXT ?= .o
BUILD_PATH = build

CFLAGS = -Wno-unused-function -fno-integrated-as -fstrict-aliasing -fPIC -Os -ffunction-sections -fdata-sections
CXXFLAGS = -std=c++17 -Os -ffunction-sections -fdata-sections -Wno-unused-result -Wno-deprecated-declarations
LDFLAGS = -Wl,--gc-sections -Os -ffunction-sections -fdata-sections

INCLUDES = -I. -Ibutton -Iconfig -Iobject -Idevice/ble -Idevice/mqtt -Igateway -Ijson -Ilog -Imqtt -Ihttp -Iprotocol/ble -Iuart -Iutil -Iwifi -Itimer
LINKEDLIBS = -lmosquittopp -lsqlite3 -pthread -lcurl -lssl -lcrypto

DEVICESRC += $(wildcard button/*.cpp)
DEVICESRC += $(wildcard config/*.cpp)
DEVICESRC += $(wildcard object/*.cpp)
DEVICESRC += $(wildcard device/ble/*.cpp)
DEVICESRC += $(wildcard device/ble/module/*.cpp)
DEVICESRC += $(wildcard device/mqtt/*.cpp)
DEVICESRC += $(wildcard device/mqtt/function/*.cpp)
DEVICESRC += $(wildcard gateway/*.cpp)
DEVICESRC += $(wildcard json/*.cpp)
DEVICESRC += $(wildcard log/*.cpp)
DEVICESRC += $(wildcard mqtt/*.cpp)
DEVICESRC += $(wildcard http/*.cpp)
DEVICESRC += $(wildcard protocol/ble/*.cpp)
DEVICESRC += $(wildcard uart/*.cpp)
DEVICESRC += $(wildcard util/*.cpp)
DEVICESRC += $(wildcard wifi/*.cpp)
DEVICESRC += $(wildcard timer/*.cpp)

CPPSRC = $(wildcard *.cpp) $(DEVICESRC)
CPPOBJ = $(CPPSRC:.cpp=$(OBJEXT))
BUILTOBJ = $(addprefix $(BUILD_PATH)/,$(CPPOBJ))

APP = demo

all: $(APP)

.PHONY: all $(APP) clean

$(APP): $(BUILTOBJ)
	$(CXX) $(LDFLAGS) -o $@ $(BUILTOBJ) $(LINKEDLIBS)
	
$(BUILD_PATH)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_PATH)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@
	
install: $(APP)
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 744 $(APP) $(DESTDIR)$(PREFIX)/bin/

clean: 
	rm -rf $(APP) $(BUILD_PATH)
