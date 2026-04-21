# Contributing to Steam Minimal C++

## Scope
This project focuses on:
- C++20 library development
- Networking / crypto / protocol implementation
- Cross-platform compatibility
- Stable API design suitable for package managers

## Branching model

- `main` → stable development branch
- feature branches → `feat/*`
- bugfix branches → `fix/*`
- release tags → `vX.Y.Z`

## Versioning

This project follows Semantic Versioning:

- MAJOR: breaking API changes
- MINOR: backward-compatible features
- PATCH: bug fixes

Version is defined in:
```cmake
project(SteamClient VERSION X.Y.Z)
```
and propagated automatically into generated headers.

## Pull requests

PRs should:

* Be focused (one concern per PR)
* Avoid unrelated refactors
* Prefer clarity over cleverness
* Have structured commit messages

No strict format, but prefer:

* clear intent
* avoiding vague messages like "fix stuff"

Example:

* `Fix AES IV generation edge case`
* `Refactor protobuf message handling`

## What TODO?

Please refer to [the TODO list](TODO.md) to see changes you can easily make.
This project uses a `command` and `event` model. In the `src/routing` directory,
you can add in new commands by following the format of the other handlers.
Proper addition of new handlers is always appreciated, so long as it slots into the
existing architecture cleanly, avoiding "quick hacks".

Despite all of this, *any* change that is clearly helpful for:
* Expansion of the library
* Code-correctness
* Documentation fixes / additions
* Overall structure

Is *greatly* appreciated. Thank you for reading, and have fun coding! :)