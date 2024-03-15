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

#ifndef RAPTORSPACEPAGE_H
#define RAPTORSPACEPAGE_H

#include <QButtonGroup>
#include <QClipboard>
#include <QEventLoop>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStack>
#include <QTimer>
#include <QWidget>

#include "../../Eject/Download/RaptorDownload.h"
#include "../../Eject/Folder/RaptorFolder.h"
#include "../../Eject/Import/RaptorImport.h"
#include "../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../Eject/Rename/RaptorRename.h"
#include "../../Eject/Share/RaptorShare.h"
#include "../../Eject/Toast/RaptorToast.h"
#include "../../Eject/Upload/RaptorUpload.h"
#include "../../../Component/Loading/RaptorLoading.h"
#include "../../../Component/Menu/RaptorMenu.h"
#include "../../../Component/TableView/RaptorTableView.h"
#include "../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../Model/Page/Space/RaptorSpaceViewModel.h"
#include "../../../Suite/Setting/RaptorSettingSuite.h"
#include "../../../Suite/Store/RaptorStoreSuite.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorSpacePage;
}

QT_END_NAMESPACE

class RaptorSpacePage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorSpacePage(QWidget *qParent = Q_NULLPTR);

    ~RaptorSpacePage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject *qObject, QEvent *qEvent) Q_DECL_OVERRIDE;

    [[nodiscard]]
    RaptorFolder *invokeFolderUiGet() const;

    [[nodiscard]]
    RaptorUpload *invokeUploadUiGet() const;

    [[nodiscard]]
    RaptorImport *invokeImportUiGet() const;

    [[nodiscard]]
    RaptorDownload *invokeDownloadUiGet() const;

    [[nodiscard]]
    RaptorShare *invokeShareUiGet() const;

    [[nodiscard]]
    RaptorRename *invokeRenameUiGet() const;

    void invokeNavigate();

private:
    struct Payload
    {
        QString _Parent = "root";
        QStack<RaptorFileItem> _Stack;
        QString _Marker;
        QString _Keyword;
    };

    struct Clipboard
    {
        QString _From;
        QVector<RaptorFileItem> _Items;
        bool _Move = false;
        bool _Working = false;
    };

    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

    void invokeItemsCopy();

    void invokeItemsPaste();

    void invokeItemsCut();

    void invokeItemsPlay() const;

    void invokeItemsRename() const;

    [[nodiscard]]
    QPair<QString, QString> invokeTypeWithCategoryFilter() const;

    [[nodiscard]]
    QString invokeVideoSubFilter(const QString &qName) const;

    Q_INVOKABLE void invokeGoToDir(const QString& qParent);

Q_SIGNALS:
    Q_SIGNAL void itemsByParentIdFetching(const QVariant &qVariant) const;

    Q_SIGNAL void itemsByIdFetching(const QVariant &qVariant) const;

    Q_SIGNAL void itemsByConditionFetching(const QVariant &qVariant) const;

    Q_SIGNAL void itemUrlFetching(const QVariant &qVariant) const;

    Q_SIGNAL void itemsRenaming(const QVariant &qVariant) const;

    Q_SIGNAL void itemsRapidCreating(const QVariant &qVariant) const;

    Q_SIGNAL void itemsStarring(const QVariant &qVariant) const;

    Q_SIGNAL void itemsCopying(const QVariant &qVariant) const;

    Q_SIGNAL void itemsMoving(const QVariant &qVariant) const;

    Q_SIGNAL void itemsTrashing(const QVariant &qVariant) const;

    Q_SIGNAL void itemVideoPreviewPlayInfoFetching(const QVariant &qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemCopyWriterHaveFound(const QVariant &qVariant) const;

    Q_SLOT void onItemAccessTokenRefreshed(const QVariant &qVariant);

    Q_SLOT void onItemLogouting(const QVariant &qVariant) const;

    Q_SLOT void onItemSpaceChanging();

    Q_SLOT void onItemSwitching(const QVariant &qVariant) const;

    Q_SLOT void onItemsFetched(const QVariant &qVariant);

    Q_SLOT void onItemUrlFetched(const QVariant &qVariant) const;

    Q_SLOT void onItemVideoPreviewPlayInfoFetched(const QVariant &qVariant) const;

    Q_SLOT void onItemsImported(const QVariant &qVariant) const;

    Q_SLOT void onItemCreated(const QVariant &qVariant);

    Q_SLOT void onItemsRenamed(const QVariant &qVariant) const;

    Q_SLOT void onItemsRapidCreated(const QVariant &qVariant) const;

    Q_SLOT void onItemsRapidImported(const QVariant &qVariant) const;

    Q_SLOT void onItemsStarred(const QVariant &qVariant) const;

    Q_SLOT void onItemsTrashed(const QVariant &qVariant) const;

    Q_SLOT void onItemsCopied(const QVariant &qVariant);

    Q_SLOT void onItemsMoved(const QVariant &qVariant);

private Q_SLOTS:
    Q_SLOT void onLoadingStateChanged(const RaptorLoading::State &state) const;

    Q_SLOT void onNewFolderClicked() const;

    Q_SLOT void onUploadClicked() const;

    Q_SLOT void onImportClicked() const;

    Q_SLOT void onTabPrevClicked() const;

    Q_SLOT void onTabAllToggled(const bool &qChecked);

    Q_SLOT void onTabFolderToggled(const bool &qChecked);

    Q_SLOT void onTabAudioToggled(const bool &qChecked);

    Q_SLOT void onTabVideoToggled(const bool &qChecked);

    Q_SLOT void onTabImageToggled(const bool &qChecked);

    Q_SLOT void onTabDocumentToggled(const bool &qChecked);

    Q_SLOT void onTabNextClicked() const;

    Q_SLOT void onSearchEditTextChanged(const QString &qText);

    Q_SLOT void onSearchClicked();

    Q_SLOT void onRootClicked();

    Q_SLOT void onBackClicked();

    Q_SLOT void onRefreshClicked();

    Q_SLOT void onItemViewIndicatorClicked(const RaptorTableView::Code &qCode) const;

    Q_SLOT void onItemViewDoubleClicked(const QModelIndex &qIndex);

    Q_SLOT void onItemViewClicked(const QModelIndex &qIndex) const;

    Q_SLOT void onItemViewCustomContextMenuRequested(const QPoint &qPoint) const;

    Q_SLOT void onItemViewSelectionChanged(const QItemSelection &qSelected,
                                           const QItemSelection &qDeselected) const;

    Q_SLOT void onItemViewVerticalScrollValueChanged(const int &qValue) const;

    Q_SLOT void onItemDownloadClicked() const;

    Q_SLOT void onItemRefreshClicked() const;

    Q_SLOT void onItemStarClicked() const;

    Q_SLOT void onItemShareClicked() const;

    Q_SLOT void onItemDeleteClicked() const;

private:
    Ui::RaptorSpacePage *_Ui = Q_NULLPTR;
    QButtonGroup *_TabGroup = Q_NULLPTR;
    Clipboard _Clipboard;
    RaptorMenu *_ItemViewContextMenu = Q_NULLPTR;
    RaptorTableViewHeader *_ItemViewHeader = Q_NULLPTR;
    RaptorSpaceViewModel *_ItemViewModel = Q_NULLPTR;
    RaptorTableViewDelegate *_ItemViewDelegate = Q_NULLPTR;
    RaptorFolder *_Folder = Q_NULLPTR;
    RaptorUpload *_Upload = Q_NULLPTR;
    RaptorImport *_Import = Q_NULLPTR;
    RaptorDownload *_Download = Q_NULLPTR;
    RaptorRename* _Rename = Q_NULLPTR;
    RaptorShare *_Share = Q_NULLPTR;
    RaptorLoading *_Loading = Q_NULLPTR;
    Payload _Payload;
};

#endif // RAPTORSPACEPAGE_H
