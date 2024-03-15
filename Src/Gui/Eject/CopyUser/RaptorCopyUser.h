/**
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

#ifndef RAPTORCOPYUSER_H
#define RAPTORCOPYUSER_H

#include "../Toast/RaptorToast.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Delegate/Eject/User/RaptorUserViewDelegate.h"
#include "../../../Header/Eject/User/RaptorUserViewHeader.h"
#include "../../../Model/Eject/User/RaptorUserViewModel.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorCopyUser;
}

QT_END_NAMESPACE

class RaptorCopyUser Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorCopyUser(QWidget* qParent = Q_NULLPTR);

    ~RaptorCopyUser() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> invokeEject();

private:
    void invokeInstanceInit() Q_DECL_OVERRIDE;

    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

public Q_SLOTS:
    Q_SLOT void onItemsLoaded(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onCloseClicked();

    Q_SLOT void onSourceViewClicked(const QModelIndex& qIndex);

    Q_SLOT void onTargetViewClicked(const QModelIndex& qIndex);

    Q_SLOT void onOkClicked();

    Q_SLOT void onCancelClicked();

private:
    Ui::RaptorCopyUser* _Ui = Q_NULLPTR;
    QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> _User;
    RaptorUserViewHeader* _SourceViewHeader = Q_NULLPTR;
    RaptorUserViewModel* _SourceViewModel = Q_NULLPTR;
    RaptorUserViewHeader* _TargetViewHeader = Q_NULLPTR;
    RaptorUserViewModel* _TargetViewModel = Q_NULLPTR;
    RaptorUserViewDelegate* _SourceViewDelegate = Q_NULLPTR;
    RaptorUserViewDelegate* _TargetViewDelegate = Q_NULLPTR;
};

#endif // RAPTORCOPYUSER_H