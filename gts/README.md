# ngx gts 模块

## 参考  [Nginx 模块开发入门](http://blog.codinglabs.org/articles/intro-of-nginx-module-development.html) 了解基本概念

## 基础

做一个基础的tcp stream服务器，需要知道
1. 如何接收新连接
2. 如何接收C端过来的数据
3. 如何发送数据给C端
4.(more) 定时器或者类似 loop的机制 (TODO)

## 配置

	stream {
		server{
			listen 2014;
			cfg "hello.txt";
		}
	}

gts模块是在stream里的server模块，定义 ngx_stream_gts_srv_conf_t 结构体。

## ngx_stream_gts_create_srv_conf，ngx_stream_gts_merge_srv_conf

创建，合并上面的配置。

## ngx_stream_gts_on

读取配置里的cfg 具体信息，hello.txt。并指定接收新连接的函数。

cscf->handler = ngx_stream_gts_init_session;  
绑定初始化session，一个连接进来

## ngx_stream_gts_commands

配置支持 cfg 命令，关联ngx_stream_gts_on函数。

## ngx_stream_gts_module_ctx

关联 ngx_stream_gts_create_srv_conf 和 ngx_stream_gts_merge_srv_conf

## ngx_stream_gts_module

关联 ngx_stream_gts_module_ctx 和 ngx_stream_gts_commands

## 接收新连接 session

该 session 进来的消息，指定读取函数  
c->read->handler = ngx_stream_gts_read_handler;

同时设置

	ctx = ngx_stream_gts_create_ctx(s);  
	ngx_stream_set_ctx(s, ctx, ngx_stream_gts_module);

用于该session的上下文  

监听读时间，触发ngx_stream_gts_read_handler

	ngx_handle_read_event(c->read, 0)

## ngx_stream_gts_read_handler

创建缓冲区

	c->buffer = ngx_create_temp_buf(c->pool, cscf->preread_buffer_size);

计算剩余BUF大小，并进行读取

	int size = c->buffer->end - c->buffer->last;
	int n = c->recv(c, c->buffer->last, size);

## ngx_stream_gts_write_buffer

写数据给客户端

生成out链

	out = ngx_chain_get_free_buf(c->pool, &ctx->free);

指定数据内容

	out->buf->pos = str.data;
	out->buf->last = out->buf->pos + str.len;

写数据

	rc = ngx_chain_writer(&ctx->writer, out);

