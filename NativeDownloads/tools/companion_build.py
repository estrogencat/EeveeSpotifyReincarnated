"""Compile SpotifyAudioProbe.dylib (the Pixie companion) for the current target."""
from pathlib import Path
import subprocess

import linux_toolchain
from build_native_share import build_native_share
from pixie_target import companion_sources, include_directories

FRAMEWORKS = ("Foundation", "UIKit", "AudioToolbox", "AVFAudio", "CoreGraphics", "AVFoundation", "CoreMedia",
              "Network", "ImageIO", "QuartzCore", "LinkPresentation", "Security", "MessageUI", "UniformTypeIdentifiers")


def sdk_path():
    return subprocess.check_output(["xcrun", "--sdk", "iphoneos", "--show-sdk-path"], text=True).strip()


def _build_companion_linux(library, workdir):
    """Same companion, built with a system clang and Theos' ld64.

    The Swift bridge is skipped: it needs a swiftc targeting arm64-apple-ios,
    which no linux toolchain ships. Everything reaches it through
    NSClassFromString, so the companion still builds, links and runs without it
    - only share-as-file goes missing.
    """
    library, workdir = Path(library), Path(workdir)
    sdk = linux_toolchain.sdk_path()
    common = ["-arch", "arm64", "-target", "arm64-apple-ios15.0", "-isysroot", sdk,
              "-F", linux_toolchain.subframeworks(sdk)]
    flags = ["-fobjc-arc", "-std=gnu11", "-O2", "-fvisibility=hidden",
             "-Wall", "-Wextra", "-Wno-unused-parameter", "-Wno-nullability-completeness",
             "-Werror=implicit-function-declaration",
             "-DSAP_RESEARCH_DUMP=0", "-DSAP_ENABLE_WAV_CAPTURE=0"]
    flags += ["-I" + str(path) for path in include_directories()]
    objects = []
    for index, source in enumerate(companion_sources()):
        obj = workdir / "companion-{:03d}.o".format(index)
        subprocess.run([linux_toolchain.compiler(), "-c"] + common + flags +
                       [str(source), "-o", str(obj)], check=True)
        objects.append(str(obj))
    command = [linux_toolchain.tool("clang"), "-dynamiclib"] + common + objects
    command += ["-lm", "-lsqlite3", "-lz"]
    runtime = linux_toolchain.compiler_rt()
    if runtime:
        command.append(runtime)
    for framework in FRAMEWORKS:
        command += ["-framework", framework]
    command += ["-Wl,-install_name,@rpath/SpotifyAudioProbe.dylib", "-o", str(library)]
    subprocess.run(command, check=True)
    linux_toolchain.adhoc_sign(library)
    linux_toolchain.validate(library)
    return library.read_bytes()


def build_companion(library, workdir):
    """Compile, ad-hoc sign and validate the companion at `library`; return its bytes."""
    if not linux_toolchain.DARWIN:
        return _build_companion_linux(library, workdir)
    library, workdir, sdk = Path(library), Path(workdir), sdk_path()
    swift = build_native_share(workdir / "NativeShareBridge.o", sdk)
    command = ["xcrun", "clang", "-dynamiclib", "-arch", "arm64", "-target", "arm64-apple-ios15.0",
               "-isysroot", sdk, "-fobjc-arc", "-std=gnu11", "-O2", "-fvisibility=hidden",
               "-Wall", "-Wextra", "-Wno-unused-parameter", "-Wno-nullability-completeness",
               "-Werror=implicit-function-declaration"]
    command += ["-I" + str(path) for path in include_directories()]
    command += [str(path) for path in companion_sources()]
    command += ["-lm", "-lsqlite3", "-lz", *swift["link_args"]]
    for framework in FRAMEWORKS:
        command += ["-framework", framework]
    command += ["-Wl,-install_name,@rpath/SpotifyAudioProbe.dylib", "-o", str(library)]
    subprocess.run(command, check=True)
    subprocess.run(["codesign", "--force", "--sign", "-", "--timestamp=none", str(library)], check=True)
    subprocess.run(["codesign", "--verify", "--strict", str(library)], check=True)
    subprocess.run(["xcrun", "dyld_info", "-validate_only", str(library)], check=True)
    return library.read_bytes()
