add_subdirectory(${CHIP_FAMILY}_drivers)

sdk_append_inc(.)
sdk_append_source(RingBuffer/lwrb.c)
