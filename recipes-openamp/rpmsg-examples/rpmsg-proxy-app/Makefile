
APP = proxy_app
APP_OBJS = proxy_app.o

# Add any other object files to this list below


all: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS)

clean:
	rm -rf $(APP) *.o

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

