# PellOS

## TODO
- [ ] filesystem support
  - [ ] Basic initrd (in progress)
    - [x] basic read functionality
    - [x] image generation scripts
    - [ ] loader for images
  - [x] reading from files in kernel
  - [x] syscall support to read files from userspace
  - [ ] `close()` syscall
  - [ ] access devices through fs
    - [x] initial vfs support
    - [ ] devfs implementation
- [ ] executable loading
  - [ ] kernel module loading
  - [ ] userspace executable loading
    - [x] static executables [elf loading][#1]
    - [ ] dynamic linking
    - [ ] userspace library
- [ ] kmalloc cleanup
  - [ ] look into alignment
  - [ ] clean up logic (it's messy in there)
- [ ] Basic security
  - [ ] Users


## Current issues
* framebuffer not working with mmu enabled
* printf long support
* support better qemu builds
  * use different timer for qemu (for IRQs)
  * investigate framebuffer support
* be more graceful with kernel tasks returning (currently crashes)

## Open Source
* printf - [Kustaa Nyholm](http://www.sparetimelabs.com/tinyprintf/tinyprintf.php)