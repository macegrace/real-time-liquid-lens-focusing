NAME:
    AutofocusSweep

AUTHOR:
    Martin Zaťovič

DESCRIPTION:
    The AutofocusSweep executable performs an image-based autofocus,
    using OpenCV's Sobel edge detector. The algorithm captures a 
    total of 36 frames, calculates their sharpness scores, finds
    the sharpes one and focuses the lens to the same focal length
    as when taking this frame.

HARDWARE:
    The program was implemented for the following hardware:
        IDS UI-3860CP-M-GL R2 Camera, 
        Corning Varioptic USB-M Flexiboard with Max14574 Driver

DEPENDENCIES:
    opencv, 
    uEye - download from https://en.ids-imaging.com/ (registration
    required)
