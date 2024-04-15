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

#include "RaptorImport.h"
#include "ui_RaptorImport.h"

RaptorImport::RaptorImport(QWidget *qParent) : RaptorEject(qParent),
                                               _Ui(new Ui::RaptorImport)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorImport::~RaptorImport()
{
    qFree(_Ui)
}

bool RaptorImport::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == this)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent *>(qEvent);
                qKeyEvent->key() == Qt::Key_Escape)
            {
                onCloseClicked();
                return true;
            }
        }
    }

    if (qObject == _Ui->_Icon)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeIconDrawing();
            return true;
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

void RaptorImport::invokeEject(const QVariant &qVariant)
{
    _Variant = qVariant;
    _Ui->_Title->setText(
        QStringLiteral("导入分享到 %1").arg(
            QString(qCreativeTemplate).arg(_Variant.value<QPair<QString, QString> >().second)));
    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Finished);
    _ItemViewModel->invokeItemsClear();
    _Ui->_Link->clear();
    _Ui->_Password->clear();
    _Ui->_Many->setChecked(false);
    _Ui->_One->setChecked(false);
    _Ui->_Body->setCurrentWidget(_Ui->_LinkWrapper);
    RaptorEject::invokeEject(qVariant);
}

void RaptorImport::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _TypeGroup = new QButtonGroup(this);
    _TypeGroup->setExclusive(true);
    _TypeGroup->addButton(_Ui->_Many);
    _TypeGroup->addButton(_Ui->_One);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));

    _ItemViewModel = new RaptorSpaceViewModel(_Ui->_ItemView);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(3);

    _ItemViewLoading = new RaptorLoading(_Ui->_ItemView);
}

void RaptorImport::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_LinkTip->setText(QStringLiteral("链接:"));
    _Ui->_Link->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_PasswordTip->setText(QStringLiteral("提取码:"));
    _Ui->_Password->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_PasswordPanelTip->setText(QString(qInformationTemplate).arg(QStringLiteral("如无提取码或快传则无需填写")));
    _Ui->_TypeTip->setText(QStringLiteral("类型:"));
    _Ui->_Many->setText(QStringLiteral("分享"));
    _Ui->_One->setText(QStringLiteral("快传"));
    _Ui->_Icon->installEventFilter(this);
    _Ui->_Parse->setText(QStringLiteral("解析"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
    _Ui->_ItemView->setModel(_ItemViewModel);
    _Ui->_ItemView->setHorizontalHeader(_ItemViewHeader);
    _Ui->_ItemView->setItemDelegate(_ItemViewDelegate);
    _Ui->_ItemView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemView->verticalHeader()->setHidden(true);
    _Ui->_ItemView->setShowGrid(false);
    _Ui->_ItemView->setColumnWidth(0, 30);
    _Ui->_ItemView->setColumnWidth(2, 110);
    _Ui->_ItemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemNameTip->setText(QStringLiteral("名称"));
    _Ui->_ItemName->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小"));
    _Ui->_ItemRoot->setText(QStringLiteral("根目录"));
    _Ui->_ItemBack->setText(QStringLiteral("后退"));
    _Ui->_Import->setText(QStringLiteral("导入"));
}

void RaptorImport::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_ItemViewLoading,
            &RaptorLoading::stateChanged,
            this,
            &RaptorImport::onItemViewLoadingStateChanged);

    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorImport::onCloseClicked);

    connect(_Ui->_One,
            &QCheckBox::stateChanged,
            this,
            &RaptorImport::onOneStateChanged);

    connect(_Ui->_Parse,
            &QPushButton::clicked,
            this,
            &RaptorImport::onParseClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorImport::onCancelClicked);

    connect(_Ui->_ItemView,
            &QTableView::clicked,
            this,
            &RaptorImport::onItemViewClicked);

    connect(_Ui->_ItemView,
            &QTableView::doubleClicked,
            this,
            &RaptorImport::onItemViewDoubleClicked);

    connect(_Ui->_ItemRoot,
            &QPushButton::clicked,
            this,
            &RaptorImport::onItemRootClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorImport::onItemViewSelectionChanged);

    connect(_Ui->_ItemBack,
            &QPushButton::clicked,
            this,
            &RaptorImport::onItemBackClicked);

    connect(_Ui->_Import,
            &QPushButton::clicked,
            this,
            &RaptorImport::onImportClicked);
}

void RaptorImport::invokeIconDrawing() const
{
    auto qPainter = QPainter(_Ui->_Icon);
    _SvgRender->load(RaptorUtil::invokeIconMatch("Share", false, true));
    if (_SvgRender->isValid())
    {
        _SvgRender->render(&qPainter, QRect(0, 0, _Ui->_Icon->width(), _Ui->_Icon->height()));
    }
}

void RaptorImport::onItemParsed(const QVariant &qVariant)
{
    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Finished);
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto [input, items] = _Data.value<QPair<RaptorInput, QVector<RaptorFileItem> > >();
    if (_Payload._Parent != input._Parent)
    {
        return;
    }

    _Payload._Marker = input._Marker;
    _Payload._Token = input._Token;
    _ItemViewModel->invokeItemsAppend(items);
    _Ui->_Body->setCurrentWidget(_Ui->_ItemWrapper);
}

void RaptorImport::onItemViewLoadingStateChanged(const RaptorLoading::State &state) const
{
    _Ui->_Import->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemName->clear();
    _Ui->_ItemSize->clear();
}

void RaptorImport::onCloseClicked()
{
    close();
}

void RaptorImport::onOneStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _Ui->_Parse->setText(QStringLiteral("导入"));
    } else if (qState == Qt::Unchecked)
    {
        _Ui->_Parse->setText(QStringLiteral("解析"));
    }
}

void RaptorImport::onParseClicked()
{
    if (!_Ui->_Many->isChecked() && !_Ui->_One->isChecked())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("请选择类型!"));
        return;
    }

    auto qLink = _Ui->_Link->toPlainText();
    if (qLink.isEmpty())
    {
        RaptorToast::invokeCriticalEject(QStringLiteral("链接不能为空!"));
        return;
    }

    auto input = RaptorInput();
    input._Id = _Variant.value<QPair<QString, QString> >().first;
    if (_Ui->_Many->isChecked())
    {
        // 链接：https://www.alipan.com/s/6tZVWTj5fUz
        // 链接：https://www.alipan.com/s/6tZVWTj5fUz
        // 提取码: h7p7
        const auto qLinkRegularExpressionMatch = QRegularExpression(
            "https://www\\.(aliyundrive|alipan)\\.com/s/[a-zA-Z0-9]{11}").match(qLink);
        if (!qLinkRegularExpressionMatch.hasMatch())
        {
            RaptorToast::invokeCriticalEject(QStringLiteral("非法链接!"));
            return;
        }

        if (const auto qIdRegularExpressionMatch = QRegularExpression(
                "[0-9a-z]{40}").match(qLink);
            qIdRegularExpressionMatch.hasMatch())
        {
            _Payload._Parent = qIdRegularExpressionMatch.captured(0);
            input._Parent = _Payload._Parent;
        }

        qLink = qLinkRegularExpressionMatch.captured(0);
        _Ui->_Link->setText(qLink);
        input._Link = qLink;
        input._Password = _Ui->_Password->text();
        Q_EMIT itemParsing(QVariant::fromValue<RaptorInput>(input));
        _Ui->_Parse->setEnabled(false);
    } else if (_Ui->_One->isChecked())
    {
        // https://www.alipan.com/t/0Pcs5mqiU2mW2WV9PPfy
        if (qLink.length() != 45 && qLink.left(27) != "https://www.alipan.com/s/" &&
            qLink.length() != 40 && qLink.left(22) != "https://www.alipan.com/s/")
        {
            RaptorToast::invokeCriticalEject(QStringLiteral("非法链接!"));
            return;
        }

        Q_EMIT itemRapidImporting(QVariant::fromValue<RaptorInput>(input));
        close();
    }
}

void RaptorImport::onCancelClicked()
{
    onCloseClicked();
}

void RaptorImport::onItemViewClicked(const QModelIndex &qIndex) const
{
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemSize->setText(item._Size);
}

void RaptorImport::onItemViewDoubleClicked(const QModelIndex &qIndex)
{
    if (!qIndex.isValid())
    {
        return;
    }

    if (const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        item._Type == "folder")
    {
        _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _Payload._Parent = item._Id;
        _Payload._Stack.push(item);
        _Payload._Marker.clear();
        _ItemViewModel->invokeItemsClear();
        auto input = RaptorInput();
        input._Id = _Ui->_Link->toPlainText().right(11);
        input._Parent = item._Id;
        input._Token = _Payload._Token;
        Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorImport::onItemViewSelectionChanged(const QItemSelection &qSelected,
                                              const QItemSelection &qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (qSelected.length() == 1)
    {
        const auto item = qSelected.indexes()[0].data(Qt::UserRole).value<RaptorFileItem>();
        _Ui->_ItemName->setText(item._Name);
        _Ui->_ItemSize->setText(item._Size);
        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemSize->clear();
}

void RaptorImport::onItemRootClicked()
{
    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Parent = "root";
    _Payload._Stack.clear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Id = _Ui->_Link->toPlainText().right(11);
    input._Parent = _Payload._Parent;
    input._Token = _Payload._Token;
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorImport::onItemBackClicked()
{
    if (!_Payload._Stack.isEmpty())
    {
        _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _ItemViewModel->invokeItemsClear();
        const auto item = _Payload._Stack.pop();
        if (_Payload._Stack.isEmpty())
        {
            _Ui->_ItemBack->setEnabled(false);
        }

        _Payload._Parent = item._Parent;
        _Payload._Marker.clear();
        auto input = RaptorInput();
        input._Id = _Ui->_Link->toPlainText().right(11);
        input._Parent = item._Parent;
        input._Token = _Payload._Token;
        Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorImport::onImportClicked()
{
    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    input._Indexes = qIndexList;
    input._Id = _Variant.value<QPair<QString, QString> >().first;
    input._Link = _Ui->_Link->toPlainText();
    input._Token = _Payload._Token;
    Q_EMIT itemImporting(QVariant::fromValue<RaptorInput>(input));
    close();
}
