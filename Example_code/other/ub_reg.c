//ubus registered object server (test file)
#include <stdio.h>
#include <unistd.h>
#include <libubus.h>
#include <libubox/uloop.h>

static struct ubus_context *ctx;
static struct ubus_request_data req_data;
static struct blob_buf bb;	//暫存傳進來的message
static int kuei_counter =0;

//宣告當被呼叫到時的觸發函數 實作在後面
static int cnt_handler( struct ubus_context *ctx, struct ubus_object *obj,struct ubus_request_data *req, const char *method,struct blob_attr *msg );
static int echo_handler( struct ubus_context *ctx, struct ubus_object *obj,struct ubus_request_data *req, const char *method,struct blob_attr *msg );

//object KUEI
enum{
	KUEI_CNT,	//回傳被使用次數
	KUEI_ECHO,	//回復傳入的值
	__KUEI_MAX,
};

//指定msg 資料解析方式(policy)
static const struct blobmsg_policy cnt_policy[] =
{
	[KUEI_ECHO] = { .name="echo", .type=BLOBMSG_TYPE_STRING},
};

//指派handler處理 事件和解析message方式
static const struct ubus_method kuei_methods[] =
{
	UBUS_METHOD("KUEI_CNT", .handler=cnt_handler),
	UBUS_METHOD("KUEI_ECHO", .handler=echo_handler),
};

//ubus object type
static struct ubus_object_type kuei_obj_type = UBUS_OBJECT_TYPE("kuei_obj", kuei_methods);

static struct ubus_object kuei_obj=
{
	.name = "kuei_obj", 	
	.type = &kuei_obj_type,
	.methods = kuei_methods,
	.n_methods = __KUEI_MAX,
};


//收到method的處理方式 
static int cnt_handler( struct ubus_context *ctx, struct ubus_object *obj,
			  struct ubus_request_data *req, const char *method,
			  struct blob_attr *msg )
{
	struct blob_attr *tb[__KUEI_MAX]; //建立table儲存經過blob轉換後的method name//
 
	blobmsg_parse(kuei_policy, ARRAY_SIZE(kuei_policy), tb, blob_data(msg), blob_len(msg));	//剖析message內容
 
	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb,"Server Reply", kuei_counter);
	ubus_send_reply(ctx, req, bb.head);
 
	/* 	-----  reply results to the caller -----
	 * NOTE: we may put proceeding job in a timeout task, just to speed up service response.
	 */
	ubus_defer_request(ctx, req, &req_data);
	ubus_complete_deferred_request(ctx, req, UBUS_STATUS_OK);
	return 0;
}


static int echo_handler(struct ubus_context *ctx, struct ubus_object *obj,
			  struct ubus_request_data *req, const char *method,
			  struct blob_attr *msg)
{
	struct blob_attr *tb[__KUEI_MAX];
	blobmsg_parse(kuei_policy, ARRAY_SIZE(kuei_policy), tb, blob_data(msg), blob_len(msg));
	if(!tb[KUEI_ECHO])
		return UBUS_STATUS_INVALID_ARGUMENT;

	blobmsg_add_u32(&bb, "Server Say: ",blobmsg_get_u32(tb[KUEI_ECHO]));
	ubus_send_reply(ctx,req, bb.head);

	return 0;

}

void main(void){
	int ret;
	const char *ubus_socket=NULL;
	uloop_init();

	//建立ubus連線
	ctx=ubus_connect(ubus_socket);
	if(ctx==NULL){
		printf("Fail to connect to ubusd\n");
		return;
	}

	//註冊給uloop監控
	ubus_add_uloop(ctx);

	//註冊object給 ubus
	ret = ubus_add_object(ctx, &kuei_obj);
	if(ret!=0){
		printf("fail to register object.\n");
		goto UBUS_FAIL;
	}else{
		printf("ADD '%s' to ubus successfully!\n",kuei_obj.name);
	}

	//執行uloop
	uloop_run();
	uloop_done();
UBUS_FAIL:
	ubus_free(ctx);
}