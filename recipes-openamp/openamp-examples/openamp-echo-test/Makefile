APP = echo_test-openamp

# Add any other object files to this list below
APP_OBJS =  echo_test.o
APP_OBJS += platform_info.o
APP_OBJS += rsc_table.o
APP_OBJS += helper.o

LDLIBS += -lmetal -lopen_amp

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS)

%.o: %c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(APP) *.o
