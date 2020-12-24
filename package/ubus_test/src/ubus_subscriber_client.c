/*
*   Here is a Subscribler client
*/
#include <unistd.h>  
#include <libubox/blobmsg_json.h>  
#include <libubox/uloop.h>  
#include <libubus.h>  
  
static struct ubus_context *ctx;  
  
static int counter = 0;  
static uint32_t obj_id;  
static struct ubus_subscriber test_event;  
  
static int test_notify(struct ubus_context *ctx, struct ubus_object *obj,  
                  struct ubus_request_data *req,  
                  const char *method, struct blob_attr *msg)  
{  
    printf("Recevive Server notify...\n");  
    counter++;  
    if (counter > 3){
        printf("Enough, let's break the connect!\n");
        ubus_unsubscribe(ctx, &test_event, obj_id); 
    }

    return 0;  
}  
  
static void test_handle_remove(struct ubus_context *ctx,  
                      struct ubus_subscriber *obj, uint32_t id)  
{  
    printf("remove handler...\n");  
}  
  
static void subscriber_main(void)  
{  
    int ret;     
    /* 通知到来时的处理函数。 */  
    test_event.cb = test_notify;  
    test_event.remove_cb = test_handle_remove; //server主动发起删除该client的订阅的cb函数（如server退出的时候）  
  
    /* 注册test_event */  
    ret = ubus_register_subscriber(ctx, &test_event);  
    if (ret)  
        fprintf(stderr, "Failed to add watch handler: %s\n", ubus_strerror(ret));  
      
    /* 得到要订阅的object的id */  
    ret = ubus_lookup_id(ctx, "mybroadcast", &obj_id);  
    if (ret)  
        fprintf(stderr, "Failed to lookup object: %s\n", ubus_strerror(ret));  
  
    /* 订阅object */  
    ret = ubus_subscribe(ctx, &test_event, obj_id);  
    if (ret)  
        fprintf(stderr, "Failed to subscribe: %s\n", ubus_strerror(ret));  
}  
  
int main(int argc, char **argv)  
{  
    const char *ubus_socket = NULL;  
  
    uloop_init();  
  
    ctx = ubus_connect(ubus_socket);  
    if (!ctx) {  
        fprintf(stderr, "Failed to connect to ubus\n");  
        return -1;  
    }  
  
    ubus_add_uloop(ctx);  
  
    subscriber_main();  
      
    uloop_run();  
  
    ubus_free(ctx);  
    uloop_done();  
  
    return 0;  
}  