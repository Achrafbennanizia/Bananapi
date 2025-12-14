# Doxygen Documentation

## 📚 Generated Documentation

The complete API documentation has been generated using Doxygen and is available in the `docs/doxygen/html` directory.

## 🌐 Viewing the Documentation

### Local Viewing

Open the documentation in your browser:

```bash
# macOS
open docs/doxygen/html/index.html

# Linux
xdg-open docs/doxygen/html/index.html

# Or use any browser
firefox docs/doxygen/html/index.html
chrome docs/doxygen/html/index.html
```

### Start Local Web Server

For better experience with search and navigation:

```bash
# Python 3
cd docs/doxygen/html
python3 -m http.server 8000

# Then open: http://localhost:8000
```

## 📖 What's Documented

### Complete API Coverage

- **All Classes** - Full class hierarchy with inheritance diagrams
- **All Interfaces** - IGpioController, INetworkCommunicator, ICpSignalReader
- **Design Patterns** - Strategy, Observer, Factory, Singleton, State, Command
- **Protocol Layer** - ISO 15118 stack implementation
- **CP Signal System** - IEC 61851-1 implementation
- **Network Communication** - UDP messaging protocol
- **State Machine** - Charging state transitions
- **Configuration** - JSON-based config system

### Documentation Features

✅ **Class Diagrams** - UML class relationships  
✅ **Collaboration Diagrams** - Object interactions  
✅ **Call Graphs** - Function call hierarchies  
✅ **Include Graphs** - Header dependencies  
✅ **Directory Structure** - File organization  
✅ **Search Functionality** - Quick navigation  
✅ **Source Code Browser** - Inline code viewing

## 🔄 Regenerating Documentation

### Update Documentation

```bash
# After code changes, regenerate:
cd /Users/achraf/pro/PJMT
doxygen Doxyfile
```

### Customize Doxyfile

Edit `Doxyfile` to change:

- Project name and version
- Input/output directories
- Documentation detail level
- Graph generation options
- HTML theme and styling

## 📋 Documentation Sections

### Main Page

- Project overview
- Quick start guide
- System architecture
- Feature list

### Modules

- **Core Components** - WallboxController, ChargingStateMachine
- **GPIO Abstraction** - Hardware interfaces and implementations
- **Network Layer** - UDP communication
- **CP Signal System** - Hardware and simulator readers
- **Protocol Layer** - ISO 15118 stack
- **API Layer** - HTTP REST API

### Classes

Alphabetical list of all classes with:

- Brief description
- Inheritance hierarchy
- Public/private members
- Detailed documentation
- Usage examples

### Files

Source code browser with:

- File dependencies
- Include graphs
- Function definitions
- Inline documentation

### Namespaces

- `Iso15118` - Protocol definitions
- Global namespace - Main components

## 🎨 Diagrams

### Available Diagrams

1. **Class Inheritance Diagrams**

   - Shows base classes and derived classes
   - Strategy pattern implementations
   - Interface hierarchies

2. **Collaboration Diagrams**

   - Object interactions
   - Message flow
   - Dependency injection

3. **Call Graphs**

   - Function call hierarchy
   - Entry points
   - Execution flow

4. **Include Dependency Graphs**
   - Header file dependencies
   - Compilation units
   - Module boundaries

## 🔍 Navigation Tips

### Quick Search

- Use the search box in top-right corner
- Search by class name, function, or keyword
- Autocomplete suggestions available

### Browse by Type

- **Classes** tab - All classes alphabetically
- **Files** tab - Source code structure
- **Namespaces** tab - Namespace organization

### View Source

- Click on any function/class name
- View implementation inline
- Jump to definition

## 📊 Statistics

**Project:** Wallbox Control System v4.1  
**Classes:** 25+ documented classes  
**Files:** 100+ source and header files  
**Functions:** 200+ documented functions  
**Lines:** 10,000+ lines of code

## 🔗 Quick Links

### Core Documentation

- [WallboxController](docs/doxygen/html/class_wallbox_controller.html)
- [ChargingStateMachine](docs/doxygen/html/class_charging_state_machine.html)
- [IGpioController](docs/doxygen/html/class_i_gpio_controller.html)
- [UdpCommunicator](docs/doxygen/html/class_udp_communicator.html)

### Design Patterns

- [Strategy Pattern](docs/doxygen/html/group__strategy__pattern.html)
- [Observer Pattern](docs/doxygen/html/group__observer__pattern.html)
- [Factory Pattern](docs/doxygen/html/group__factory__pattern.html)
- [State Pattern](docs/doxygen/html/group__state__pattern.html)

### Protocol Layer

- [ISO 15118 Stack](docs/doxygen/html/namespace_iso15118.html)
- [Message Structures](docs/doxygen/html/struct_iso_stack_msg.html)
- [Protocol Implementation](docs/doxygen/html/class_iso_stack_ctrl_protocol.html)

## 🛠️ Troubleshooting

### Documentation Not Generating

```bash
# Check Doxygen installation
doxygen --version

# Install on macOS
brew install doxygen graphviz

# Install on Linux
sudo apt-get install doxygen graphviz

# Verify Doxyfile syntax
doxygen -s -g Doxyfile.test
```

### Missing Graphs

Ensure Graphviz is installed:

```bash
# macOS
brew install graphviz

# Linux
sudo apt-get install graphviz

# Verify
dot -V
```

### Update Path in Doxyfile

If graphs not generating, check `HAVE_DOT` and `DOT_PATH` settings in Doxyfile.

## 📝 Contributing to Documentation

### Adding Documentation

Use Doxygen-style comments in source code:

```cpp
/**
 * @brief Brief description of the class
 *
 * Detailed description explaining the purpose,
 * design patterns used, and usage examples.
 *
 * @note Important notes or warnings
 * @see Related classes or functions
 */
class MyClass {
    /**
     * @brief Brief method description
     * @param param1 Description of parameter 1
     * @param param2 Description of parameter 2
     * @return Description of return value
     * @throws std::runtime_error When condition occurs
     */
    int myMethod(int param1, bool param2);
};
```

### Documentation Tags

Common Doxygen tags:

- `@brief` - Short description
- `@param` - Parameter documentation
- `@return` - Return value description
- `@throws` - Exception documentation
- `@note` - Important notes
- `@warning` - Warnings
- `@see` - Cross-references
- `@example` - Usage examples
- `@code` / `@endcode` - Code blocks

## 📚 Additional Resources

- [Doxygen Manual](https://www.doxygen.nl/manual/)
- [Doxygen Special Commands](https://www.doxygen.nl/manual/commands.html)
- [Graphviz Documentation](https://graphviz.org/documentation/)
- [Project README](../README.md)
- [Architecture Documentation](architecture/ARCHITECTURE.md)

---

**Generated:** December 14, 2025  
**Version:** 4.1  
**Doxygen:** 1.12.0  
**Format:** HTML with search engine
