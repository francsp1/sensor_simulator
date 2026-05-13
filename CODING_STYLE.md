# C Coding Style & Rules

This document defines the coding conventions for this project. All code must follow these rules.

---

## 1. Project Structure

```
/
├── inc/            # All header files
│   ├── srv/        # Server-side headers
│   ├── cli/        # Client-side headers
│   └── lib/        # Library/utility headers
├── src/            # All source files
│   ├── srv/        # Server-side implementations
│   ├── cli/        # Client-side implementations
│   └── lib/        # Library/utility implementations
├── obj/            # Compiled object files (mirrors src/ structure)
├── bin/            # Compiled binaries
└── docs/           # Documentation
```

Header and source directories mirror each other exactly. Every `.c` file has a corresponding `.h` file in the matching `inc/` subdirectory.

---

## 2. Module Rules

- Code is divided into **modules**. Each module is a `.c`/`.h` pair.
- Modules must **not call functions from other modules**. Any functionality shared between modules must be placed in `common.c`/`common.h`.
- Modules may reference types (structs, enums) from other modules only when strictly necessary, but must not depend on their implementation.
- Internal helper functions that are not part of the public API must be `static` and must only appear in the `.c` file.

---

## 3. Header Files (`.h`)

Each `.h` file must contain:
- Include guard (see below)
- File-level Doxygen block
- `#include` directives for all types used in the public interface
- All `typedef`, `struct`, and `enum` definitions
- All `#define` macros and constants
- Doxygen-documented declarations of all **public** functions, structs and enum
- **No function implementations**
- **No `static` function declarations**

### Include Guards

```c
#ifndef _MODULE_NAME_H_
#define _MODULE_NAME_H_

// ... content ...

#endif // _MODULE_NAME_H_
```

The guard name must be `_FILENAME_H_` in uppercase with underscores. The closing `#endif` must include a comment matching the guard name.

---

## 4. Source Files (`.c`)

Each `.c` file must contain:
- File-level Doxygen block
- `#include` directives (system headers first, then local headers)
- Forward declarations of `static` helper functions with their Doxygen block
- Implementations of all functions (public and static)
- A path comment at the end of the file: `// Path: src/module/file.c`

### Include Order

1. Standard/system headers (`<stdio.h>`, `<stdlib.h>`, etc.)
2. Local project headers (`"module.h"`, `"common.h"`)

---

## 5. Naming Conventions

| Category | Convention | Examples |
|---|---|---|
| Functions (public) | `snake_case` | `init_server_socket`, `receive_from_socket` |
| Functions (static/internal) | `_snake_case` (leading underscore) | `_destroy_n_queues`, `_open_logs_files` |
| Struct typedefs | `snake_case_t` | `server_thread_params_t`, `logs_file_t` |
| Enum typedefs | `snake_case_e` | `server_socket_status_e`, `client_socket_status_e` |
| Macros / constants | `UPPER_CASE` | `NUMBER_OF_SENSORS`, `MAX_BUFFER_SIZE` |
| Enum values | `UPPER_CASE` | `SERVER_SOCKET_SUCCESS`, `INIT_SERVER_SOCKET_BIND_ERROR` |
| Local variables | `snake_case` | `server_socket`, `bytes_read`, `sent_bytes` |
| Pointer parameters | `p_<name>` | `p_server_socket`, `p_client_socket` |
| Output parameters | `<name>_out` | `server_socket_out`, `client_socket_out` |

### Struct and Typedef Pattern

Always define structs together with their typedef. The struct tag and the typedef name must match (minus the `_t` suffix):

```c
typedef struct server_thread_params {
    uint32_t id;
    int server_socket;
} server_thread_params_t;
```

---

## 6. Error Handling

### Rule: every public function must return a module-specific status enum. Never return data directly — use output function parameters instead.

### Status Enum Definition

Each module defines one status enum in its `.h` file, named `<module>_status_e`:

```c
typedef enum server_socket_status {
    SERVER_SOCKET_SUCCESS = 0,

    INIT_SERVER_SOCKET_SOCKET_ERROR      = -101,
    INIT_SERVER_SOCKET_REUSEADDR_ERROR   = -102,
    INIT_SERVER_SOCKET_RCVBUF_ERROR      = -103,
    INIT_SERVER_SOCKET_BIND_ERROR        = -104,
    INIT_SERVER_SOCKET_SETSOCKOPT_ERROR  = -105,

    RECEIVE_FROM_SOCKET_INTERUPTED_BY_SIGNAL = -201,
    RECEIVE_FROM_SOCKET_RECVFROM_ERROR       = -202,
    RECEIVE_FROM_SOCKET_EMPTY_PACKET         = -203,
    RECEIVE_FROM_SOCKET_TRUNCATED_PACKET     = -204,

    DESERIALIZE_SENSOR_DATA_NULL_POINTER = -301,
} server_socket_status_e;
```

### Rules for Status Enums

- **Success is always `0`**.
- **Errors are always negative**.
- Error codes are grouped by function in blocks of 100 (blocks can be bigger if needed):
  - First function: `-101`, `-102`, `-103`, ...
  - Second function: `-201`, `-202`, `-203`, ...
  - Third function: `-301`, `-302`, ...
- Error value names follow the pattern: `<FUNCTION_NAME>_<ERROR_DESCRIPTION>` in `UPPER_CASE`.
- The success value name follows the pattern: `<MODULE>_SUCCESS`.

### Checking Return Values

Always compare against the success/error constant, not against zero or `-1`:

```c
// Correct
if (init_server_socket(port, &fd) != SERVER_SOCKET_SUCCESS) { ... }

// Wrong
if (init_server_socket(port, &fd) == -1) { ... }
if (init_server_socket(port, &fd) < 0) { ... }
```

### Output Parameters

Return data through pointer parameters suffixed with `_out`:

```c
server_socket_status_e init_server_socket(int port, int *p_server_socket_out);
```

---

## 7. Doxygen Documentation

All documentation lives in the `.h` file. The `.c` file has only the file-level block.

### File Header (both `.h` and `.c`)

```c
/**
 * @file filename.h
 * @brief One-line description of what this file provides.
 *
 * Longer description of the module's purpose and responsibilities.
 *
 * @date DD/MM/YYYY
 * @authors First Last
 * @path inc/srv/filename.h
 */
```

### Enum

```c
/**
 * This enum defines the status codes returned by the <module> functions.
 * @brief Status codes for the <module> module.
 * @param VALUE_1 Description.
 * @param VALUE_2 Description.
 * @typedef module_status_e
 */
typedef enum module_status { ... } module_status_e;
```

### Struct

```c
/**
 * This structure contains the parameters passed to the server threads.
 * @brief Server thread parameters.
 * @param id Thread ID.
 * @param server_socket Server socket file descriptor.
 * @struct server_thread_params
 * @typedef server_thread_params_t
 */
typedef struct server_thread_params { ... } server_thread_params_t;
```

### Function

```c
/**
 * This function initializes the server socket.
 * @brief Initialize the server socket.
 * @param port Port number to bind to.
 * @param p_server_socket_out Pointer to the socket file descriptor to be initialized.
 * @return server_socket_status_e value indicating the result of the operation.
 */
server_socket_status_e init_server_socket(int port, int *p_server_socket_out);
```

Required tags: `@brief`, `@param` (one per parameter), `@return`.
Optional tags: `@note` for important caveats.

---

## 8. Memory Management

- **Prefer `calloc`** over `malloc` to ensure zero-initialization.
- **Set pointer to NULL** on declaration
- **Set pointers to `NULL` after `free`**:
  ```c
  free(ptr);
  ptr = NULL;
  ```
- **Check before freeing**:
  ```c
  if (ptr != NULL) {
      free(ptr);
      ptr = NULL;
  }
  ```
- When a function allocates memory that the caller must free, document it explicitly in the `@note` tag.
- **Zero-initialize structs** with `memset` before use, especially for socket address structures:
  ```c
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  ```
- **Partial cleanup on error**: when allocating N resources in a loop and allocation fails at index `i`, clean up only the resources already allocated (indices `0` to `i-1`). Use a private helper for this:
  ```c
  if (queues[i] == NULL) {
      _destroy_n_queues(queues, i);
      return CREATE_SERVER_QUEUES_ERROR;
  }
  ```

---

## 9. Functions

### Static (Internal) Functions

- Prefix the name with `_`.
- Declare them as `static` at the top of the `.c` file with a Doxygen comment.
- Define them in the same `.c` file.
- They must not be declared in the `.h` file.

```c
// In .c — forward declaration
/**
 * @brief Destroy the first n queues in the array.
 * @param queues Pointer to the queue array.
 * @param n Number of queues to destroy.
 * @return server_queues_status_e value indicating the result.
 */
static server_queues_status_e _destroy_n_queues(queue_thread_safe_t **queues, uint32_t n);
```

### Error Paths and Cleanup

Every error path must clean up all resources acquired up to that point before returning. Cleanup must happen in reverse order of acquisition:

```c
server_queues_status_e create_server_queues(queue_thread_safe_t **queues) {
    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++) {
        queues[i] = queue_create_thread_safe();
        if (queues[i] == NULL) {
            _destroy_n_queues(queues, i); // clean up what was created
            return CREATE_SERVER_QUEUES_ERROR;
        }
    }
    return SERVER_QUEUES_SUCCESS;
}
```

### Network Byte Order

Always convert values explicitly when sending/receiving over the network:
- Host → Network: `htonl()`, `htons()`
- Network → Host: `ntohl()`, `ntohs()`

### Protocol Structs

Network protocol structs must use `__attribute__((packed))` and be verified with `_Static_assert`:

```c
typedef struct proto_hdr {
    proto_type_t type;
    uint32_t sensor_id;
    uint16_t len;
} __attribute__((packed)) proto_hdr_t;
_Static_assert(sizeof(proto_hdr_t) == 10, "proto_hdr_t must be 10 bytes");
```

---

## 10. Thread Safety

- Pass all thread parameters via a dedicated struct, never as raw `void *` cast from multiple values.
- Protect shared data with `pthread_mutex_t`.
- Use `pthread_cond_t` for blocking waits on a condition.
- Use `atomic_bool` (`<stdatomic.h>`) for inter-thread flags.
- Access atomics with explicit memory ordering: `atomic_load_explicit(..., memory_order_acquire)` / `atomic_store_explicit(..., memory_order_release)`.

---

## 11. Compiler Settings

The project compiles with `-std=c11` and the following warning flags (all warnings are errors):

```
-ggdb -std=c11 -Wall -Wextra -fno-short-enums -Wpedantic -pedantic
-pedantic-errors -Wmissing-declarations -Wmissing-include-dirs
-Wundef -Wfloat-equal -D_POSIX_C_SOURCE=200809L
```

All code must compile cleanly under these flags with zero warnings.

---

