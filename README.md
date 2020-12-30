# Minimal ESP32 SDK

## Description

This project implements a bare-metal ESP32 firmware that is loaded as second-stage bootloader (but it isn't) from the ESP32 first-stage bootloader stored in its mask-ROM.

## Startup Execution Flow

As described in [ESP-IDF docs](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/general-notes.html). 

When the ESP32 powers on, it executes the first-stage bootloader stored in its mask-ROM (unchangable, factory-programmed). 

Once it finishes basic execution and hardware init, it attempts to load the second-stage bootloader from flash offset 0x1000. It loads it into IRAM and then executes it. 

The linker script provides the function `call_start_cpu0()` as the entry point. 

After we've been loaded by the first-stage bootloader, we can do anything we want. Doesn't have to be loading another application.

## Current State 

The current code and PlatformIO setup flashes the application to the correct offset, so that the first-stage bootloader will load it.

The firmware then does some initialization of the BSS, clear some instruction cache stuff and then use some functions provided to us in mask-ROM to do a `ets_printf()`. 

Further, we must periodically clear the watchdogs, that is, the RTC Watchdog and the Timer group 0, timer 0 watchdog (responsible for CPU0).

Otherwise it just sits in a loop and prints `Loop!`. 

## Progress and Goals 

* [x] create image that is bootable by the first-stage bootloader
* [x] print startup message using `printf()`-style function
* [x] reset watchdogs properly so that firmware runs forever
* [ ] toggle LED using GPIO registers only
* [ ] relocate interrupt vector (this is what [sdk](https://github.com/espressif/esp-idf/blob/b0150615dff529662772a60dcb57d5b559f480e2/components/esp_system/port/cpu_start.c#L134) does, too, without this we can't have user-controlled ISRs )
* [ ] use timer registers and ISR 
* [ ] implement drivers? 
* [ ] start up second CPU? 

## Issues / Quirks