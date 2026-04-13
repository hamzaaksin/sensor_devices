sensor_devices0
├── Core
│   ├── Inc
│   │   ├── circular_buffer.h      # Circular buffer structure and definitions
│   │   ├── main.h                 # Peripheral configurations and flags
│   │   ├── median_filter.h        # Filter logic and window definitions
│   │   ├── mpu9250.h              # Sensor register map
│   │   └── stm32f4xx_it.h         # Interrupt handler declarations
│   └── Src
│       ├── main.c                 # Main loop and system initialization
│       ├── circular_buffer.c      # Static buffer and overflow management
│       ├── median_filter.c        # Moving median filter implementation
│       ├── mpu.c                  # I2C DMA reading and raw data parsing
│       └── stm32f4xx_it.c         # TIM1 and I2C DMA callback logic
├── flowchart.drawio               # Source file for the system flowchart
├── flowchart.png                  # Visual representation of the algorithm
├── Read.md                        # General information about the project
├── Report - Hamza AKSİN - Embedded Systems Developer Intern.pdf  # Final Report
└── sensor_devices0.ioc            # STM32CubeMX configuration file