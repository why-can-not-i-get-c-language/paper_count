# PaperCounter Firmware

## Target and Toolchain

- MCU: STM32F103C8T6 (medium-density STM32F1).
- Firmware library: STM32F10x Standard Peripheral Library v3.6.0.
- Build and flash tool: Keil uVision with ST-LINK.
- Do not replace the Standard Peripheral Library with HAL or another framework.

## Directory Ownership

- `User/`: main program, interrupt handlers, board-wide configuration.
- `System/`: common services such as SysTick delays.
- `Hardware/`: MCU peripheral and device drivers.
- `Application/`: paper-counting, calibration, and UI logic.
- `Memory/`: project architecture, decisions, responsibilities, and current progress records.
- `Library/` and `Start/`: copied vendor files. Do not edit them unless a change is explicitly required.

## Hardware Constraints

- PC13 is the onboard LED and is active-low on the usual STM32F103C8 board.
- PA13 and PA14 are reserved for ST-LINK SWD debugging.
- The NE555 output must be level-shifted to 3.3 V before it reaches an STM32 pin.

## Coding Rules

- Use C compatible with the Keil STM32F1 project.
- Write code comments in Chinese.
- Keep each hardware driver in a matching `.c` and `.h` pair.
- Include the required Standard Peripheral Library headers explicitly in each driver.
- Keep `main.c` limited to initialization and high-level task scheduling.
- Do not add a source file to a design unless its `.c` file is also added to the relevant Keil group.

## Verification

- After each change, review the changed files for missing declarations and dependencies.
- The user runs Keil Build (F7) and hardware tests; report the exact errors or observed behavior before continuing.
- Treat a feature as complete only after it compiles and its expected hardware behavior is verified.

## Git Workflow

- After each verifiable small task, run the relevant test or record why it cannot be run.
- Review `git diff` before staging files.
- Stage only files directly related to the current task.
- Update the relevant `Memory/` records before creating a commit.
- Create a clear local commit after the task is accepted.
- Do not push to a remote unless the user explicitly requests it.
