# PellOS

## TODO
- [] Basic initrd (`in progress`)
- [] executable loading
  - [] kernel module loading
  - [] userspace executable loading
    - [] userspace library


## Current issues
* framebuffer not working with mmu enabled
* printf long support
* support better qemu builds
  * use different timer for qemu (for IRQs)
  * investigate framebuffer support
* be more graceful with kernel tasks returning (currently crashes)

## Open Source
* printf - [github.com/mpaland](https://github.com/mpaland/printf)