
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>
#include <nginx.h>

typedef struct {
	ngx_str_t  cfg;
} ngx_stream_gts_srv_conf_t;

typedef struct{
    ngx_chain_writer_ctx_t  writer;
} ngx_stream_gts_ctx_t;

static void *ngx_stream_gts_create_srv_conf(ngx_conf_t *cf);
static char *ngx_stream_gts_merge_srv_conf(ngx_conf_t *cf, void *parent, void *child);
static char *ngx_stream_gts_on(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void ngx_stream_gts_init_session(ngx_stream_session_t *s);
static void ngx_stream_gts_read_handler(ngx_event_t *rev);
static void ngx_stream_gts_write_handler(ngx_event_t *rev);
static ngx_stream_gts_ctx_t *ngx_stream_gts_create_ctx(ngx_stream_session_t *s);

static ngx_command_t  ngx_stream_gts_commands[] = {	
	{
		ngx_string("cfg"),
		NGX_STREAM_MAIN_CONF|NGX_STREAM_SRV_CONF|NGX_CONF_FLAG,
		ngx_stream_gts_on,
		NGX_STREAM_SRV_CONF_OFFSET,
		offsetof(ngx_stream_gts_srv_conf_t, cfg),
		NULL
	},
	ngx_null_command
};

static char *
ngx_stream_gts_on(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_stream_core_srv_conf_t  *cscf;
	cscf = (ngx_stream_core_srv_conf_t*)ngx_stream_conf_get_module_srv_conf(cf, ngx_stream_core_module);
	cscf->handler = ngx_stream_gts_init_session;

	printf("load gts conf\n");
	ngx_conf_set_str_slot(cf, cmd, conf);

	ngx_stream_gts_srv_conf_t* gtsConf = (ngx_stream_gts_srv_conf_t*)conf;
	printf("gtsconf %s \n", gtsConf->cfg.data);
	
	return NGX_CONF_OK;
}

static ngx_stream_module_t  ngx_stream_gts_module_ctx = {
	NULL,                                  /* preconfiguration */
	NULL,                                  /* postconfiguration */

	NULL,                                  /* create main configuration */
	NULL,                                  /* init main configuration */

	ngx_stream_gts_create_srv_conf,      /* create server configuration */
	ngx_stream_gts_merge_srv_conf        /* merge server configuration */
};

ngx_module_t  ngx_stream_gts_module = {
    NGX_MODULE_V1,
    &ngx_stream_gts_module_ctx,          /* module context */
    ngx_stream_gts_commands,             /* module directives */
    NGX_STREAM_MODULE,                     /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static void *
ngx_stream_gts_create_srv_conf(ngx_conf_t *cf)
{
	ngx_stream_gts_srv_conf_t  *conf;

	conf = (ngx_stream_gts_srv_conf_t*)ngx_pcalloc(cf->pool, sizeof(ngx_stream_gts_srv_conf_t));
	if (conf == NULL) {
		return NULL;
	}

	//ngx_log_stderr(0, "create srv conf");
	printf("gts create conf\n");
	conf->cfg.len = 0;
	conf->cfg.data = NULL;
	return conf;
}

static char *
ngx_stream_gts_merge_srv_conf(ngx_conf_t *cf, void *parent, void *child)
{
	ngx_stream_gts_srv_conf_t *prev = (ngx_stream_gts_srv_conf_t*)parent;
	ngx_stream_gts_srv_conf_t *conf = (ngx_stream_gts_srv_conf_t*)child;

	printf("gts merge srv conf\n");
	ngx_conf_merge_str_value(conf->cfg, prev->cfg, "");

	printf("after merge %s \n", conf->cfg.data);
	return NGX_CONF_OK;
}


static void
ngx_stream_gts_init_session(ngx_stream_session_t *s)
{
	ngx_connection_t                *c;
	ngx_stream_gts_srv_conf_t     *conf;
    ngx_stream_gts_ctx_t          *ctx;
    
	c = s->connection;

	conf = (ngx_stream_gts_srv_conf_t*)ngx_stream_get_module_srv_conf(s, ngx_stream_gts_module);

	//ngx_log_stderr(0, "hello world");
	printf("hello world\n");
	printf("init sessioin\n");

	c->write->handler = ngx_stream_gts_write_handler;
	c->read->handler = ngx_stream_gts_read_handler;

    ctx = ngx_stream_gts_create_ctx(s);
    ngx_stream_set_ctx(s, ctx, ngx_stream_gts_module);
    
	if (ngx_handle_read_event(c->read, 0) != NGX_OK)
	{
		printf("handle read event error\n");
		ngx_close_connection(c);
	}
	//
	//if (ngx_handle_write_event(c->write, 0) != NGX_OK) {
	//	printf("handle write event error\n");
	//	ngx_close_connection(c);
	//}
}

static void ngx_stream_gts_read_handler(ngx_event_t *rev)
{
    ngx_int_t rc;
    
	ngx_connection_t* c = rev->data;
	ngx_stream_core_srv_conf_t  *cscf;
	ngx_stream_session_t *s = c->data;
	cscf = ngx_stream_get_module_srv_conf(s, ngx_stream_core_module);
	if (c->buffer == NULL) {
		c->buffer = ngx_create_temp_buf(c->pool, cscf->preread_buffer_size);
	}
	
	int size = c->buffer->end - c->buffer->last;
	int n = c->recv(c, c->buffer->last, size);
	
	printf("read handler %d recv %d\n", size, n);
    
    //do the echo service
    // ngx_copy the buffer
    //rc = ngx_stream_gts_write_buffer(s, );
    printf("write buffer %d\n", rc);
}


static void
ngx_stream_gts_write_handler(ngx_event_t *wev)
{
	printf("gts write handler\n");
}

static ngx_int_t
ngx_stream_gts_write_buffer(ngx_stream_session_t *s, ngx_str_t str)
{
    ngx_int_t            rc;
    ngx_chain_t         *out;
    ngx_connection_t    *c;
    ngx_stream_gts_ctx_t*ctx;
    
    c = s->connection;
    
    ctx = ngx_stream_get_module_ctx(s, ngx_stream_gts_module);
    
    out = ngx_chain_get_free_buf(c->pool, NULL);
    if (out == NULL) {
        return NGX_ERROR;
    }
    
    out->buf->memory = 1;
    
    out->buf->pos = str.data;
    out->buf->last = out->buf->pos + str.len;
    
    out->buf->tag = (ngx_buf_tag_t) &ngx_stream_gts_module;
    
    rc = ngx_chain_writer(&ctx->writer, out);
        
    //ngx_chain_update_chains(c->pool, &ctx->free, &ctx->busy, &out,
    //                        (ngx_buf_tag_t) &ngx_stream_echo_module);
    
    return rc;

}

static ngx_stream_gts_ctx_t *ngx_stream_gts_create_ctx(ngx_stream_session_t *s)
{
    ngx_connection_t            *c;
    ngx_stream_gts_ctx_t       *ctx;
    
    c = s->connection;
    
    ctx = ngx_pcalloc(c->pool, sizeof(ngx_stream_gts_ctx_t));
    if (ctx == NULL) {
        return NULL;
    }
    
    ctx->writer.pool = c->pool;
    ctx->writer.last = &ctx->writer.out;
    ctx->writer.connection = c;
    
    return ctx;
}
