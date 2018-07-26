libogg, libvorbis, libtheora built as frameworks for iOS.

Highly experimental!

Based on older build instructions found at http://iosdeveloperzone.com/2012/09/30/tutorial-open-source-on-ios-part-4-compiling-libvorbis/

# Prerequisites

* Mac OS X 10.9 machine
* Xcode 5.1 with iOS 7.1 SDK and CLI tools installed
* install autoconf, automake, and libtool from Homebrew
* (...hopefully that's it...)

# Building

First, get the source:

```
git clone https://github.com/brion/OgvKit.git
git submodule update --init
```

Now build the frameworks!

```
make
```

There is a sample application, OgvDemo, which can be opened and built in Xcode.
It plays a sample video streamed from Wikimedia Commons (currently without audio).
