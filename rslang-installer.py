import sys

if sys.version_info.major < 3 or sys.version_info.minor < 7:
    raise Exception("required Python3.7+")

import argparse
import json
import logging
import os
import re
import shutil
import subprocess as sp
import tarfile
import tempfile
import urllib.request as request
from http import HTTPStatus
from http.client import HTTPResponse
from pathlib import Path
from typing import Literal, NoReturn, TypedDict
from urllib.error import HTTPError, URLError

ENC = sys.getdefaultencoding()

GITHUB_API_VERSION = "2022-11-28"
GITHUB_API_URL = "https://api.github.com/repos/NilFoundation/zkllvm"
GITHUB_API_HEADERS = {
    "Accept": "application/vnd.github+json",
    "X-GitHub-Api-Version": GITHUB_API_VERSION,
}

Uri = str
Datetime = str


class User(TypedDict):
    """GitHub user."""

    pass


class Asset(TypedDict):
    """GitHub release asset.

    JSON schema described here:
    https://docs.github.com/en/rest/releases/assets?apiVersion=2022-11-28
    """

    url: Uri
    browser_download_url: Uri
    id: int
    node_id: str
    name: str
    label: str | None
    state: Literal["uploaded", "open"]
    content_type: str
    size: int
    download_count: int
    created_at: Datetime
    updated_at: Datetime
    uploader: User | None


class Release(TypedDict):
    """GitHub release.

    JSON schema described here:
    https://docs.github.com/en/rest/releases/releases?apiVersion=2022-11-28
    """

    url: Uri
    html_url: Uri
    assets_url: Uri
    upload_url: Uri
    tarball_url: Uri | None
    zipball_url: Uri | None
    id: int
    node_id: str
    tag_name: str
    target_commitish: str
    name: str | None
    body: str | None
    draft: bool
    prerelease: bool
    created_at: Datetime
    published_at: Datetime | None
    author: User
    assets: list[Asset]
    body_html: str
    body_text: str
    mentions_count: int
    discussion_url: Uri
    reactions: object


def parse_args():
    description="""This is an installer for rslang - zkLLVM fork of Rust.
    This script finds a pre-built release of rslang for your platform and installs it.
    """
    epilog="To get more information about zkLLVM visit https://github.com/NilFoundation/zkllvm."

    parser = argparse.ArgumentParser("rslang-installer",
                                     description=description,
                                     epilog=epilog)
    parser.add_argument("release",
                        nargs='?',
                        default="latest",
                        help="release version (e.g. 'v0.1.0' or 'latest')")
    parser.add_argument("-n", "--name",
                        default="zkllvm",
                        help="name of the toolchain to use (defaults to 'zkllvm'). "
                        "Has no effect if --no-rustup is used.")
    parser.add_argument("-f", "--force",
                        action="store_true",
                        help="overwrite toolchain, if already installed")
    parser.add_argument("--no-rustup",
                        action="store_true",
                        help="install without rustup")
    parser.add_argument("--rustup-home",
                        default=None,
                        help="rustup home path (defaults to '$HOME/.rustup')")
    parser.add_argument("--prefix",
                        default=None,
                        help="path to rslang installation. "
                        "Only takes effect if --no-rustup is used. "
                        "Default value depends on platform.")
    parser.add_argument("--no-color",
                        action="store_true",
                        help="do not use color in the output")
    parser.add_argument("-v", "--verbose",
                        action="store_true")
    return parser.parse_args()


args = parse_args()

release_version: str = args.release
toolchain_name: str = args.name
force_overwrite: bool = args.force
no_rustup: bool = args.no_rustup
rustup_home = Path.home() / ".rustup"
if args.rustup_home is not None:
    rustup_home = Path(args.rustup_home)


class DebugInfoFilter(logging.Filter):
    def filter(self, rec):
        return rec.levelno in (logging.DEBUG, logging.INFO)


class ColorFormatter(logging.Formatter):

    grey = "\x1b[37;20m"
    green = "\x1b[92;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    reset = "\x1b[0m"

    FORMATS = {
        logging.DEBUG: "%(name)s > " + grey + "[%(levelname)s]" + reset + " %(message)s",
        logging.INFO: "%(name)s > " + green + "[%(levelname)s]" + reset + " %(message)s",
        logging.WARNING: "%(name)s > " + yellow + "[%(levelname)s]" + reset + " %(message)s",
        logging.ERROR: "%(name)s > " + red + "[%(levelname)s]" + reset + " %(message)s",
        logging.CRITICAL: "%(name)s > " + red + "[%(levelname)s]" + reset + " %(message)s",
    }

    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record)


logger = logging.getLogger("rslang-installer")

logger.setLevel(logging.INFO)
if args.verbose:
    logger.setLevel(logging.DEBUG)

sh = logging.StreamHandler()
sh.setLevel(logging.DEBUG)

if args.no_color:
    formatter = logging.Formatter("%(name)s > [%(levelname)s] %(message)s")
else:
    formatter = ColorFormatter()

sh.setFormatter(formatter)
logger.addHandler(sh)

logger.info("Rslang installer")
logger.info("")


def error(message: str) -> NoReturn:
    """Print error message and exit with code `1`."""
    for part in message.split("\n"):
        logger.error(part)
    logger.error("")
    logger.error("Exiting...")
    sys.exit(1)


def cmd_exists(cmd: str) -> bool:
    return shutil.which(cmd) is not None


def need_cmd(cmd: str):
    """Exits with error, if `cmd` as a shell command is not found."""

    if not cmd_exists(cmd):
        error(f"`{cmd}` not found")


def cmd_output(cmd: str) -> str:
    """Run `cmd` in shell, exiting with error, if it fails."""

    try:
        logger.debug(f"Running '{cmd}'")
        output = sp.check_output(cmd.split()).strip()
    except sp.CalledProcessError as e:
        msg = ""
        if e.stdout is not None:
            msg += e.stdout.decode(ENC)
        if e.stderr is not None:
            msg += e.stderr.decode(ENC)
        error(f"{' '.join(e.cmd)}\n{msg}")
    return output.decode(ENC)


def cmd_output_no_fail(cmd: str) -> str | None:
    """Run `cmd` in shell, returning `None`, if it fails."""

    try:
        logger.debug(f"Running '{cmd}'")
        output = sp.check_output(cmd.split()).strip()
    except (sp.CalledProcessError, OSError) as e:
        return None
    return output.decode(ENC)


# This is taken from bootstrap.py triple detection in Rust repo.
def get_host_triple() -> str:
    """Try to get host triple in LLVM-compatible format."""

    logger.debug("Detecting host triple")
    # If the user already has a host build triple with an existing `rustc`
    # install, use their preference. This fixes most issues with Windows builds
    # being detected as GNU instead of MSVC.
    try:
        output = sp.check_output(["rustc", "--version", "--verbose"],
                                 stderr=sp.DEVNULL)
        version = output.decode(ENC)
        host = next(x for x in version.split('\n') if x.startswith("host: "))
        triple = host.split("host: ")[1]
        logger.debug(f"Detected host triple '{triple}' "
                     "from pre-installed rustc")
        return triple
    except Exception as e:
        logger.debug(f"Pre-installed rustc not found: {e}")
        logger.debug("Falling back to auto-detect")

    if sys.platform == "win32":
        ostype = cmd_output_no_fail("uname -s")
        cputype = cmd_output_no_fail("uname -m")
    else:
        ostype = cmd_output("uname -s")
        cputype = cmd_output("uname -m")

    # If we do not have `uname`, assume Windows.
    if ostype is None or cputype is None:
        return "x86_64-pc-windows-msvc"

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
        os_from_sp = cmd_output("uname -o")
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
        need_cmd("isainfo")
        cputype = cmd_output("isainfo -k")
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
        error(f"unknown OS type: {ostype}")

    if cputype in ["powerpc", "riscv"] and ostype == "unknown-freebsd":
        cputype = cmd_output("uname -p")
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
            cputype = cmd_output("uname -p")
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
            error(f"unknown byteorder: {sys.byteorder}")
    elif cputype == "mips64":
        if sys.byteorder == "big":
            cputype = "mips64"
        elif sys.byteorder == "little":
            cputype = "mips64el"
        else:
            error(f"unknown byteorder: {sys.byteorder}")
        # only the n64 ABI is supported, indicate it
        ostype += "abi64"
    elif cputype == "sparc" or cputype == "sparcv9" or cputype == "sparc64":
        pass
    else:
        error(f"unknown cpu type: {cputype}")

    return f"{cputype}-{ostype}"


host = get_host_triple()
logger.info(f"Host platform: {host}")


def get_install_prefix(platform: str) -> Path:
    """"Get default installation prefix for different platforms."""

    if "linux" in platform:
        prefix = "/opt/rslang"
    elif "darwin" in platform:
        prefix = "/usr/local/opt"
    elif "windows" in platform:
        prefix = "C:\Program Files"
    else:
        # TODO: enrich list of known locations for other platforms
        error(f"{platform} default prefix is unknown.\n"
              "Please use --prefix option.")
    return Path(prefix)


if no_rustup:
    if args.prefix is None:
        prefix = get_install_prefix(host)
    else:
        prefix = Path(args.prefix).resolve()
else:
    need_cmd("rustup")

    logger.info(f"Rustup home: {rustup_home}")

    toolchains = cmd_output("rustup toolchain list").split("\n")
    pattern = re.compile(f"^{toolchain_name}( \(default\))?$")
    toolchain_exists = any([pattern.match(name) for name in toolchains])

    if toolchain_exists:
        if force_overwrite:
            logger.info(f"Uninstalling toolchain {toolchain_name}")
            cmd = ["rustup", "toolchain", "uninstall", toolchain_name]
            sp.check_output(cmd, stderr=sp.DEVNULL)
        else:
            error(f"{toolchain_name} toolchain already exists. "
                  "If you want to override it, use -f/--force."
                  "Or use -n/--name to choose a different name.")
    prefix = rustup_home / "toolchains" / toolchain_name

prefix.mkdir(parents=True, exist_ok=True)

logger.info(f"Installation prefix: {prefix}")


def fetch_release(version: str) -> Release:
    """Get release info from GitHub API."""

    if version == "latest":
        url = f"{GITHUB_API_URL}/releases/latest"
    else:
        url = f"{GITHUB_API_URL}/releases/tags/{version}"

    logger.debug(f"Querying {url}")
    req = request.Request(url, headers=GITHUB_API_HEADERS)
    try:
        response: HTTPResponse = request.urlopen(req)
    except HTTPError as e:
        if e.code == HTTPStatus.NOT_FOUND:
            error(f"{version} release not found")
        error(f"Failed to fetch {version} release - {e}")
    except URLError as e:
        error(f"Failed to reach release server: {e.reason}")

    release: Release = json.load(response)
    if release["draft"] or release["prerelease"]:
        error(f"{version} release is not published."
              "If you want to get the newest possible version of rslang, "
              "consider building from sources.")
    return release


logger.info(f"Querying for {release_version} release")
release = fetch_release(release_version)
logger.info(f"Found {release['tag_name']} release")
logger.debug(f"{len(release['assets'])} assets, "
             f"published at {release['published_at']}")


def find_asset_by_name(release: Release, pattern: re.Pattern) -> Asset | None:
    for asset in release["assets"]:
        if pattern.match(asset["name"]):
            return asset
    return None


def find_host_toolchain() -> Asset | None:
    pat = re.compile(f"rust-[0-9]+\.[0-9]+\.[0-9]+-{host}\.tar\.gz")
    return find_asset_by_name(release, pat)


def find_assigner_target() -> Asset | None:
    assigner = "assigner-unknown-unknown"
    pat = re.compile(f"rust-std-[0-9]+\.[0-9]+\.[0-9]+-{assigner}\.tar\.gz")
    return find_asset_by_name(release, pat)


host_toolchain = find_host_toolchain()
if host_toolchain is None:
    error(f"Could not find toolchain for {host} platform.\n"
          "This platform is probably not supported.")
logger.debug(f"Found host toolchain: {host_toolchain['name']}")


assigner_target = find_assigner_target()
if assigner_target is None:
    error("Could not find assigner target in this release.\n"
          "This is probably an internal error.")
logger.debug(f"Found assigner target: {assigner_target['name']}")


def download_asset(asset: Asset, tmpdir: Path) -> Path:
    """Download given asset into tmpdir, returning path to local file."""

    logger.debug(f"Fetching {asset['browser_download_url']}")
    try:
        response: HTTPResponse = request.urlopen(asset["browser_download_url"])
    except HTTPError as e:
        if e.code == HTTPStatus.NOT_FOUND:
            error(f"Asset not found: {asset['name']}")
        error(f"Failed to fetch asset {asset['name']} - {e}")
    except URLError as e:
        error(f"Failed to reach asset server: {e.reason}")

    local_file = tmpdir / asset["name"]
    logger.debug(f"Writing to file {local_file}")
    with open(local_file, 'wb') as f:
        shutil.copyfileobj(response, f)
    return local_file


def unpack_archive(path: Path, tmpdir: Path) -> Path:
    """Unpack given archive inplace, returning the name of root directory inside it."""

    # FIXME: probably there is a better way to get ouput dir name.
    # We should consider supporting different archive types,
    # since we don't know exactly which one will be used in the future.
    with tarfile.open(path, "r") as tar:
        tar_info = tar.next()
        if tar_info is None:
            error(f"{os.path.basename(path)}\n"
                  "This archive is malformed (empty).")
        name = tar_info.path

    logger.debug(f"Processing archive {path}")
    shutil.unpack_archive(path, tmpdir)
    return tmpdir / name


def process_dist(asset: Asset, tmpdir: Path):
    """Download asset, unpack it and install."""

    logger.info(f"Downloading {asset['name']}")
    path = download_asset(asset, tmpdir)

    logger.info(f"Unpacking {asset['name']}")
    dirname = unpack_archive(path, tmpdir)

    logger.info(f"Installing {os.path.basename(dirname)}")
    cmd_output(f'{dirname}/install.sh '
               f'--destdir={prefix} '
               '--prefix="" '
               '--disable-ldconfig')


with tempfile.TemporaryDirectory() as tmpdir:
    logger.debug(f"Tmp directory created: {tmpdir}")

    process_dist(host_toolchain, Path(tmpdir))
    process_dist(assigner_target, Path(tmpdir))

logger.info("")
logger.info("Installation completed")
logger.info("")
if no_rustup:
    logger.info("You can check your installation:")
    logger.info(f"    {prefix}/bin/rustc -V")
    logger.info("")
    logger.info("Note: you were using a custom installation prefix, "
                "so don't forget to add it to PATH.")
else:
    logger.info("You can check your installation:")
    logger.info(f"    rustc +{toolchain_name} -V")
