# HueShiftCore

*Visual Studio framework for rapid development of hook/hijack DLLs*

> [!NOTE]
> This repository is from one of my personal hobby projects; since the only expected user is *myself*, expect PoC / alpha-quality code, an extreme lack of documentation, and no further support or maintenance are guaranteed whatsoever.
> 
> In short, if you wish to use the code here for anything good, think twice.

This project originates from a private project called "HueShift" back in 2020, which involves injecting a DLL into some program to hook many APIs it uses. It's a relatively simple matter, just import [Detours](https://github.com/microsoft/Detours), write hook routines with boilerplate code and call it a day; but things gets boring real fast when I realized that the same process and code needs to be repeated for every project that involves API hooks. And don't get me started on writing DLL hijack stub routines; I know you could generate them with some tool, but the mere idea of having the same function prototype repeated several times in one piece of code just sounds error-prone and tedious for me. So that's how HueShiftCore is born, and why I kept using it to this day.

Although called a "framework" in repository description, this project is nothing more than just a bunch of boilerplate code files with a convenient file structure and macros designed to reduce as much clutter as possible.

- No more `DllMain()` and boilerplate code for hooking and unhooking routines, all of that came pre-fabbed; just reference them in your project, write hook routines, and you're good to go.
- No more copying function prototypes all over the place; only one is needed (and that's where you define your hook routine), and all that original routine pointer shenanigans are all already been taken care of.
- No more manually copy-pasing (or even worse, *writing*) tons of stub code in your DLL project; [have the compiler and linker do it for you](Examples/DllHijackExample).
- If all you want is just hook some Windows APIs in a single module, Detours dependency can be dropped with the [IAT-modifying alternate hook implementation](Examples/IatExample).
- There is also a [pre-made DLL injector](Examples/InjectorExample) available, in case of you need to ship the hook DLL with some program; a couple of `#define`s are all you need to build it.

You will need Visual Studio with C++ capability to use this project. Check the projects in `Examples` directory for ideas about how this project works, and how to create your own hook DLLs with this framework.

This project utilizes Detours (as mentioned above) and includes it as a git submodule. In order to build anything, you would need to obtain it first by either use `git clone --recursive ...` while cloning this repo, or run `git submodule update --init --recursive` afterwards.

