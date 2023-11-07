# This script is taken from bootstrap.py from Rust repository.
# The purpose of this sdcipt is to detect host platform name in a format
# which is expected by Rust. Rust target triples are slightly different from
# LLVM, e.g. when LLVM_HOST_TRIPLE is `arm64-apple-darwin22.5.0`, Rust host name
# is `aarch64-apple-darwin`.


import os
import subprocess
import sys


def require(cmd, exit=True, exception=False):
    """Run a command, returning its output.
    On error,
        If `exception` is `True`, raise the error
        Otherwise If `exit` is `True`, exit the process
        Else return None."""
    try:
        return subprocess.check_output(cmd).strip()
    except (subprocess.CalledProcessError, OSError) as exc:
        if exception:
            raise
        elif exit:
            print("error: unable to run `{}`: {}".format(" ".join(cmd), exc), file=sys.stderr)
            print("Please make sure it's installed and in the path.", file=sys.stderr)
            sys.exit(1)
        return None


def default_build_triple():
    """Build triple as in LLVM"""
    # If the user already has a host build triple with an existing `rustc`
    # install, use their preference. This fixes most issues with Windows builds
    # being detected as GNU instead of MSVC.
    default_encoding = sys.getdefaultencoding()
    try:
        version = subprocess.check_output(
            ["rustc", "--version", "--verbose"], stderr=subprocess.DEVNULL
        )
        version = version.decode(default_encoding)
        host = next(x for x in version.split("\n") if x.startswith("host: "))
        triple = host.split("host: ")[1]
        return triple
    except Exception:
        # falling back to auto-detect
        pass

    required = sys.platform != "win32"
    ostype = require(["uname", "-s"], exit=required)
    cputype = require(["uname", "-m"], exit=required)

    # If we do not have `uname`, assume Windows.
    if ostype is None or cputype is None:
        return "x86_64-pc-windows-msvc"

    ostype = ostype.decode(default_encoding)
    cputype = cputype.decode(default_encoding)

    # The goal here is to come up with the same triple as LLVM would,
    # at least for the subset of platforms we're willing to target.
    ostype_mapper = {
        "Darwin": "apple-darwin",
        "DragonFly": "unknown-dragonfly",
        "FreeBSD": "unknown-freebsd",
        "Haiku": "unknown-haiku",
        "NetBSD": "unknown-netbsd",
        "OpenBSD": "unknown-openbsd",
    }

    # Consider the direct transformation first and then the special cases
    if ostype in ostype_mapper:
        ostype = ostype_mapper[ostype]
    elif ostype == "Linux":
        os_from_sp = (
            subprocess.check_output(["uname", "-o"]).strip().decode(default_encoding)
        )
        if os_from_sp == "Android":
            ostype = "linux-android"
        else:
            ostype = "unknown-linux-gnu"
    elif ostype == "SunOS":
        ostype = "pc-solaris"
        # On Solaris, uname -m will return a machine classification instead
        # of a cpu type, so uname -p is recommended instead.  However, the
        # output from that option is too generic for our purposes (it will
        # always emit 'i386' on x86/amd64 systems).  As such, isainfo -k
        # must be used instead.
        cputype = require(["isainfo", "-k"]).decode(default_encoding)
        # sparc cpus have sun as a target vendor
        if "sparc" in cputype:
            ostype = "sun-solaris"
    elif ostype.startswith("MINGW"):
        # msys' `uname` does not print gcc configuration, but prints msys
        # configuration. so we cannot believe `uname -m`:
        # msys1 is always i686 and msys2 is always x86_64.
        # instead, msys defines $MSYSTEM which is MINGW32 on i686 and
        # MINGW64 on x86_64.
        ostype = "pc-windows-gnu"
        cputype = "i686"
        if os.environ.get("MSYSTEM") == "MINGW64":
            cputype = "x86_64"
    elif ostype.startswith("MSYS"):
        ostype = "pc-windows-gnu"
    elif ostype.startswith("CYGWIN_NT"):
        cputype = "i686"
        if ostype.endswith("WOW64"):
            cputype = "x86_64"
        ostype = "pc-windows-gnu"
    elif sys.platform == "win32":
        # Some Windows platforms might have a `uname` command that returns a
        # non-standard string (e.g. gnuwin32 tools returns `windows32`). In
        # these cases, fall back to using sys.platform.
        return "x86_64-pc-windows-msvc"
    else:
        err = "unknown OS type: {}".format(ostype)
        sys.exit(err)

    if cputype in ["powerpc", "riscv"] and ostype == "unknown-freebsd":
        cputype = (
            subprocess.check_output(["uname", "-p"]).strip().decode(default_encoding)
        )
    cputype_mapper = {
        "BePC": "i686",
        "aarch64": "aarch64",
        "amd64": "x86_64",
        "arm64": "aarch64",
        "i386": "i686",
        "i486": "i686",
        "i686": "i686",
        "i786": "i686",
        "m68k": "m68k",
        "powerpc": "powerpc",
        "powerpc64": "powerpc64",
        "powerpc64le": "powerpc64le",
        "ppc": "powerpc",
        "ppc64": "powerpc64",
        "ppc64le": "powerpc64le",
        "riscv64": "riscv64gc",
        "s390x": "s390x",
        "x64": "x86_64",
        "x86": "i686",
        "x86-64": "x86_64",
        "x86_64": "x86_64",
    }

    # Consider the direct transformation first and then the special cases
    if cputype in cputype_mapper:
        cputype = cputype_mapper[cputype]
    elif cputype in {"xscale", "arm"}:
        cputype = "arm"
        if ostype == "linux-android":
            ostype = "linux-androideabi"
        elif ostype == "unknown-freebsd":
            cputype = (
                subprocess.check_output(["uname", "-p"])
                .strip()
                .decode(default_encoding)
            )
            ostype = "unknown-freebsd"
    elif cputype == "armv6l":
        cputype = "arm"
        if ostype == "linux-android":
            ostype = "linux-androideabi"
        else:
            ostype += "eabihf"
    elif cputype in {"armv7l", "armv8l"}:
        cputype = "armv7"
        if ostype == "linux-android":
            ostype = "linux-androideabi"
        else:
            ostype += "eabihf"
    elif cputype == "mips":
        if sys.byteorder == "big":
            cputype = "mips"
        elif sys.byteorder == "little":
            cputype = "mipsel"
        else:
            raise ValueError("unknown byteorder: {}".format(sys.byteorder))
    elif cputype == "mips64":
        if sys.byteorder == "big":
            cputype = "mips64"
        elif sys.byteorder == "little":
            cputype = "mips64el"
        else:
            raise ValueError("unknown byteorder: {}".format(sys.byteorder))
        # only the n64 ABI is supported, indicate it
        ostype += "abi64"
    elif cputype == "sparc" or cputype == "sparcv9" or cputype == "sparc64":
        pass
    else:
        err = "unknown cpu type: {}".format(cputype)
        sys.exit(err)

    return "{}-{}".format(cputype, ostype)


if __name__ == "__main__":
    host_triple = default_build_triple()
    print(host_triple)
