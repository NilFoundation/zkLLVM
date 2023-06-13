import argparse
import logging
import os
import platform
import re
import shutil
import subprocess as sp
import sys
import tarfile
import tempfile
from pathlib import Path
from typing import Callable, Literal, NoReturn, TypedDict

import requests

GITHUB_API_VERSION = "2022-11-28"
GITHUB_API_URL = "https://api.github.com/repos/NilFoundation/zkllvm-rslang"
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
    parser = argparse.ArgumentParser("rslang-installer")
    parser.add_argument("release",
                        nargs='?',
                        default="latest",
                        help="Release version (e.g. 'v0.1.0' or 'latest')")
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


class Triple:
    """Target triplet, e.g. `x86_64-unknown-linux-gnu`."""

    def __init__(self, machine: str, vendor: str, _os: str):
        self.machine = machine.lower()
        self.vendor = vendor.lower()
        self.os = _os.lower()

    @classmethod
    def host(cls):
        """Get host triple."""
        machine = platform.machine().lower()
        vendor = "unknown"  # FIXME: fill this
        _os = platform.system().lower()  # FIXME: handle postfixes like "-gnu" and "-musl"
        return cls(machine, vendor, _os)

    @classmethod
    def assigner(cls):
        """Get assigner triple."""
        return cls("assigner", "unknown", "unknown")

    def is_windows(self) -> bool:
        return host.os.startswith("windows")

    def is_macos(self) -> bool:
        return host.os.startswith("darwin")

    def is_linux(self) -> bool:
        return host.os.startswith("linux")

    def __str__(self):
        return f"{self.machine}-{self.vendor}-{self.os}"


host = Triple.host()
logger.info(f"Host platform: {host}")


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
        output = sp.check_output(cmd.split())
    except sp.CalledProcessError as e:
        msg = ""
        if e.stdout is not None:
            msg += e.stdout.decode()
        if e.stderr is not None:
            msg += e.stderr.decode()
        error(f"{' '.join(e.cmd)}\n{msg}")
    return output.decode()


def get_default_install_prefix() -> Path:
    """"Get default installation prefix for different platforms."""

    if host.is_linux():
        prefix = "/opt/rslang"
    elif host.is_macos():
        prefix = "/usr/local/opt"
    elif host.is_windows():
        prefix = "C:\Program Files"
    else:
        error(f"{host} default prefix is unknown.\n"
              "Please use --prefix option.")
    return Path(prefix)


if no_rustup:
    if args.prefix is None:
        prefix = get_default_install_prefix()
    else:
        prefix = Path(args.prefix).resolve()
else:
    need_cmd("rustup")

    logger.info(f"Rustup home: {rustup_home}")

    toolchains = cmd_output("rustup toolchain list").strip().split("\n")
    pattern = re.compile(f"^{toolchain_name}( \(default\))?$")
    toolchain_exists = any([pattern.match(name) for name in toolchains])

    if toolchain_exists:
        if force_overwrite:
            # FIXME: maybe we should silence the output of this command, which sends to stderr
            cmd_output(f"rustup toolchain uninstall {toolchain_name}")
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
    response = requests.get(url, headers=GITHUB_API_HEADERS)
    if response.status_code == requests.codes.not_found:
        error(f"{version} release not found")
    if response.status_code != requests.codes.ok:
        error("Response is not 200 OK:\n"
              f"    {response.status_code} {response.reason}")

    release: Release = response.json()
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


def find_asset_by_name(release: Release, filter: Callable[[Asset], bool], pattern: re.Pattern) -> Asset | None:
    for asset in release["assets"]:
        if filter(asset) and pattern.match(asset["name"]):
            return asset
    return None


def gzip_or_zip(asset: Asset) -> bool:
    # FIXME: this filter is probably not necessary since we trust our name pattern matching.
    return asset["content_type"] in ("application/gzip", "application/zip")


def find_host_toolchain() -> Asset | None:
    pat = re.compile(f"rust-[0-9]+\.[0-9]+\.[0-9]+-.+-{host}")
    return find_asset_by_name(release, gzip_or_zip, pat)


def find_assigner_toolchain() -> Asset | None:
    assigner_triple = Triple.assigner()
    pat = re.compile(f"rust-std-[0-9]+\.[0-9]+\.[0-9]+-.+-{assigner_triple}")
    return find_asset_by_name(release, gzip_or_zip, pat)


host_toolchain = find_host_toolchain()
if host_toolchain is None:
    error(f"Could not find toolchain for {host} platform.\n"
          "This platform is probably not supported.")
logger.debug(f"Found host toolchain: {host_toolchain['name']}")


assigner_toolchain = find_assigner_toolchain()
if assigner_toolchain is None:
    error("Could not find assigner toolchain in this release.\n"
          "This is probably an internal error.")
logger.debug(f"Found assigner toolchain: {assigner_toolchain['name']}")


def download_asset(asset: Asset, tmpdir: Path) -> Path:
    """Download given asset into tmpdir, returning path to local file."""

    logger.debug(f"Fetching {asset['browser_download_url']}")
    with requests.get(asset["browser_download_url"], stream=True) as response:
        if response.status_code == requests.codes.not_found:
            error(f"{release} release not found")
        if response.status_code != requests.codes.ok:
            error("Response is not 200 OK:\n"
                  f"    {response.status_code} {response.reason}")

        local_file = tmpdir / asset["name"]
        logger.debug(f"Writing to file {local_file}")
        with open(local_file, 'wb') as f:
            for chunk in response.iter_content(chunk_size=8192):
                f.write(chunk)
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


def process_toolchain(asset: Asset, tmpdir: Path):
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

    process_toolchain(host_toolchain, Path(tmpdir))
    process_toolchain(assigner_toolchain, Path(tmpdir))

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
