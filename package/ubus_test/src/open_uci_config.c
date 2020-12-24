#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <uci.h>

const char *b1 = "interface";
const char *b2 = "start";
const char *b3 = "limit";


int getvalue(struct uci_context *ctx, const char *key){
	char strkey[100];
	struct uci_ptr ptr;
	snprintf(strkey, sizeof(strkey), "dhcp.lan.%s",key);

	if(uci_lookup_ptr(ctx, &ptr, strkey, true)==UCI_OK){
		printf("dhcp.lan.%s: %s\n",key,ptr.o->v.string);
	}
	return 0;
}

int main(){
	struct uci_context *ctx = uci_alloc_context();
	if(!ctx){
		fprintf(stderr, "NO MEMORY!\n");
		return -1;
	}
	getvalue(ctx,b1);
	getvalue(ctx,b2);
	getvalue(ctx,b3);


//free space
	

	return 0;
}

/*
struct uci_context
{
    // 配置檔案包列表 
    struct uci_list root;

    // 解析上下文，只用於錯誤處理 
    struct uci_parse_context *pctx;

    // 後端匯入匯出 
    struct uci_backend *backend;
    struct uci_list backends;

    // uci 執行標識 
    enum uci_flags flags;

    char *confdir;
    char *savedir;

    // search path for delta files 
    struct uci_list delta_path;

    // 私有資料 
    int err;
    const char *func;
    jmp_buf trap;
    bool internal, nested;
    char *buf;
    int bufsz;
};
*/

/*
struct uci_ptr
{
    enum uci_type target;
    enum {
        UCI_LOOKUP_DONE =     (1 << 0),
        UCI_LOOKUP_COMPLETE = (1 << 1),
        UCI_LOOKUP_EXTENDED = (1 << 2),
    } flags;

    struct uci_package *p;
    struct uci_section *s;
    struct uci_option *o;
    struct uci_element *last;

    const char *package;
    const char *section;
    const char *option;
    const char *value;
};
*/