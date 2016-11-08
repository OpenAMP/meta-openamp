APP = libmetal-amp-demo

# Add any other object files to this list below
APP_OBJS = libmetal_amp_demo.o init_linux.o

LDLIBS += -lmetal

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS)

%.o: %c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(APP) *.o
