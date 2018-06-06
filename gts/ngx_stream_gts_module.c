
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>
#include <nginx.h>

typedef struct {
	ngx_str_t  ed;
} ngx_stream_gts_srv_conf_t;

static void *ngx_stream_gts_create_srv_conf(ngx_conf_t *cf);
static char *ngx_stream_gts_merge_srv_conf(ngx_conf_t *cf, void *parent, void *child);
static char *ngx_stream_gts_on(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void ngx_stream_gts_init_session(ngx_stream_session_t *s);

static ngx_command_t  ngx_stream_gts_commands[] = {	
	{
		ngx_string("server"),
		NGX_STREAM_SRV_CONF|NGX_CONF_ANY,
		ngx_stream_gts_on,
		NGX_STREAM_SRV_CONF_OFFSET,
		0,
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

	ngx_log_stderr(0, "create srv conf");
	//conf->ed = ""
	return conf;
}

static char *
ngx_stream_gts_merge_srv_conf(ngx_conf_t *cf, void *parent, void *child)
{
	//ngx_stream_gts_srv_conf_t *prev = (ngx_stream_gts_srv_conf_t*)parent;
	//ngx_stream_gts_srv_conf_t *conf = (ngx_stream_gts_srv_conf_t*)child;

	return NGX_CONF_OK;
}


static void
ngx_stream_gts_init_session(ngx_stream_session_t *s)
{
	ngx_connection_t                *c;
	ngx_stream_gts_srv_conf_t     *conf;

	c = s->connection;

	conf = (ngx_stream_gts_srv_conf_t*)ngx_stream_get_module_srv_conf(s, ngx_stream_gts_module);

	ngx_log_stderr(0, "hello world");	
}
