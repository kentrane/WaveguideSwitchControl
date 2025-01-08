
# Waveguide Switch Controller

Arduino-based controller for managing a two-position waveguide switch with position feedback and command input.

## Hardware Requirements
- Arduino board (Uno/Nano compatible)
- DC motor with H-bridge driver
- Position feedback sensors (2x)
- Command input switch
- Power supply

## Pin Configuration
- Direction Control: Pin 13
- PWM Motor Control: Pin 11
- Brake Control: Pin 8
- Command Input: Pin 2
- Position Indicators: Pins 9, 10
- Position Sensors: A0, A1

## Installation
1. Connect hardware according to pin configuration
2. Upload sketch to Arduino board
3. Verify position sensor alignment

## Operation
- System starts in UNKNOWN position
- Position changes triggered by command input
- Automatic movement between TOKAMAK and DUMP positions
- Position feedback via LED indicators

## Safety Features
- Motor timeout protection (2s)
- Debounced inputs
- Brake engagement on stop
- Position verification

## States
- POS_UNKNOWN (0)
- POS_TOKAMAK (1)
- POS_DUMP (2)

## Troubleshooting
- Timeout error indicates movement failure
- Check sensor alignment if position unclear
- Verify power supply if motor doesn't move
