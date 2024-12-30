# HueShiftCore

*Visual Studio framework for rapid development of hook/hijack DLLs*

> [!NOTE]
> This repository is part of my *Year-end Things Clearance 2024* code dump, which implies the following things:
> 
> 1. The code in this repo is from one of my personal hobby projects (mainly) developed in 2024
> 2. Since it's a hobby project, expect PoC / alpha-quality code, and an *astonishing* lack of documentation
> 3. The project is archived by the time this repo goes public; there will (most likely) be no future updates
> 
> In short, if you wish to use the code here for anything good, think twice.

This project originates from a private project called "HueShift" back in 2020, which involves injecting a DLL into some program to hook many APIs it uses. It's a relatively simple matter, just import [Detours](https://github.com/microsoft/Detours), write hook routines with boilerplate code and call it a day; but things gets boring real fast when I realized that the same process and code needs to be repeated for every project that involves API hooks. And don't get me started on writing DLL hijack stub routines; I know you could generate them with some tool, but the mere idea of having the same function prototype repeated several times in one piece of code just sounds error-prone and tedious for me. So that's how HueShiftCore is born, and why I kept using it to this day (in late 2024).

Although called a "framework" in repository description, this project is nothing more than just a bunch of boilerplate code files with a convenient file structure and macros designed to reduce as much clutter as possible.

- No more `DllMain()` and boilerplate code for hooking and unhooking routines; a single line of `#include "dllmain.hpp"` would suffice.
- No more copying function prototypes all over the place; only one is needed (and that's where you define your hook routine), and all that original routine shenanigans are all already been taken care of.
- No more manually copy-pasing (or even worse, *writing*) tons of stub code in your DLL project; have the compiler and linker do it for you.
- There is also a pre-made DLL injector available, in case of you need to ship the hook DLL with some program; `#include "main.hpp"` and a couple of `#define`s are all you need to build it.

You will need Visual Studio with C++ capability to use this project. Check the projects in `Examples` directory for ideas about how this project works, and how to create your own hook DLLs with this framework.

This project utilizes Detours (as mentioned above) and includes it as a git submodule. In order to build anything, you would need to obtain it first by either use `git clone --recursive ...` while cloning this repo, or run `git submodule update --init --recursive` afterwards.

