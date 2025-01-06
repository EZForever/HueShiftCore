This example shows how to use the alternate hook implementation, the IatCore, for IAT hooking.
Sometimes all you need for a hook DLL to do is simply to change how some standard Windows APIs works, and, maybe preferably, only affect certain modules in the process. Detours is great, but seems a little bit too heavy for the purpose. IatCore is designed for exactly this kind of scenario.
It works by replacing the routine pointer in target module's IAT, effectively redirecting all calls to the original routine (through IAT) to the hook routine. Of course it will not work if the target uses GetProcAddress() for routine pointers, but hey, simplicity comes with its own cost, right? ;)

The process of creating a new Core project is basically the same as CoreExample, just change DetoursCore reference to IatCore, and drop Detours reference.
See hook.cpp for details (and common pitfalls) on writing IAT hook routines.

