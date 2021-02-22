/*
*   Invoke object client
*/
#include <stdio.h>
#include <unistd.h>
#include <libubus.h>
#include <libubox/uloop.h>
#include <libubox/blobmsg.h>

struct ubus_context *ctx;
struct blob_buf b;
char* obj_name = "kuei_obj";

static const struct blobmsg_policy rtn_policy[] =
{
    [0] = { .name="return", .type=BLOBMSG_TYPE_STRING},
};

static void result_handler(struct ubus_request *req, int type, struct blob_attr *msg);

//look up for object exist or not
static int client_ubus_findo(){
    unsigned int host_id;
    int ret;
    ret = ubus_lookup_id(ctx, obj_name, &host_id); 
    if(ret != UBUS_STATUS_OK){
        printf("Target object is nonexist");
        return ret;
    }
    printf(" FOUND THE OBJECT %s\n",obj_name);
    return host_id;
}

//connect to uloop
static int client_ubus_init(const char *path){
    uloop_init();
    ctx = ubus_connect(path);
    if(!ctx){
        printf("ubus connect fail\n");
        return -1;
    }
    printf("ubus connected as %08x\n",ctx->local_id);
    return 0;
}

//end uloop
static void client_ubus_done(void){
    if(ctx)
        ubus_free(ctx);
}

int main(void){
    unsigned int host_id;
    char *path = NULL;
    if(client_ubus_init(path) < 0){
        printf("init fail \n");
        printf("leaving process..\n");
        return -1;
    }
    host_id = client_ubus_findo();

    /* ubus message */
    blob_buf_init(&b ,0);
    blobmsg_add_string(&b, "echo","This is test Message!");

    //invoke ubus server
    int i;
    for(i=0;i<5;i++){
        ubus_invoke(ctx , host_id , "KUEI_ECHO" , b.head , result_handler , 0 , 3*1000);
        sleep(2);
    }
    client_ubus_done();
    return 0;
}
 
/* callback function for ubus_invoke to process result from the host
 * Here we just print out the message.
 */
static void result_handler(struct ubus_request *req, int type, struct blob_attr *msg)
{
        struct blob_attr *tb[1];
        if(!msg)
                return;
 
        blobmsg_parse(rtn_policy, 1, tb, blob_data(msg), blob_len(msg));
        printf("MESSAGE : %s\n",blobmsg_get_string(tb[0]));
}
