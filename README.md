# PellOS

## TODO
- [] Basic initrd (`in progress`)
  - [x] reading from files in kernel
  - [] syscall support to read files from userspace
- [] executable loading
  - [] kernel module loading
  - [] userspace executable loading
    - [] userspace library
- [] kmalloc cleanup
  - [] look into alignment
  - [] clean up logic (it's messy in there)


## Current issues
* framebuffer not working with mmu enabled
* printf long support
* support better qemu builds
  * use different timer for qemu (for IRQs)
  * investigate framebuffer support
* be more graceful with kernel tasks returning (currently crashes)

## Open Source
* printf - [github.com/mpaland](https://github.com/mpaland/printf)