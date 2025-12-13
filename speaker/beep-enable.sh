#!/bin/bash

# Watchguard Firebox M270
echo 1 >/sys/devices/platform/nct6775.2576/hwmon/hwmon2/beep_enable

# app-misc/beep
beep

# TODO: still no sound!

