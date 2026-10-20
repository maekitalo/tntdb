# Logging

Tntdb uses cxxtools for logging. Cxxtools provides more detailed documentation
of its logging system; this section covers the basic configuration required for
Tntdb.

The application should call `log_init(argc, argv)`, declared in
`<cxxtools/log.h>`, to initialize logging.

A simple configuration can be placed in `_appname_.properties`:

```properties
rootlogger=INFO
logger.tntdb=DEBUG
file=myapp.log
maxfilesize=2M
```

`rootlogger` specifies the default log level. Since the default is `INFO`, it
may be omitted, but specifying it explicitly can improve clarity.

`logger.tntdb=DEBUG` enables debug logging for Tntdb. Debug logging can also be
enabled for a more specific logger, for example:

```properties
logger.tntdb.connect=DEBUG
```

`file` specifies the output file. If it is omitted, logging is written to
`stderr`.

`maxfilesize` limits the size of the log file. When the configured size is
reached, the existing file is renamed to `myapp.log.0` and a new `myapp.log` is
created. With the default settings, this preserves up to approximately twice
`maxfilesize` before older logging data is overwritten.
