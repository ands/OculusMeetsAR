OculusMeetsAR
=============
<p align="center">
  <img src="https://raw.githubusercontent.com/wiki/ands/OculusMeetsAR/images/explosion.jpg" alt="OculusMeetsAR - Hardware Explosion"/>
</p>

OculusMeetsAR is a library to use the Oculus Rift DK2 Head Mounted Display, two Logitech C310 Webcams (with fisheye lenses) and the Natural Point OptiTrack Flex 3 tracking hardware in Augmented Reality applications.


A few example videos:

<p align="center">
  <a href="https://www.youtube.com/watch?v=00ViRXw4kz8&index=1&list=PLhHrY3VnlsyW8hXCeFxSEMeTvPfaKLa_S">
    <img src="https://raw.githubusercontent.com/wiki/ands/OculusMeetsAR/images/RemoteGameThumb.png" alt="OculusMeetsAR - Example video playlist"/>
  </a>
</p>

Please visit our wiki for documentation: https://github.com/ands/OculusMeetsAR/wiki


Project Files
=============
- **ARLib:** The OculusMeetsAR libraries. They can be used to create augmented reality applications with an Oculus Rift DK2, attached cameras and a tracking system. Also features OGRE integration.
- **ARLib_Samples:** Two simple example applications that use ARLib with OGRE.
- **Hardware:** Contains our camera calibration tool and 3D-Printable camera and lens mounts.
- **TrackingTestServer:** A simple tracking system server that rotates a rigid body (to test ARLibTracking without a real setup).

Included Third Party Libraries
==============================
- OGRE v1.9 with Bullet
- LibOVR v0.4.3
- NatNetSDK v2.7
- TinyThread++ 1.1
- OpenCV 2.4.10 (only for the calibration tool)

Building
========
For Visual Studio 2010 with service pack 1 ONLY (if you want to use OGRE v1.9)!

- Execute SET_ENV.bat to set environment variables for all libraries prior to building.
- Open the ARLib solution file and batch build everything.
- Open up the ARLib_Samples solution file and set the working directory for the example projects to "$(TargetDir)" (those are not saved in the Visual Studio project files - don't ask...).
- Build and run the ARLib_Samples.
- Enjoy.
- $Profit.
