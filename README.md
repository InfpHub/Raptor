# Raptor

[![Powered by Qt](https://img.shields.io/badge/Powered%20by-Qt-49CF59.svg)](https://changelog.com/jsparty/96)
[![Discuss on GitHub](https://img.shields.io/badge/Discuss%20on-GitHub-00A4FF)](https://github.com/InfpPointPlus/Raptor/discussions)

使用 Qt 和 C++ 构建的低内存、简洁和美观的第三方阿里云盘桌面应用客户端🚀

Raptor 由 **Qt5** 💚 驱动，与其他基于 Chromium 的解决方案（如 Electron）相比，这使得其 CPU 和内存使用较低

[//]: # (Visit https://github.com/InfpPointPlus/Raptor for docs.)

---

## 我们已提供

- 🧬 用户 - 持久化登录，多用户登录，多用户切换
- 📉 文件（夹） - 创建/多级创建文件夹/重命名/搜索
- 💅 文件（夹） - 移动/复制
- ✅ 文件（夹） - 上传/下载/唤起第三方下载器下载
- 🎪 视频 - MPV/VLC/PotPlayer
- 💸 分享 - 创建分享/导入分享
- 🦹🏻‍♀️ 秒传 - 创建秒传/导入秒传
- ⭐️ 收藏 - 收藏/取消收藏/搜索
- 🕵️‍♂️ 回收站 - 清空/删除/恢复
- 📚 Plus - 跨网盘文件复制
- 🏅 UI - 可自由修改的字体，图标与主题

## 今后的工作

- 不断迭代，将最好最稳定的客户端献给最需要的你
- 主流 Linux 桌面系统适配（也许是 Qt，也许是 PyQt，也许是也许...）
- 在线文档站点搭建
- ...

## 从代码开始

本节对于那些想要从源代码构建 Raptor 的人来说很有用

### 必要的工具集

- Visual Studio 2019
- Qt 5.15（静态或动态）
- CMake 3.5 及其更高版本
- JetBrains CLion 2024

### 第三方依赖库

- [ZInt](https://www.zint.org.uk)
- [Curl](https://curl.se)
- [Crypto++](https://www.cryptopp.com)（仅静态编译需要）
- [Sass](https://sass-lang.com)
- [SECP256K1](https://github.com/bitcoin-core/secp256k1)（仅动态编译需要）

编译之后按照 `CMakeList.txt` 中的头文件路径和库路径放置

### 即刻开始构建

#### Debug

- 使用 JetBrains CLion 打开 Raptor
- 根据向导配置 Visual Studio 工具集
- 修改 `CMakeList.txt` 中的 `CMAKE_PREFIX_PATH` 为您的 Qt 安装目录
- 修改 `CMakeList.txt` 中的 `Standalone` 为 `ON` （静态编译） 或者 `OFF` （动态编译）
- 点击右上角编译图标以构建
- 最后，在 `Target/[x64|x86]/[Debug|Release]/[Static|Share]` 中可查看二进制程序

#### Release

- 编辑 `Script` 中的脚本，修改 `QT_DIR` 和 `Visual Studio 2019 命令提示符` 为相应目录
- 运行相关脚本。稍等片刻，在 `Dist` 中即可查看压缩好的二进制发行包

PS：如果您选择静态编译则对应的 Qt 工具集也必须是静态的，动态编译则需要动态编译的 Qt 工具集

## 声明

Raptor 是自由软件，依据 [GNU General Public License V3.0](https://www.gnu.org/licenses) 授权发布。你可以再分发之和/或依照由自由软件基金会发布的 GNU 通用公共许可证修改之，无论是版本 3 许可证，还是（按你的决定）任何以后版都可以。发布 Raptor 是希望它能有用，但是并无保障；甚至连可销售和符合某个特定的目的都不保证。请参看 GNU 通用公共许可证，了解详情。你应该随程序获得一份 GNU 通用公共许可证的复本。如果没有，请看 [在线许可证](https://www.gnu.org/licenses)

Raptor 所使用的第三方依赖库，其许可可在 `Store/Legal` 中找到

## 需要您的支持

Raptor 是一个开源项目，需要您的支持。如果你喜欢这个项目，请考虑通过点击这个 Github 存储库上的 Star 按钮来支持我的工作