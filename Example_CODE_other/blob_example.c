// 宣告blob陣列格式
static const struct blobmsg_policy pol[] = {
	[FOO_MESSAGE] = {
		.name = "message",
		.type = BLOBMSG_TYPE_STRING,
	},
	[FOO_LIST] = {
		.name = "list",
		.type = BLOBMSG_TYPE_ARRAY,
	},
	[FOO_TESTDATA] = {
		.name = "testdata",
		.type = BLOBMSG_TYPE_TABLE,
	},
};

//解析後輸出
static void dump_message(struct blob_buf *buf)
{
	struct blob_attr *tb[ARRAY_SIZE(pol)];

	if (blobmsg_parse(pol, ARRAY_SIZE(pol), tb, blob_data(buf->head), blob_len(buf->head)) != 0) {
		fprintf(stderr, "Parse failed\n");
		return;
	}
	if (tb[FOO_MESSAGE])
		fprintf(stderr, "Message: %s\n", (char *) blobmsg_data(tb[FOO_MESSAGE]));

	if (tb[FOO_LIST]) {
		fprintf(stderr, "List: ");
		dump_table(blobmsg_data(tb[FOO_LIST]), blobmsg_data_len(tb[FOO_LIST]), 0, true);
	}
	if (tb[FOO_TESTDATA]) {
		fprintf(stderr, "Testdata: ");
		dump_table(blobmsg_data(tb[FOO_TESTDATA]), blobmsg_data_len(tb[FOO_TESTDATA]), 0, false);
	}
}


//生成blob
static void fill_message(struct blob_buf *buf)
{
	void *tbl;

	blobmsg_add_string(buf, "message", "Hello, world!");

	tbl = blobmsg_open_table(buf, "testdata");
	blobmsg_add_u32(buf, "hello", 1);
	blobmsg_add_string(buf, "world", "2");
	blobmsg_close_table(buf, tbl);

	tbl = blobmsg_open_array(buf, "list");
	blobmsg_add_u32(buf, NULL, 0);
	blobmsg_add_u32(buf, NULL, 1);
	blobmsg_add_u32(buf, NULL, 2);
	blobmsg_close_table(buf, tbl);
}


/////--------
int main(int argc, char **argv)
{
	static struct blob_buf buf;

	blobmsg_buf_init(&buf);
	fill_message(&buf);
	dump_message(&buf);	

	return 0;
}