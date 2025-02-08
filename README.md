# Endstone C++ Example Plugin

Welcome to the example C++ plugin for Endstone servers.

## Prerequisites

### Windows

- Visual Studio 2017 or newer

### Linux

- Clang 5 or higher
- libc++ installed

## Structure Overview

```
cpp-example-plugin/
├── include/                      # Header files for the plugin
├── src/                          # Source files for the plugin
│   ├── example_listener.h        # Header for the ExampleListener class
│   ├── fibonacci_command.h       # Header for the FibonacciCommand class
│   ├── example_plugin.h          # Header for the ExamplePlugin class
│   └── example_plugin.cpp        # Source and metadata for the plugin
├── .clang-format                 # Configuration for Clang format rules
├── .clang-tidy                   # Configuration for Clang tidy rules
├── .clangd                       # Configuration for Clangd language server
├── .gitignore                    # Git ignore rules
├── xmake.lua                     # xmake configuration for building the plugin
├── LICENSE                       # License details
└── README.md                     # This file
```

## Getting Started

1. **Clone this Repository**

   ```bash
   git clone https://github.com/EndstoneMC/cpp-example-plugin.git
   ```

2. **Navigate to the Cloned Directory**

   ```bash
   cd cpp-example-plugin
   ```

3. **Build Your Plugin**

   ```bash
   xmake
   ```

   This process will compile your code and produce a shared library that Endstone servers can load.

   **Note: If you wish to build against a specific version of Endstone, you can modify the `add_requires` in
   the `xmake.lua` file to point to the desired tag or commit.**

4. **Test Your Plugin**

   Once built, copy the output binary `endstone_example_plugin.dll` (Windows) or `endstone_example_plugin.so` (Linux) to
   the `plugins` directory of your Endstone server. Start the Endstone server and check the logs to ensure your plugin
   loads and operates as expected.

## Documentation

For a deeper dive into the Endstone API and its functionalities, refer to the main
Endstone [documentation](https://endstone.readthedocs.io) (WIP).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.