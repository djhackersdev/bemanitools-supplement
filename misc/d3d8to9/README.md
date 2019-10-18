# DirectX 8 to 9 wrapper
Source: https://github.com/crosire/d3d8to9

Includes:
* d3d8.dll (compiled binary)
* Source code for compiled binary

# Why?
Some Konami games are quite old and don't run well on modern versions of Windows (7 and newer):
* Framerate is not locked to v-sync though v-sync is turned on
* Random stuttering and slow-downs

Games with these issues experienced thus far:
* IIDX 9 to 13: Even with bemanitools various settings in place, these games would not run flawlessly on Windows 10: Random stuttering in menus and during songs causing them to drift off-sync though the framerate was supposed to be stable.

# Install
* Requires the D3D9 SDK: https://www.microsoft.com/en-us/download/confirmation.aspx?id=8109
* Put the d3d8.dll next to your game executable.

For example:
* IIDX: Put d3d8.dll next to bm2dx.exe.
