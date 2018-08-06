# MemDebug
### C++ "Plug N' Play" Style Memory Debugging Library

## How It Works
This library automatically hooks into `operator new()` and `operator delete()`, so there is no need to search and replace your memory allocations. All of the debugging data is stored in a managed singleton object, so all you have to do to fetch information from it is to call the provided functions in `Include/MemDebug.h`.
*Note: Verify the validity of any pointers returned by the interface header before using them, as these will be null pointers during configurations that don't have the library enabled.*

## Usage
#### Visual Studio:
After adding the `Include/MemDebug.h` header, go to your project properties, select `C/C++`, and under `Preprocessor` append the preprocessor definition `MEMDEBUG` for your debugging configurations. Any configurations without this defined will have the debugging library disabled and return null information.

![](https://github.com/XenonicDev/MemDebug/blob/master/GitHub/PreprocessorDefinitions.png "Project Properties -> C/C++ -> Preprocessor")

*Note: If you don't have a `C/C++` section in the properties page of your project, you can make it visible by adding a C++ source file to the project or modifying `<MyProject>.vcxproj` to include `<PreprocessorDefinitions>MEMDEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>` under the `<ClCompile>` section for your debugging configurations.*

#### Documentation is provided in the `Include/MemDebug.h` interface header.
