# Barectf Lttng Lib

A library that uses barectf to generate trace similar to that lttng generates. The purpose is to allow using Trace Compass with baremetal targets(where lttng isn't available)

To run the test:

```shell
cd <path-to-barectf_lttng_lib-folder>

# install dependency, should only need to be run once
bash setup.sh

bash run_test.sh
```
