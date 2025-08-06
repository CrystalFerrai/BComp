## BComp

Command line utility for quickly comparing two files or directories to determine of they are binary equal.

## Usage

From a command prompt, run the program with no parameters to print the usage.

```
Performs a binary equality comparison on two files or two directories with
similar contents. Prints results to stdout and errors to stderr.

Returns 0 if everything matches, 1 if there are any differences.
Returns -1 if any error occurs.

Usage: bcomp [[-f Filter] [-r]] [First Path] [Second Path]

  First Path   The first path to compare. Can be a file or directory.

  Second Path  The second path to compare. Must match first path type (file.
               or directory)

Options

  -f Filter  If paths are directories, filter file names using this filter.

  -r         If paths are directories, recursively compare all subdirectories.
```

**Example: files**

Compare two specific files.
```
bcomp "C:\FileA.dat" "C:\FileB.dat"
```

**Example: directories**

Compare all .dat files in two directories.
```
bcomp -f "*.dat" "C:\FolderA" "C:\FolderB"
```

**Example: directory tree**

Compare all .dat files which have names starting with "foo" in two directories, including their subdirectories. 
```
bcomp -f "foo*.dat" -r "C:\FolderA" "C:\FolderB"
```

## Releases

Release can be found [here](https://github.com/CrystalFerrai/BComp/releases).

You will need to have the Visual C++ x64 redistributable for Visual Studio 2022 installed on your system to run this program. You can find the installer at Microsoft's website [here](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist). Look for the X64 download link.

## Building

To build from source, just open the SLN file in Visual Studio 2022 and build it. For older versions of Visual Studio, you should be able to create a solution containing the library's single source file and build that.

## Platforms Supported

**Windows Only**

BComp uses Windows-specific APIs for improved performance when running on Windows. Other platforms have similar APIs, but only Windows has been implemented. Therefore, this library will not work on other platforms.
