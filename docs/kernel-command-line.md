# Torus kernel command line

The Torus kernel accepts a command-line string provided by the bootloader through the Torus boot protocol.

The command line consists of whitespace-separated options. Unknown options are ignored.

## Boolean options

### debug

The `debug` option enables debug logs.

By default, logs with the debug log level are suppressed. When the option is present, debug logs are printed normally.
