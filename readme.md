> Writing an OS in ~1,000 lines.

## Features

- [ ] Multitasking: Switch between processes to allow multiple applications to share the CPU.
- [ ] Exception handler: Handle events requiring OS intervention, such as illegal instructions.
- [ ] Paging: Provide an isolated memory address space for each application.
- [ ] System calls: Allow applications to call kernel features.
- [ ] Device drivers: Abstract hardware functionalities, such as disk read/write.
- [ ] File system: Manage files on disk.
- [ ] Command-line shell: User interface for humans.

```sh
├── disk/     - File system contents
├── common.c  - Kernel/user common library: printf, memset, ...
├── common.h  - Kernel/user common library: definitions of structs and constants
├── kernel.c  - Kernel: process management, system calls, device drivers, file system
├── kernel.h  - Kernel: definitions of structs and constants
├── kernel.ld - Kernel: linker script (memory layout definition)
├── shell.c   - Command-line shell
├── user.c    - User library: functions for system calls
├── user.h    - User library: definitions of structs and constants
├── user.ld   - User: linker script (memory layout definition)
└── run.sh    - Build script
```
