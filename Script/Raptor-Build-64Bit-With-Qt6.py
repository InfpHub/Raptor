#
# Copyright (C) 2024 凉州刺史. All rights reserved.
#
# This file is part of Raptor.
#
# $RAPTOR_BEGIN_LICENSE$
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl.html.
#
# $RAPTOR_END_LICENSE$
#
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#

import os
import platform
import shutil
import subprocess
import zipfile
from typing import List

APPLICATION_NAME: str = "Raptor"
MINGW_DIR: str = r"C:\Qt\Tools\mingw1310_64\bin"
QT_DIR: str = r"C:\Qt\6.8.0\mingw_64"
SCRIPT_DIR: str = os.path.dirname(os.path.abspath(__file__))
BUILD_DIR: str = os.path.abspath(os.path.join(SCRIPT_DIR, "..", f"{APPLICATION_NAME}-Legend-Never-Die"))
DIST_DIR: str = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "Dist"))
OUTPUT_NAME: str = ""


def invokeSuiteAssert():
    if not os.path.exists(MINGW_DIR):
        raise FileNotFoundError(MINGW_DIR)

    if not os.path.exists(QT_DIR):
        raise FileNotFoundError(QT_DIR)


def invokeWindowsBuild():
    os.environ["Path"] = f"{MINGW_DIR};{os.environ['Path']}"
    if os.path.exists(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)

    _CmakeArgs: List[str] = [
        "cmake",
        "-DCMAKE_BUILD_TYPE=Release",
        f"-DCMAKE_PREFIX_PATH={QT_DIR}",
        "-G",
        "MinGW Makefiles",
        "-S",
        "..",
        "-B",
        BUILD_DIR
    ]
    subprocess.run(_CmakeArgs, check=True)
    _BuildArgs: List[str] = [
        "cmake",
        "--build",
        BUILD_DIR
    ]
    subprocess.run(_BuildArgs, check=True)
    _AbsolutePath: str = os.path.abspath(os.path.join(SCRIPT_DIR, "..", "Target", "Release"))
    if not os.path.exists(_AbsolutePath):
        os.makedirs(_AbsolutePath)

    _Log: str = os.path.join(_AbsolutePath, f"{APPLICATION_NAME}.log")
    if os.path.exists(_Log):
        os.remove(_Log)

    _ThirdParty: List[str] = [
        r"Windows\Curl\bin\Curl.dll",
        r"Windows\OpenSSL\bin\Crypto.dll",
        r"Windows\OpenSSL\bin\SSL.dll",
        r"Windows\Sass\bin\Sass.dll",
        r"Windows\SECP256K1\bin\SECP256K1.dll",
        r"Windows\UV\bin\UV.dll",
        r"Windows\Yaml\bin\Yaml.dll",
        r"Windows\ZInt\bin\ZInt.dll"
    ]
    for item in _ThirdParty:
        shutil.copy(os.path.join(SCRIPT_DIR, "..", "3rd-Party", item), _AbsolutePath)

    _WinDeployQt: str = os.path.join(QT_DIR, "bin", "WinDeployQt.exe")
    _DeployArgs: List[str] = [
        _WinDeployQt,
        "-no-translations",
        os.path.join(_AbsolutePath, f"{APPLICATION_NAME}.exe"),
    ]
    subprocess.run(_DeployArgs, check=True)
    invokePackage(_AbsolutePath)


def invokeLinuxBuild():
    pass


def invokeMacBuild():
    pass


def invokePackage(_AbsolutePath: str):
    if os.path.exists(DIST_DIR):
        shutil.rmtree(DIST_DIR)
    os.makedirs(DIST_DIR, exist_ok=True)
    with zipfile.ZipFile(os.path.join(DIST_DIR, OUTPUT_NAME), "w", zipfile.ZIP_DEFLATED, compresslevel=9) as _Zip:
        for item, itens, iteos in os.walk(_AbsolutePath):
            for iteo in iteos:
                _Zip.write(str(os.path.join(item, iteo)), str(os.path.join(item.replace(_AbsolutePath, ""), iteo)))


if __name__ == '__main__':
    invokeSuiteAssert()
    match platform.system():
        case "Windows":
            OUTPUT_NAME = f"{APPLICATION_NAME}-Windows-x64.zip"
            invokeWindowsBuild()
        case "Linux":
            OUTPUT_NAME = f"{APPLICATION_NAME}-Linux-x64.zip"
            invokeLinuxBuild()
        case "Darwin":
            OUTPUT_NAME = f"{APPLICATION_NAME}-Mac-x64.zip"
            invokeMacBuild()
        case _:
            print(f"Unsupported System: {platform.system()}")
    print("Done!")
