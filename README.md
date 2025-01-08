
# Waveguide Switch Controller

Arduino controller for two-position waveguide switch with position feedback.

## Pins
- Direction: 13
- PWM Motor: 11
- Brake: 8
- Command: 2 (Input)
- Position outputs: 9, 10
- Position Sensors: A0, A1

## Features
- Position control (TOKAMAK/DUMP)
- Position feedback and verification
- 2s timeout protection for not overheating motor
- Test mode available
- Debounced inputs
- Status output for labview feedback to see state in control room

## Operation
1. Power up in UNKNOWN state
2. Command input triggers position change
3. Outputs indicate current position
4. Motor auto-stops on position reached

## States
- UNKNOWN (0)
- DUMP (1)
- TOKAMAK (2)

