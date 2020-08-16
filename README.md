# xmrig-nonces-heatmap

This tool created for nonce distribution visualization using heatmap. All required data can be obtained directly from daemon by JSON RPC protocol without any external tool. Daemon API must implement `get_last_block_header` and `get_block_headers_range` methods. Maximum heatmap resolution is limited to 1 Gigapixel.

## Configuration

All configuration is done using JSON configuration file with default name `config.json`. Working directory can be overridden by the command line option `--data-dir` (or `-d`) and config name by `--config` (or `-c`). If no configuration file exists default one will be created for you.

### Options

* `"nonces_file"` name for file where nonce information will be stored, if not specified `nonces.json`. File format is a simple JSON array where the first element is nonce of block with height 0.
* `"offline"` use `true` to disable connection to daemon, this will work only if the nonces file already exists.
* `"daemon"` object with daemon options.
    * `"host"` hostname of daemon, by default `"127.0.0.1"` can be replaced by public node, for example `"node.xmrig.com"` or `"nodes.hashvault.pro"`.
    * `"port" port of daemon, by default `18081` (Monero mainnet).
    * `"tls"` use TLS, you must enable TLS support first on compile time by `-DWITH_TLS=ON`.
    * `"print_time" number of seconds to print sync status, by default `10`.
    * `"concurrency"`, `"max_range"`, `"timeout"` advanced connection options.
* `"heatmap"` object with heatmap options.
    * `"name"` file name of heatmap, by default `"heatmap.png"`. [Environment variables](https://xmrig.com/docs/miner/environment-variables) is supported and special variables `${WIDTH}`, `${HEIGHT}`, `${COUNT}`, `${START}`, `${END}`, `${MIN_NONCE}`, `${MAX_NONCE}`, `${RADIUS}`, `${X}`.
    * `"info_file"` file with information about heatmap, if not specified it will be the name of heatmap with `.json` extension.
    * `"x" blocks per pixel for X coordinate (horizontal), if not specified will be detected automatically.
    * `"height"` image height in pixels.
    * `"radius"` heatmap radius, best value depends of image size and nonces distribution.
    * `"start"`, `"end"`, `"min_nonce"`, `"max_nonce"` advanced heatmap options.

## Build from source

You can use instructions from the XMRig miner with following differences:

* OpenSSL disabled by default, use `-DWITH_TLS=ON` to enable it.
* Use `-DWITH_LIBPNG=ON` to enable libpng support for performance improvements.