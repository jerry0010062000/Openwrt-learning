APP_SRCS := $(wildcard *.c)
APP_OBJS := $(addsuffix .o, $(basename ${APP_SRCS}))
APP_CFLAGS := -g -Wall -Werror $(CFLAGS) -I. 

all: $(APP_OBJS)
	$(CC) ubus_server_example.o $(LDFLAGS) -lubus -lubox -o ubus_server
	$(CC) ubus_client_example.o $(LDFLAGS) -lubus -lubox -o ubus_client
	$(CC) ubus_notify_server.o $(LDFLAGS) -lubus -lubox -o ubus_sserver
	$(CC) ubus_subscriber_client.o $(LDFLAGS) -lubus -lubox -o ubus_sclient
	$(CC) open_uci_config.o $(LDFLAGS) -luci -o read_uci

%.o: %.c
	$(CC) $(APP_CFLAGS) -c $< -o $@
	
clean:
	rm *.o