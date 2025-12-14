# Contributing to Wallbox Control System

Thank you for your interest in contributing! This document provides guidelines for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [Submitting Changes](#submitting-changes)
- [Review Process](#review-process)

## Code of Conduct

### Our Pledge

We pledge to make participation in our project a harassment-free experience for everyone, regardless of age, body size, disability, ethnicity, gender identity, level of experience, nationality, personal appearance, race, religion, or sexual orientation.

### Our Standards

**Positive behavior includes:**

- Using welcoming and inclusive language
- Being respectful of differing viewpoints
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

**Unacceptable behavior includes:**

- Harassment, trolling, or derogatory comments
- Personal or political attacks
- Publishing others' private information
- Any conduct which could be considered inappropriate in a professional setting

### Enforcement

Project maintainers are responsible for clarifying standards and will take appropriate action in response to unacceptable behavior.

## Getting Started

### Prerequisites

- C++14 or later compiler
- CMake 3.10+
- Git
- Basic understanding of:
  - ISO 15118 protocol
  - IEC 61851-1 CP Signal
  - SOLID principles
  - Design patterns

### Fork and Clone

```bash
# Fork the repository on GitHub
# Then clone your fork
git clone https://github.com/YOUR_USERNAME/Bananapi.git
cd Bananapi

# Add upstream remote
git remote add upstream https://github.com/Achrafbennanizia/Bananapi.git
```

### Build and Test

```bash
# Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)

# Run tests
./scripts/test/run_all_tests.sh
```

## Development Workflow

### 1. Create a Branch

```bash
# Update your fork
git fetch upstream
git checkout main
git merge upstream/main

# Create feature branch
git checkout -b feature/your-feature-name
```

### Branch Naming

- `feature/` - New features
- `bugfix/` - Bug fixes
- `docs/` - Documentation changes
- `refactor/` - Code refactoring
- `test/` - Test additions/changes

### 2. Make Changes

- Write clean, readable code
- Follow SOLID principles
- Add tests for new functionality
- Update documentation
- Commit frequently with clear messages

### 3. Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only
- `style`: Formatting, missing semicolons, etc.
- `refactor`: Code change that neither fixes a bug nor adds a feature
- `test`: Adding missing tests
- `chore`: Changes to build process or tools

**Example:**

```
feat(api): Add endpoint for charging status

- Implement GET /api/charging/status
- Add unit tests for status endpoint
- Update API documentation

Closes #123
```

### 4. Stay Updated

```bash
# Regularly sync with upstream
git fetch upstream
git rebase upstream/main
```

## Coding Standards

### C++ Style Guide

**File Organization:**

```cpp
// 1. Header guard
#ifndef WALLBOX_MY_CLASS_H
#define WALLBOX_MY_CLASS_H

// 2. System includes
#include <memory>
#include <string>

// 3. Project includes
#include "IInterface.h"

// 4. Namespace
namespace Wallbox
{

// 5. Class declaration
class MyClass : public IInterface
{
public:
    // Public interface

private:
    // Private implementation
};

} // namespace Wallbox

#endif // WALLBOX_MY_CLASS_H
```

**Naming Conventions:**

- Classes: `PascalCase`
- Functions/Methods: `camelCase`
- Variables: `camelCase`
- Constants: `UPPER_CASE`
- Member variables: `m_camelCase`
- Private members: `m_` prefix

**Code Style:**

```cpp
// Good
class ChargingController
{
public:
    void startCharging(int power);

private:
    int m_maxPower;
    std::unique_ptr<IGpioController> m_gpio;
};

// Avoid
class charging_controller
{
public:
    void StartCharging(int Power);

private:
    int MaxPower;
    IGpioController* gpio;
};
```

### SOLID Principles

Every contribution should follow SOLID principles:

1. **Single Responsibility**: Each class has one reason to change
2. **Open/Closed**: Open for extension, closed for modification
3. **Liskov Substitution**: Derived classes must be substitutable
4. **Interface Segregation**: No client should depend on unused methods
5. **Dependency Inversion**: Depend on abstractions, not concretions

### Design Patterns

Use appropriate design patterns:

- **Strategy**: Platform-specific implementations (GPIO)
- **Factory**: Object creation
- **Observer**: Event notification
- **Command**: User actions
- **State**: Charging states
- **Dependency Injection**: Constructor injection preferred

## Testing

### Unit Tests

```cpp
// tests/unit/test_ChargingStateMachine.cpp
#include <gtest/gtest.h>
#include "ChargingStateMachine.h"

TEST(ChargingStateMachine, InitialState)
{
    ChargingStateMachine sm;
    EXPECT_EQ(sm.getCurrentState(), State::IDLE);
}

TEST(ChargingStateMachine, TransitionToReady)
{
    ChargingStateMachine sm;
    sm.transition(State::READY);
    EXPECT_EQ(sm.getCurrentState(), State::READY);
}
```

### Integration Tests

Add integration tests to `tests/integration/`:

```bash
./scripts/test/test_wallbox.sh
```

### Test Coverage

Aim for:

- Unit tests: 80%+ coverage
- Integration tests for all major features
- Edge cases and error handling

## Documentation

### Code Documentation

```cpp
/**
 * @brief Manages charging state transitions
 *
 * Implements the State pattern for ISO 15118 charging states.
 * Thread-safe for concurrent access.
 *
 * @see ISO 15118-2 Section 8.3.4
 */
class ChargingStateMachine
{
public:
    /**
     * @brief Transition to new charging state
     * @param newState Target state to transition to
     * @return true if transition successful, false otherwise
     * @throws std::invalid_argument if newState is invalid
     */
    bool transition(State newState);
};
```

### Markdown Documentation

- Use clear headings
- Include code examples
- Add diagrams where helpful
- Keep language simple and clear
- Update table of contents

### Commit Documentation Changes

Documentation changes should be in separate commits:

```bash
git add docs/
git commit -m "docs: Update API reference with new endpoints"
```

## Submitting Changes

### 1. Push to Your Fork

```bash
git push origin feature/your-feature-name
```

### 2. Create Pull Request

- Go to GitHub repository
- Click "New Pull Request"
- Select your feature branch
- Fill out PR template

### PR Template

```markdown
## Description

Brief description of changes

## Motivation

Why is this change necessary?

## Changes

- List of changes made
- One per line

## Testing

How was this tested?

## Checklist

- [ ] Code follows style guidelines
- [ ] Tests added/updated
- [ ] Documentation updated
- [ ] All tests pass
- [ ] No breaking changes (or documented)
```

### 3. Address Review Comments

- Be responsive to feedback
- Make requested changes promptly
- Ask questions if unclear
- Update PR with additional commits

## Review Process

### What We Look For

✅ **Code Quality**

- Follows coding standards
- Well-structured and readable
- Proper error handling
- No code smells

✅ **Functionality**

- Solves the problem
- No regressions
- Edge cases handled
- Performance considered

✅ **Tests**

- Adequate test coverage
- Tests pass consistently
- Clear test names
- Fast execution

✅ **Documentation**

- Code is documented
- User-facing docs updated
- CHANGELOG.md updated
- Migration guide if needed

### Review Timeline

- Initial review: Within 3 business days
- Follow-up reviews: Within 2 business days
- Final approval: When all requirements met

### After Approval

Once approved, your PR will be:

1. Squashed and merged (if multiple commits)
2. Added to CHANGELOG.md
3. Included in next release

## Getting Help

### Questions

- Open a [GitHub Discussion](https://github.com/Achrafbennanizia/Bananapi/discussions)
- Check existing issues
- Read documentation thoroughly first

### Report Bugs

Use the bug report template:

```markdown
**Describe the bug**
Clear description of what the bug is.

**To Reproduce**
Steps to reproduce:

1. Go to '...'
2. Click on '...'
3. See error

**Expected behavior**
What you expected to happen.

**Environment**

- OS: [e.g., Ubuntu 22.04]
- Compiler: [e.g., GCC 11.2]
- Version: [e.g., v4.1.0]

**Additional context**
Any other context about the problem.
```

### Feature Requests

Use the feature request template:

```markdown
**Is your feature request related to a problem?**
Clear description of the problem.

**Describe the solution you'd like**
Clear description of desired solution.

**Describe alternatives**
Alternative solutions considered.

**Additional context**
Any other context or screenshots.
```

## Recognition

Contributors will be:

- Listed in CONTRIBUTORS.md
- Credited in release notes
- Given credit in commit history

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (see LICENSE file).

---

**Thank you for contributing to make this project better!** 🚀

If you have questions about the contribution process, please open a discussion on GitHub.
