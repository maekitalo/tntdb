# Logging

Tntdb implements logging using cxxtools. Cxxtools has more in depth documentation on how it
is used. Here is just a brief howto for a simple case.

The application should call `log_init(argc, argv)`, which is defined in the
header `<cxxtools/log.h>`. That is all what needs to be done to enable
logging in the application.

Easiest way is the to create a file '_appname_.properties' with the configuration.
A typical file will look like this:

```
    rootlogger=INFO
    logger.tntdb=DEBUG
    file=myapp.log
    maxfilesize=2M
```

Rootlogger specifies the default log level. Since the default is INFO, it can
also be ommitted but is specified here for clarity.

The setting 'logger.tntdb' enables debug log for tntdb. Alternatively it is
possible to enable DEBUG only for specific classes e.g.
'logger.tntdb.connect=DEBUG'.

The file setting specifies the file name where the log is written to. Without
it, the output is written to stderr.

'maxfilesize' limits the file size. If the file size is reached, the original file is
renamed to myapp.log.0 and a new log file is created. This means that logs up to 2 times
'maxfilesize' can be preserved before logging data is lost with default settings.
