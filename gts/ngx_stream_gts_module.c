
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>
#include <nginx.h>

typedef struct {
	ngx_str_t  ed;
} ngx_stream_gts_conf_t;

static void *ngx_stream_gts_create_svr_conf(ngx_conf_t *cf);
static char *ngx_stream_gts_merge_svr_conf(ngx_conf_t *cf, void *parent, void *child);
