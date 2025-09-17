# Repository Guidelines

## Project Structure & Module Organization

  - Source lives in `src/` with gameplay objects such as `Ball`, `Paddle`, and `SceneManager`.
  - Shared headers (for example `pch.h`) are compiled as a precompiled header.
  - Runtime assets (icons, particle textures, desktop file) live in `res/`.
  - Prebuilt binaries and packaging artifacts are tracked in `bin/`, including the desktop AppImage and the `wasm/` output for the web target.
  - Build scripts and configuration live at the repository root (`CMakeLists.txt`, `Dockerfile`, `docker-compose.yml`, `make.sh`).

## Build, Test, and Development Commands

To generate both the AppImage and WebAssembly bundle:

```bash
docker compose build
docker compose run --rm glpong
```

Run the packaged desktop build via `./bin/GLPong-x86_64.AppImage`.

To generate both the AppImage and WebAssembly, and then start the Emscripten (WebAssembly) web server
(open http://localhost:8080/glpong.html in browser once started):

```bash
docker compose build
docker compose run --rm --service-ports glpong ./make.sh --emrun
```

Alternatively configure and compile natively with `cmake -S . -B build` followed by `cmake --build build` to produce `build/glpong`.
Use `./make.sh` to generate both the AppImage and WebAssembly bundle; append `--emrun` to launch the wasm build for local testing.

## Coding Style & Naming Conventions

  - Use Google C++ style:
      - Two-space indentation, braces on the same line, and standard library includes grouped after headers.
      - Classes use `PascalCase`, member functions favor verbs (`Update`, `Render`).
      - Member variables end with a trailing underscore (`ball_speed_`).
  - Keep headers self-sufficient and prefer `#pragma once`.
  - Run `clang-format` with the default LLVM style when in doubt.
  - Avoid introducing unused globals or magic numbers—promote them to `constexpr` instead.

## Testing Guidelines

There is no automated test suite; prioritize manual verification. After each change, run the AppImage and exercise paddle controls, scoring, and particle effects.
For wasm-specific changes, regenerate with `./make.sh --emrun` and confirm touch controls and ESC handling in the browser shell. Document any uncovered regressions in the PR description.

## Commit & Pull Request Guidelines

  - Each PR should explain the problem, summarize the fix.
  - Link related issues when available, and attach screenshots or short clips when UI or visual effects change.
  - Note the manual test matrix you executed so reviewers can replicate the steps.
