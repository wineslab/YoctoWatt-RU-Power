YOCTO_API_SRC = ../../Sources/
YOCTO_API_DIR_x86_64 = ../../Binaries/linux/x86_64/
DIR_x86_64 = Binary_Linux/x86_64/

OPTS_GENERIC = -std=c++17 -O2 -g -I$(YOCTO_API_SRC)
OPTS_x86_64 = -m64
OPTS_LINK = -lyocto-static -lm -lpthread -lusb-1.0

TARGET = $(DIR_x86_64)yoctowatt_test
SRC = main.cpp

default: $(TARGET)

$(TARGET): $(SRC) $(YOCTO_API_DIR_x86_64)libyocto-static.a | $(DIR_x86_64)
	@g++ $(OPTS_GENERIC) $(OPTS_x86_64) -o $@ $< -L$(YOCTO_API_DIR_x86_64) $(OPTS_LINK)

$(DIR_x86_64):
	@mkdir -p $@

clean:
	@rm -rf $(TARGET)

.PHONY: default clean