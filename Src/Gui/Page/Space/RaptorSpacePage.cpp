/**
 *
 * Copyright (C) 2024 凉州刺史. All rights reserved.
 *
 * This file is part of Raptor.
 *
 * $RAPTOR_BEGIN_LICENSE$
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 * $RAPTOR_END_LICENSE$
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "RaptorSpacePage.h"
#include "ui_RaptorSpacePage.h"

RaptorSpacePage::RaptorSpacePage(QWidget* qParent) : QWidget(qParent),
                                                     _Ui(new Ui::RaptorSpacePage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorSpacePage::~RaptorSpacePage()
{
    FREE(_Ui)
}

bool RaptorSpacePage::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == _Ui->_ItemName)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->modifiers() == Qt::ControlModifier && qKeyEvent->key() == Qt::Key_Return)
            {
                _Ui->_ItemNameTip->setText(QStringLiteral("名称 (双击以修改):"));
                _Ui->_ItemName->setReadOnly(true);
                auto qTextCursor = _Ui->_ItemName->textCursor();
                qTextCursor.clearSelection();
                _Ui->_ItemName->setTextCursor(qTextCursor);
                if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
                    qIndexList.length() == 1)
                {
                    const auto qIndex = qIndexList[0];
                    if (const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
                        item._Name != _Ui->_ItemName->toPlainText() && !_Ui->_ItemName->toPlainText().isEmpty())
                    {
                        auto input = RaptorInput();
                        input._Index = qIndex;
                        input._Name = _Ui->_ItemName->toPlainText();
                        Q_EMIT itemRenaming(QVariant::fromValue<RaptorInput>(input));
                    }
                }

                return true;
            }
            else if (qKeyEvent->key() == Qt::Key_Escape)
            {
                _Ui->_ItemNameTip->setText(QStringLiteral("名称 (双击以修改):"));
                _Ui->_ItemName->setReadOnly(true);
                auto qTextCursor = _Ui->_ItemName->textCursor();
                qTextCursor.clearSelection();
                _Ui->_ItemName->setTextCursor(qTextCursor);
                return true;
            }
        }
    }

    if (qObject == _Ui->_ItemName->viewport())
    {
        if (qEvent->type() == QEvent::MouseButtonDblClick)
        {
            if (_Ui->_ItemView->selectionModel()->selectedRows().length() == 1 && !_Ui->_ItemName->toPlainText().isEmpty())
            {
                _Ui->_ItemNameTip->setText(QStringLiteral(R"(<p>名称 (<span style="color: rgba(32, 192, 160, 255);">[Ctrl + Enter]</span> 提交或者 <span style="color: rgba(192, 32, 60, 255);">[ESC]</span> 取消):</p>)"));
                _Ui->_ItemName->setReadOnly(false);
                auto qTextCursor = _Ui->_ItemName->textCursor();
                qTextCursor.select(QTextCursor::Document);
                _Ui->_ItemName->setTextCursor(qTextCursor);
                return true;
            }
        }
    }

    if (qObject == _Ui->_ItemUrl->viewport())
    {
        if (qEvent->type() == QEvent::MouseButtonDblClick)
        {
            if (const auto qUrl = _Ui->_ItemUrl->toPlainText();
                !qUrl.isEmpty())
            {
                qApp->clipboard()->setText(qUrl);
                RaptorToast::invokeInformationEject(QStringLiteral("链接已拷贝到剪切板!"));
                return true;
            }
        }
    }

    if (qObject == _Ui->_SearchEdit)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->key() == Qt::Key_Enter || qKeyEvent->key() == Qt::Key_Return)
            {
                onSearchClicked();
                return true;
            }
        }
    }

    if (qObject == _Ui->_ItemView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->key() == Qt::Key_Return)
            {
                if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
                    qIndexList.length() == 1)
                {
                    onItemViewDoubleClicked(qIndexList[0]);
                    return true;
                }
            }
            else if (qKeyEvent->matches(QKeySequence::Copy))
            {
                invokeItemsCopy();
                return true;
            }
            else if (qKeyEvent->matches(QKeySequence::Paste))
            {
                invokeItemsPaste();
                return true;
            }
            else if (qKeyEvent->matches(QKeySequence::Cut))
            {
                invokeItemsCut();
                return true;
            }
            else if (qKeyEvent->modifiers() == Qt::ControlModifier && qKeyEvent->key() == Qt::Key_D)
            {
                onItemDeleteClicked();
                return true;
            }
            else if (qKeyEvent->matches(QKeySequence::Delete))
            {
                onItemDeleteClicked();
                return true;
            }
            else if (qKeyEvent->matches(QKeySequence::MoveToStartOfLine))
            {
                onRootClicked();
                return true;
            }
            else if (qKeyEvent->matches(QKeySequence::Refresh))
            {
                onRefreshClicked();
                return true;
            }
            else if (qKeyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && qKeyEvent->key() == Qt::Key_Return)
            {
                invokeItemsPlay();
                return true;
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

RaptorFolder* RaptorSpacePage::invokeFolderUiGet() const
{
    return _Folder;
}

RaptorUpload* RaptorSpacePage::invokeUploadUiGet() const
{
    return _Upload;
}

RaptorImport* RaptorSpacePage::invokeImportUiGet() const
{
    return _Import;
}

RaptorDownload* RaptorSpacePage::invokeDownloadUiGet() const
{
    return _Download;
}

RaptorShare* RaptorSpacePage::invokeShareUiGet() const
{
    return _Share;
}

void RaptorSpacePage::invokeNavigate()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _Ui->_ItemView->invokeBackgroundPaintableSet(false);
    _ItemViewModel->invokeItemsClear();
    _Payload._Parent = "root";
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    const auto [qType, qCatagory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCatagory;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::invokeInstanceInit()
{
    _TabGroup = new QButtonGroup(this);
    _TabGroup->setExclusive(true);
    _TabGroup->addButton(_Ui->_TabAll);
    _TabGroup->addButton(_Ui->_TabFolder);
    _TabGroup->addButton(_Ui->_TabVideo);
    _TabGroup->addButton(_Ui->_TabAudio);
    _TabGroup->addButton(_Ui->_TabImage);
    _TabGroup->addButton(_Ui->_TabDocument);

    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemViewModel = new RaptorSpaceViewModel(_Ui->_ItemView);
    _ItemViewModel->invokeColumnCountSet(4);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
    _Folder = new RaptorFolder(RaptorStoreSuite::invokeWorldGet());
    _Upload = new RaptorUpload(RaptorStoreSuite::invokeWorldGet());
    _Import = new RaptorImport(RaptorStoreSuite::invokeWorldGet());
    _Download = new RaptorDownload(RaptorStoreSuite::invokeWorldGet());
    _Share = new RaptorShare(RaptorStoreSuite::invokeWorldGet());

    _Loading = new RaptorLoading(_Ui->_ItemView);
}

void RaptorSpacePage::invokeUiInit()
{
    _Ui->_Title->setText(QStringLiteral("空间"));
    _Ui->_NewFolder->setText(QStringLiteral("新建文件夹"));
    _Ui->_Upload->setText(QStringLiteral("上传"));
    _Ui->_Import->setText(QStringLiteral("导入分享"));
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabAll->setText(QStringLiteral("所有"));
    _Ui->_TabAll->setChecked(true);
    _Ui->_TabFolder->setText(QStringLiteral("文件夹"));
    _Ui->_TabAudio->setText(QStringLiteral("音频"));
    _Ui->_TabVideo->setText(QStringLiteral("视频"));
    _Ui->_TabImage->setText(QStringLiteral("图像"));
    _Ui->_TabDocument->setText(QStringLiteral("文档"));
    _Ui->_TabNext->setIcon(QIcon(RaptorUtil::invokeIconMatch("Right", false, true)));
    _Ui->_TabNext->setIconSize(QSize(14, 14));
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
    _Ui->_SearchEdit->installEventFilter(this);
    _Ui->_SearchEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_Search->setIcon(QIcon(RaptorUtil::invokeIconMatch("Search", false, true)));
    _Ui->_Search->setIconSize(QSize(16, 16));
    _Ui->_Root->setText(QStringLiteral("根目录"));
    _Ui->_Back->setText(QStringLiteral("后退"));
    _Ui->_Refresh->setText(QStringLiteral("刷新"));
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小") << QStringLiteral("最后修改");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _Ui->_ItemView->installEventFilter(this);
    _Ui->_ItemView->setModel(_ItemViewModel);
    _Ui->_ItemView->setHorizontalHeader(_ItemViewHeader);
    _Ui->_ItemView->setItemDelegate(_ItemViewDelegate);
    _Ui->_ItemView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemView->horizontalHeader()->setMinimumSectionSize(26);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemView->verticalHeader()->setHidden(true);
    _Ui->_ItemView->setShowGrid(false);
    _Ui->_ItemView->setColumnWidth(0, 30);
    _Ui->_ItemView->setColumnWidth(2, 110);
    _Ui->_ItemView->setColumnWidth(3, 80);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);

    _Ui->_ItemNameTip->setText(QStringLiteral("名称 (双击以修改):"));
    _Ui->_ItemName->installEventFilter(this);
    _Ui->_ItemName->viewport()->installEventFilter(this);

    _Ui->_ItemCreatedTip->setText(QStringLiteral("创建时间:"));
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小:"));
    _Ui->_ItemUpdatedTip->setText(QStringLiteral("最后修改:"));
    _Ui->_ItemUrlTip->setText(QStringLiteral("链接 (双击以拷贝):"));
    _Ui->_ItemUrl->setTextInteractionFlags(Qt::NoTextInteraction);
    _Ui->_ItemUrl->viewport()->setCursor(Qt::PointingHandCursor);
    _Ui->_ItemUrl->viewport()->installEventFilter(this);
    _Ui->_ItemRefresh->setText(QStringLiteral("刷新"));
    _Ui->_ItemDownload->setText(QStringLiteral("下载"));
    _Ui->_ItemStar->setText(QStringLiteral("收藏"));
    _Ui->_ItemShare->setText(QStringLiteral("分享"));
    _Ui->_ItemDelete->setText(QStringLiteral("删除"));
}

void RaptorSpacePage::invokeSlotInit() const
{
    connect(_Loading,
            &RaptorLoading::stateChanged,
            this,
            &RaptorSpacePage::onLoadingStateChanged);

    connect(_Ui->_NewFolder,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onNewFolderClicked);

    connect(_Ui->_Upload,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onUploadClicked);

    connect(_Ui->_Import,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onImportClicked);

    connect(_Ui->_TabPrev,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onTabPrevClicked);

    connect(_Ui->_TabAll,
            &QPushButton::toggled,
            this,
            &RaptorSpacePage::onTabAllToggled);

    connect(_Ui->_TabFolder,
            &QPushButton::toggled,
            this,
            &RaptorSpacePage::onTabFolderToggled);

    connect(_Ui->_TabAudio,
            &QPushButton::toggled,
            this,
            &RaptorSpacePage::onTabAudioToggled);

    connect(_Ui->_TabVideo,
            &QPushButton::toggled,
            this,
            &RaptorSpacePage::onTabVideoToggled);

    connect(_Ui->_TabImage,
            &QPushButton::toggled,
            this,
            &RaptorSpacePage::onTabImageToggled);

    connect(_Ui->_TabDocument,
            &QPushButton::toggled,
            this,
            &RaptorSpacePage::onTabDocumentToggled);

    connect(_Ui->_TabNext,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onTabNextClicked);

    connect(_Ui->_SearchEdit,
            &QLineEdit::textChanged,
            this,
            &RaptorSpacePage::onSearchEditTextChanged);

    connect(_Ui->_Search,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onSearchClicked);

    connect(_Ui->_Root,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onRootClicked);

    connect(_Ui->_Back,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onBackClicked);

    connect(_Ui->_Refresh,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onRefreshClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::indicatorClicked,
            this,
            &RaptorSpacePage::onItemViewIndicatorClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::doubleClicked,
            this,
            &RaptorSpacePage::onItemViewDoubleClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::clicked,
            this,
            &RaptorSpacePage::onItemViewClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorSpacePage::onItemViewSelectionChanged);

    connect(_Ui->_ItemView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorSpacePage::onItemViewVerticalScrollValueChanged);

    connect(_Ui->_ItemDownload,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onItemDownloadClicked);

    connect(_Ui->_ItemRefresh,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onItemRefreshClicked);

    connect(_Ui->_ItemStar,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onItemStarClicked);

    connect(_Ui->_ItemShare,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onItemShareClicked);

    connect(_Ui->_ItemDelete,
            &QPushButton::clicked,
            this,
            &RaptorSpacePage::onItemDeleteClicked);
}

void RaptorSpacePage::invokeItemsCopy()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Clipboard._Working)
    {
        RaptorToast::invokeInformationEject(QStringLiteral("已有拷贝/移动任务正在进行，请稍后重试!"));
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    auto items = QVector<RaptorFileItem>();
    for (auto& qIndex : qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        items << item;
    }

    _Clipboard._Items = items;
    _Clipboard._Move = false;
    _Clipboard._From = RaptorStoreSuite::invokeUserGet()._Space;
}

void RaptorSpacePage::invokeItemsPaste()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto [_From, _Items, _Move, _Ing] = _Clipboard;
    if (_Items.isEmpty())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    auto qName = QStringLiteral("根目录");
    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        qIndexList.isEmpty())
    {
        if (!_Payload._Stack.isEmpty())
        {
            const auto item = _Payload._Stack.last();
            input._Id = item._Id;
            qName = item._Name;
        }
        else
        {
            input._Id = "root";
        }
    }
    else if (qIndexList.length() > 1)
    {
        RaptorToast::invokeInformationEject(QStringLiteral("仅支持移动到一个目标文件夹!"));
        return;
    }
    else if (qIndexList.length() == 1)
    {
        const auto qIndex = qIndexList[0];
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        if (_Items.contains(item))
        {
            RaptorToast::invokeInformationEject(QStringLiteral("源文件包含不能目标文件夹，请重新选择!"));
            return;
        }

        if (item._Type == "folder")
        {
            input._Id = item._Id;
            qName = item._Name;
        }
    }

    auto qTitle = QStringLiteral("拷贝");
    if (_Move)
    {
        qTitle = QStringLiteral("移动");
    }

    if (!RaptorMessageBox::invokeInformationEject(QStringLiteral("%1文件").arg(qTitle),
                                                  QStringLiteral("即将%1文件到 %2，是否继续?").arg(qTitle, QStringLiteral(CREATIVE_TEMPLATE).arg(qName))))
    {
        return;
    }

    input._Parent = _Clipboard._From;
    input._Variant = QVariant::fromValue<QVector<RaptorFileItem>>(_Items);
    _Clipboard._Working = true;
    if (_Move)
    {
        Q_EMIT itemsMoving(QVariant::fromValue<RaptorInput>(input));
    }
    else
    {
        Q_EMIT itemsCopying(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorSpacePage::invokeItemsCut()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Clipboard._Working)
    {
        RaptorToast::invokeInformationEject(QStringLiteral("已有拷贝/移动任务正在进行，请稍后重试!"));
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    auto items = QVector<RaptorFileItem>();
    for (auto& qIndex : qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        items << item;
    }

    _Clipboard._Items = items;
    _Clipboard._Move = true;
    _Clipboard._From = RaptorStoreSuite::invokeUserGet()._Space;
}

void RaptorSpacePage::invokeItemsPlay() const
{
    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何文件，无法继续!"));
    }

    for (auto& qIndex : qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        if (item._Type == "folder")
        {
            continue;
        }

        if (item._Url.isEmpty())
        {
            RaptorToast::invokeInformationEject(QStringLiteral("请先刷新 % 的地址，再进行强制播放!").arg(QStringLiteral(WARNING_TEMPLATE).arg(item._Name)));
            continue;
        }

        if (const auto qError = RaptorUtil::invoke3rdPartyPlayerEvoke(item._Url);
            !qError.isEmpty())
        {
            RaptorToast::invokeWarningEject(qError);
        }
    }
}

QPair<QString, QString> RaptorSpacePage::invokeTypeWithCategoryFilter() const
{
    auto qType = QString();
    auto qCategory = QString();
    if (_Ui->_TabFolder->isChecked())
    {
        qType = "folder";
    }
    else
    {
        if (_Ui->_TabAudio->isChecked())
        {
            qCategory = "audio";
        }
        else if (_Ui->_TabVideo->isChecked())
        {
            qCategory = "video";
        }
        else if (_Ui->_TabImage->isChecked())
        {
            qCategory = "image";
        }
        else if (_Ui->_TabDocument->isChecked())
        {
            qCategory = "doc";
        }
    }

    return qMakePair(qType, qCategory);
}

void RaptorSpacePage::onItemCopyWriterHaveFound(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto items = _Data.value<QVector<RaptorCopyWriter>>();
    for (auto& [_Page, _Content] : items)
    {
        if (_Page == metaObject()->className())
        {
            _Ui->_Description->setText(_Content);
            break;
        }
    }
}

void RaptorSpacePage::onItemAccessTokenRefreshed(const QVariant& qVariant)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::Forbidden);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("403 尚未登录"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("去登录"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        return;
    }

    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::InternalError);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("500 服务器错误"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("服务器出错可能说明该雇更多程序员了"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("散财消灾"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _Ui->_ItemView->invokeBackgroundPaintableSet(false);
    _ItemViewModel->invokeItemsClear();
    _Payload._Parent = "root";
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onItemLogouting(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item != RaptorStoreSuite::invokeUserGet())
    {
        return;
    }

    _ItemViewModel->invokeItemsClear();
    _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::Forbidden);
    _Ui->_ItemView->invokeTitleSet(QStringLiteral("403 尚未登录"));
    _Ui->_ItemView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
    _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("去登录"));
    _Ui->_ItemView->invokeBackgroundPaintableSet(true);
    _Ui->_SearchEdit->clear();
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemUrl->clear();
}

void RaptorSpacePage::onItemSpaceChanging()
{
    _Payload._Parent = "root";
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    _Payload._Keyword.clear();
}

void RaptorSpacePage::onItemSwitching(const QVariant& qVariant) const
{
    Q_UNUSED(qVariant)
    _ItemViewModel->invokeItemsClear();
    _Ui->_SearchEdit->clear();
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemUrl->clear();
}

void RaptorSpacePage::onItemsFetched(const QVariant& qVariant)
{
    _Loading->invokeStateSet(RaptorLoading::State::Finished);
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::Forbidden);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("403 尚未登录"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("去登录"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        return;
    }

    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::InternalError);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("500 服务器错误"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("服务器出错可能说明该雇更多程序员了"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("散财消灾"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto [input, items] = _Data.value<QPair<RaptorInput, QVector<RaptorFileItem>>>();
    if (items.isEmpty())
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::NotFound);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("404 资源不存在"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("生活总归带点荒谬"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("找点乐子吧"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        return;
    }

    _Ui->_ItemView->invokeBackgroundPaintableSet(false);
    if (_Payload._Parent != input._Parent && _Payload._Keyword.isEmpty())
    {
        return;
    }

    _Payload._Marker = input._Marker;
    _ItemViewModel->invokeItemsAppend(items);
}

void RaptorSpacePage::onItemUrlFetched(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto [qIndex, item] = _Data.value<QPair<QModelIndex, RaptorFileItem>>();
    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        qIndexList.length() == 1 && item == qIndexList[0].data(Qt::UserRole).value<RaptorFileItem>())
    {
        _Ui->_ItemUrl->setText(item._Url);
    }

    _ItemViewModel->setData(qIndex, QVariant::fromValue<RaptorFileItem>(item), Qt::EditRole);
    RaptorToast::invokeInformationEject(QStringLiteral("%1 的链接已经刷新。").arg(QStringLiteral(CREATIVE_TEMPLATE).arg(item._Name)));
}

void RaptorSpacePage::onItemPreviewPlayInfoFetched(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    if (const auto qError = RaptorUtil::invoke3rdPartyPlayerEvoke(_Data.value<QString>());
        !qError.isEmpty())
    {
        RaptorToast::invokeWarningEject(qError);
    }
}

void RaptorSpacePage::onItemsImported(const QVariant& qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("分享已导入。稍后刷新即可看到!"));
}

void RaptorSpacePage::onItemCreated(const QVariant& qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto [qCount, qParent] = _Data.value<QPair<quint8, QString>>();
    RaptorToast::invokeInformationEject(QStringLiteral("%1 个文件夹已创建。").arg(QStringLiteral(SUCCESS_TEMPLATE).arg(qCount)));
    if (qParent == _Payload._Parent)
    {
        onRefreshClicked();
    }
}

void RaptorSpacePage::onItemRenamed(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto [qIndex, qName] = _Data.value<QPair<QModelIndex, QString>>();
    auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
    item._Name = qName;
    _ItemViewModel->setData(qIndex, QVariant::fromValue<RaptorFileItem>(item), Qt::EditRole);
    _Ui->_ItemView->viewport()->update();
    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        qIndexList.length() == 1 && qIndex.data(Qt::UserRole).value<RaptorFileItem>() == qIndexList[0].data(Qt::UserRole).value<RaptorFileItem>())
    {
        _Ui->_ItemName->setText(qName);
    }

    RaptorToast::invokeInformationEject(QStringLiteral("完成重命名。"));
}

void RaptorSpacePage::onItemsRapidCreated(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    qApp->clipboard()->setText(_Data.value<QString>());
    RaptorToast::invokeSuccessEject(QStringLiteral("快传已生成并已拷贝至剪切板!"));
}

void RaptorSpacePage::onItemsRapidImported(const QVariant& qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("快传已导入。稍后刷新即可看到!"));
}

void RaptorSpacePage::onItemsStarred(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    if (const auto [qIndexList, qState] = _Data.value<QPair<QModelIndexList, bool>>(); qState)
    {
        RaptorToast::invokeSuccessEject(QStringLiteral("所选文件已添加到收藏。"));
    }
}

void RaptorSpacePage::onItemsTrashed(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto qIndexList = _Data.value<QModelIndexList>();
    for (auto i = qIndexList.length() - 1; i >= 0; --i)
    {
        _ItemViewModel->removeRow(qIndexList[i].row());
    }

    _Ui->_ItemView->selectionModel()->clearSelection();
    _Ui->_ItemView->viewport()->update();
    if (_ItemViewModel->rowCount() == 0)
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::NotFound);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("404 资源不存在"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("生活总归带点荒谬"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("找点乐子吧"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("所选文件已被放入回收站。"));
}

void RaptorSpacePage::onItemsCopied(const QVariant& qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    if (const auto qId = _Data.value<QString>();
        qId == _Payload._Parent)
    {
        onRefreshClicked();
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("所选文件已经完成复制。"));
    _Clipboard._Items.clear();
    _Clipboard._Working = false;
}

void RaptorSpacePage::onItemsMoved(const QVariant& qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    if (const auto qId = _Data.value<QString>();
        qId == _Payload._Parent)
    {
        onRefreshClicked();
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("所选文件已经完成移动。"));
    _Clipboard._Items.clear();
    _Clipboard._Working = false;
}

void RaptorSpacePage::onLoadingStateChanged(const RaptorLoading::State& state) const
{
    _Ui->_NewFolder->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Upload->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Import->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabPrev->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabAll->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabFolder->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabAudio->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabVideo->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabImage->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabDocument->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabNext->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_SearchEdit->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Search->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Back->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Root->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Refresh->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemRefresh->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemDownload->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemStar->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemShare->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemDelete->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemUrl->clear();
}

void RaptorSpacePage::onNewFolderClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    auto qParent = QString("root");
    auto qName = QString("根目录");
    if (!_Payload._Stack.isEmpty())
    {
        const auto item = _Payload._Stack.last();
        qParent = item._Id;
        qName = item._Name;
    }

    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        qIndexList.length() == 1)
    {
        if (const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorFileItem>();
            item._Type == "folder")
        {
            qParent = item._Id;
            qName = item._Name;
        }
    }

    _Folder->invokeEject(QVariant::fromValue<QPair<QString, QString>>(qMakePair(qParent, qName)));
}

void RaptorSpacePage::onUploadClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    auto qParent = QString("root");
    auto qName = QString("根目录");
    if (qIndexList.isEmpty())
    {
        if (!_Payload._Stack.isEmpty())
        {
            const auto item = _Payload._Stack.top();
            qParent = item._Id;
            qName = item._Name;
        }
    }
    else if (qIndexList.length() == 1)
    {
        if (const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorFileItem>();
            item._Type == "folder")
        {
            qParent = item._Id;
            qName = item._Name;
        }
    }

    _Upload->invokeEject(QVariant::fromValue<QPair<QString, QString>>(qMakePair(qParent, qName)));
}

void RaptorSpacePage::onImportClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    auto qParent = QString("root");
    auto qName = QString("根目录");
    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        qIndexList.isEmpty())
    {
        if (!_Payload._Stack.isEmpty())
        {
            const auto item = _Payload._Stack.top();
            qParent = item._Id;
            qName = item._Name;
        }
    }
    else if (qIndexList.length() == 1)
    {
        if (const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorFileItem>();
            item._Type == "folder")
        {
            qParent = item._Id;
            qName = item._Name;
        }
    }
    else if (qIndexList.length() > 1)
    {
        RaptorToast::invokeWarningEject(QStringLiteral("仅支持导入到一个目标文件夹!"));
        return;
    }

    _Import->invokeEject(QVariant::fromValue<QPair<QString, QString>>(qMakePair(qParent, qName)));
}

void RaptorSpacePage::onTabPrevClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton*>();
    qPushButtonList.pop_front();
    qPushButtonList.pop_back();
    if (qPushButtonList.length() < 2)
    {
        return;
    }

    for (auto i = 0; i < qPushButtonList.length(); ++i)
    {
        if (qPushButtonList[i]->isChecked())
        {
            if (i == 1)
            {
                qPushButtonList[qPushButtonList.length() - 1]->setChecked(true);
            }
            else
            {
                qPushButtonList[i - 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorSpacePage::onTabAllToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onTabFolderToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    const auto [qType, qCatagory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCatagory;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onTabAudioToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    const auto [qType, qCatagory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCatagory;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onTabVideoToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    const auto [qType, qCatagory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCatagory;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onTabImageToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    const auto [qType, qCatagory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCatagory;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onTabDocumentToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    const auto [qType, qCatagory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCatagory;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onTabNextClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton*>();
    qPushButtonList.pop_front();
    qPushButtonList.pop_back();
    if (qPushButtonList.length() < 2)
    {
        return;
    }

    for (auto i = 0; i < qPushButtonList.length(); ++i)
    {
        if (qPushButtonList[i]->isChecked())
        {
            if (i == qPushButtonList.length() - 1)
            {
                qPushButtonList[0]->setChecked(true);
            }
            else
            {
                qPushButtonList[i + 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorSpacePage::onSearchEditTextChanged(const QString& qText)
{
    _Payload._Keyword = qText;
}

void RaptorSpacePage::onSearchClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    _ItemViewModel->invokeItemsClear();
    _Payload._Parent = "root";
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    if (_Payload._Keyword.isEmpty())
    {
        onRootClicked();
    }
    else
    {
        _Loading->invokeStateSet(RaptorLoading::State::Loading);
        input._Name = _Payload._Keyword;
        Q_EMIT itemsByConditionFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorSpacePage::onRootClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _Payload._Parent = "root";
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    _ItemViewModel->invokeItemsClear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onBackClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!_Payload._Stack.isEmpty())
    {
        _Loading->invokeStateSet(RaptorLoading::State::Loading);
        const auto item = _Payload._Stack.pop();
        _Payload._Parent = item._Parent;
        _Payload._Marker.clear();
        _ItemViewModel->invokeItemsClear();
        auto input = RaptorInput();
        input._Id = item._Id;
        input._Parent = item._Parent;
        const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
        input._Type = qType;
        input._Category = qCategory;
        Q_EMIT itemsByIdFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorSpacePage::onRefreshClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Parent = _Payload._Parent;
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onItemViewIndicatorClicked(const RaptorTableView::Code& qCode) const
{
    if (qCode == RaptorTableView::Forbidden)
    {
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeLoginEject",
                                  Qt::AutoConnection);
    }
}

void RaptorSpacePage::onItemViewDoubleClicked(const QModelIndex& qIndex)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        item._Type == "folder")
    {
        _Loading->invokeStateSet(RaptorLoading::State::Loading);
        _Ui->_Root->setEnabled(true);
        _Ui->_Back->setEnabled(true);
        _Ui->_ItemName->clear();
        _Ui->_ItemCreated->clear();
        _Ui->_ItemSize->clear();
        _Ui->_ItemUpdated->clear();
        _Ui->_ItemUrl->clear();
        _Payload._Parent = item._Id;
        _Payload._Stack.push(item);
        _Payload._Marker.clear();
        _ItemViewModel->invokeItemsClear();
        auto input = RaptorInput();
        input._Parent = item._Id;
        Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
    }
    else
    {
        if (item._Mime.startsWith("video/"))
        {
            if (const auto qQuality = RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                                         Setting::Play::Quality).toString();
                !qQuality.isEmpty())
            {
                auto input = RaptorInput();
                if (qQuality == Setting::Play::Origin)
                {
                    if (item._Url.isEmpty())
                    {
                        RaptorToast::invokeWarningEject(QStringLiteral("请刷新播放地址，然后再次双击以播放。"));
                        return;
                    }

                    if (const auto qError = RaptorUtil::invoke3rdPartyPlayerEvoke(item._Url);
                        !qError.isEmpty())
                    {
                        RaptorToast::invokeWarningEject(qError);
                    }
                }
                else
                {
                    input._Id = qQuality;
                }

                input._Index = qIndex;
                Q_EMIT itemPreviewPlayInfoFetching(QVariant::fromValue<RaptorInput>(input));
            }
        }
        else if (item._Mime.startsWith("audio/"))
        {
            if (item._Url.isEmpty())
            {
                RaptorToast::invokeWarningEject(QStringLiteral("请刷新播放地址，然后再次双击以播放。"));
                return;
            }

            if (const auto qError = RaptorUtil::invoke3rdPartyPlayerEvoke(item._Url);
                !qError.isEmpty())
            {
                RaptorToast::invokeWarningEject(qError);
            }
        }
    }
}

void RaptorSpacePage::onItemViewClicked(const QModelIndex& qIndex) const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemCreated->setText(item._Created);
    _Ui->_ItemSize->setText(item._Size);
    _Ui->_ItemUpdated->setText(item._Updated);
    _Ui->_ItemUrl->setText(item._Url);
}

void RaptorSpacePage::onItemViewSelectionChanged(const QItemSelection& qSelected,
                                                 const QItemSelection& qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (qSelected.length() == 1)
    {
        const auto item = qSelected.indexes()[0].data(Qt::UserRole).value<RaptorFileItem>();
        _Ui->_ItemName->setText(item._Name);
        _Ui->_ItemCreated->setText(item._Created);
        _Ui->_ItemSize->setText(item._Size);
        _Ui->_ItemUpdated->setText(item._Updated);
        _Ui->_ItemUrl->setText(item._Url);
        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemUrl->clear();
}

void RaptorSpacePage::onItemViewVerticalScrollValueChanged(const int& qValue) const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qMarker = _Payload._Marker;
    if (qMarker.isEmpty())
    {
        return;
    }

    if (qValue != _Ui->_ItemView->verticalScrollBar()->maximum())
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    auto input = RaptorInput();
    if (const auto qKeyword = _Ui->_SearchEdit->text();
        !qKeyword.isEmpty())
    {
        const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
        input._Type = qType;
        input._Category = qCategory;
        input._Marker = qMarker;
        input._Name = qKeyword;
        Q_EMIT itemsByConditionFetching(QVariant::fromValue<RaptorInput>(input));
        return;
    }

    input._Parent = _Payload._Parent;
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    input._Marker = qMarker;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onItemDownloadClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    _Download->invokeEject(QVariant::fromValue<QModelIndexList>(qIndexList));
}

void RaptorSpacePage::onItemRefreshClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.length() != 1)
    {
        RaptorToast::invokeInformationEject(QStringLiteral("仅能刷新一个文件的链接!"));
        return;
    }

    if (const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorFileItem>();
        item._Type != "file")
    {
        RaptorToast::invokeInformationEject(QStringLiteral("所选目标不能是文件夹，请重新选择!"));
        return;
    }

    auto input = RaptorInput();
    input._Index = qIndexList[0];
    Q_EMIT itemUrlFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onItemStarClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    input._State = true;
    input._Indexes = qIndexList;
    Q_EMIT itemsStarring(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSpacePage::onItemShareClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    if (RaptorStoreSuite::invokeSpaceGet() == Private)
    {
        if (!RaptorMessageBox::invokeInformationEject(QStringLiteral("快传"),
                                                      QStringLiteral("备份盘无法创建公开分享。是否创建快传链接?")))
        {
            return;
        }

        auto input = RaptorInput();
        input._Indexes = qIndexList;
        Q_EMIT itemsRapidCreating(QVariant::fromValue<RaptorInput>(input));
        return;
    }

    if (RaptorMessageBox::invokeInformationEject(QStringLiteral("快传"),
                                                 QStringLiteral("是否需要创建快传?如果是请点击[确定]，否则创建普通分享")))
    {
        auto input = RaptorInput();
        input._Indexes = qIndexList;
        Q_EMIT itemsRapidCreating(QVariant::fromValue<RaptorInput>(input));
        return;
    }

    _Share->invokeEject(QVariant::fromValue<QModelIndexList>(qIndexList));
}

void RaptorSpacePage::onItemDeleteClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!RaptorMessageBox::invokeWarningEject(QStringLiteral("删除文件"),
                                              QStringLiteral("即将移动所选文件到回收站。是否继续?")))
    {
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    input._Indexes = qIndexList;
    Q_EMIT itemsTrashing(QVariant::fromValue<RaptorInput>(input));
}
