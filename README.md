OculusMeetsAR
=============

THIS IS A WORK IN PROGRESS, NOT AN USABLE LIBRARY/APPLICATION (yet)!

Resources:

- Wiki: https://github.com/ands/OculusMeetsAR/wiki



Including:

- NatNetSDK v2.7

- OGRE v1.9

- LibOVR v0.4.3

- OpenAL Soft 1.1

- TinyThread++ 1.1


For Visual Studio 2010 (with service pack 1!) ONLY

Contents
========

- ARLib: The main library. Can be used to create augmented reality applications with an Oculus Rift DK2 and attached cameras. Also features direct Ogre integration.

- ARLib_Samples: Various sample applications that use ARLib with Ogre.

- LibOVR: Static libraries from the original Oculus SDK.

- NatNetSDK: Dynamically linked libraries from the original NatNetSDK for tracking purposes.

- OgreBulletPack: The Object-Oriented Graphics Rendering Engine with Bullet Physics bindings (and the viargo input aggregation library).

Building
========

- Execute SET_ENV.bat to set environment variables for all the libraries prior to building.

- Open the ARLib solution file and batch build everything.

- Open up the ARLib_Samples solution file and set the working directory for the example projects to "$(TargetDir)" (those are not saved in the Visual Studio project files - don't ask...).

- Build and run the ARLib_Samples.

- Enjoy.

- $Profit.
