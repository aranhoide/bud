{
  "targets": [{
    "target_name": "bud",
    "type": "executable",
    "dependencies": [
      "deps/openssl/openssl.gyp:openssl",
      "deps/uv/uv.gyp:libuv",
      "deps/ringbuffer/ringbuffer.gyp:ringbuffer",
      "deps/parson/parson.gyp:parson",
      "deps/http_parser/http_parser.gyp:http_parser",
    ],
    "include_dirs": [
      "src",
    ],
    "sources": [
      "src/bio.c",
      "src/bud.c",
      "src/client.c",
      "src/common.c",
      "src/config.c",
      "src/error.c",
      "src/hello-parser.c",
      "src/http-pool.c",
      "src/logger.c",
      "src/master.c",
      "src/ocsp.c",
      "src/server.c",
      "src/sni.c",
      "src/worker.c",
    ],
  }]
}
