# Repository Guidelines

## Project Structure

This repository contains the Electronic Load project. Keep changes in the existing project layout and preserve the roles of its native design, firmware, documentation, and generated files. Before adding a new directory, inspect the surrounding files and follow their established organization. Do not commit temporary exports, build output, or local machine configuration unless the project explicitly tracks them.

## Build, Test, and Development

Use the build and validation commands documented by the project itself (for example, its README, build scripts, or CI configuration). Run the narrowest relevant checks after a change and report the exact commands and results. Do not claim a hardware design or firmware build is validated when only a partial check was run.

## Style and Naming

Match the formatting and naming already used in neighboring files. Use descriptive, consistent names for source files and assets; keep documentation in Markdown and use relative paths for repository links. Make focused edits and avoid reformatting unrelated files.

## Testing and Design Checks

Add or update tests alongside the code they cover, following existing test naming and framework conventions. For hardware changes, inspect the native design files and run the project's available electrical, rule, or manufacturing checks where relevant. Treat generated outputs as derived evidence, not replacements for editable source files.

## Commits and Pull Requests

Use concise commit subjects that describe one change; follow the conventions visible in recent history when available. A pull request should explain the purpose, summarize affected files or subsystems, list checks performed and their results, and include screenshots or rendered views for visual changes. Link related issues when applicable.

## Configuration and Safety

Keep credentials, personal paths, and machine-specific settings out of tracked files. Preserve existing project data and configuration; ask before changing electrical limits, safety behavior, or other design assumptions that affect hardware operation.
