# Slash Project

Code-focused Unreal Engine 5 action game project built in C++.

This repository keeps the gameplay code layer of the project and is intended for code review, learning, and internship portfolio use.

## Highlights

- Modular combat architecture built around a shared `BaseCharacter` class
- Clear separation between player logic (`SlashCharacter`) and AI enemy logic (`Enemy`)
- Interface-driven interactions for hit handling and item pickup
- Reusable attribute component for health, stamina, gold, and souls
- Weapon collision and box-trace based hit detection
- HUD and health bar support integrated from C++ into UE gameplay flow

## Tech Stack

- Unreal Engine 5
- C++
- UE Gameplay Framework
- Blueprint-integrated C++ architecture

## Repository Scope

This is a code-only snapshot of the project.

Included:

- `Source/`
- `Slash.uproject`

Excluded:

- large binary assets
- most UE content resources
- generated build/cache directories

## Main Code Areas

- `Source/Slash/Public/Characters`
- `Source/Slash/Private/Characters`
- `Source/Slash/Public/Enemy`
- `Source/Slash/Private/Enemy`
- `Source/Slash/Public/Items`
- `Source/Slash/Private/Items`
- `Source/Slash/Public/HUD`
- `Source/Slash/Private/HUD`
- `Source/Slash/Public/Components`
- `Source/Slash/Public/Interfaces`

## Notes

The project focuses on gameplay systems design rather than content packaging. The current repository is optimized to present C++ class design, combat flow, interaction patterns, and gameplay code structure clearly on GitHub.
