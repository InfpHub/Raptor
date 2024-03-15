# Raptor 2024.2.0

发行日期：2024.03.15

发行宣言：所向无前

发行说明：

- [优化] - 升级到 Qt 6.8.0 以支持高 DPI 缩放
- [优化] - 配置文件改为 Yaml 格式。现在您无需启动 Raptor 即可编辑所有配置项
- [优化] - 导入分享支持 aliyundrive.com 和 alipan.com。并且如果输入的链接携带文件 ID 则解析后则视图可直接定位到指定 ID 下
- [优化] - Sass 样式表现在使用统一变量控制大多数控件的字体大小
- [优化] - 无论您是否具有尊贵会员身份。下载速度都可以直接跑满下行带宽，并且是可控的并行（1-32）
- [优化] - 关于页面的祝福文案将使用阿里云盘官方的每日祝福文案
- [优化] - 跨网盘复制现在支持文件夹
- [优化] - 暗色主题下二维码识别度过低
- [优化] - 现在界面字符串 "私有" 和 "公开" 改为 "备份盘" 和 "资源盘"
- [新增] - 大文件清理
- [新增] - 增加托盘图标。在 <u>设置 - Ui - 启用托盘图标</u>
- [新增] - 新版本检测与通知。在 <u>设置 - Ui - 允许来自 Raptor 仓库的新版本和公告通知</u>
- [新增] - 增加空间，收藏和回收站视图右键菜单。在 <u>设置 - Ui - 启用视图菜单</u>
- [新增] - 增加关闭时最小化到托盘。在 <u>设置 - Ui - 关闭时最小化到托盘而不是退出</u>
- [新增] - 视频播放将自动尝试加载字幕。在 <u>设置 - 视频 - 尝试自动加载字幕</u>
- [新增] - 现在，如果您是 Windows 系统，您可以配置 Microsoft Office 以预览 PDF，Word，PowerPoint 和 Excel 文件。在 <u>设置 - 办公</u>
- [新增] - 现在，您可以配置 IP 解析方式。IPV4，IPV6 或自动。在 <u>设置 - 网络 - IP 解析</u>
- [新增] - Raptor 所依赖的第三方库已提交。现在，您可以完成的编译 Raptor
- [新增] - 正在编写文档（尚未完成）
- [新增] - 可以查看所有已经登录的设备
- [新增] - 批量重命名
- [修复] - 部分使用者计算机出现因 Qt 字符串转换的错误导致无法登录
- [修复] - 索引计算错误导致点击上一个按钮切换视图程序崩溃
- [修复] - 取消下载时临时文件夹没有删除
- [修复] - 自动主题下背景图没有绘制
- [修复] - 创建分享时名称不显示的逻辑错误
- [修复] - 日志套件中的正则表达式匹配带有数字的函数名称无法匹配
- [修复] - 在编辑数据时，因视图共用相同代理导致的没有正确提交或关闭编辑器的警告
- [修复] - 因 CSS ID 选择器重复导致导入分享弹出层中分享复选框样式不预期
- [修复] - 编译脚本忘记提交至仓库
- [其他] - 不再提供 32 位版本
- [其他] - 不再提供静态编译版本
- [其他] - 编译器改为 `MinGW 13.1.0`。 为在 2024.3.0 适配 Linux 和 Mac 减少不必要的麻烦。注意：2024.2.0 携带的第三方库的二进制分发版本均使用 MinGW 13.1.0 编译，如果您要自行编译 Raptor 并且避免未知的问题出现，请使用 `Qt-Unified-Platform-x64-Online` 进行下载 `MinGW 13.1.0` 和 `Qt 6.8.0`
- [其他] - 默认不再携带第三方播放器 MPV，VLC，PotPlayer。需要自行下载并解压至 Raptor 所在路径下的 Store 下的对应文件夹

已知问题：

- 高 DPI 下最小化按钮图标消失 

# Raptor 2024.1.0

发行日期：2024.02.15

发行宣言：无人问津

发行说明：

- 初始发行

已知问题：