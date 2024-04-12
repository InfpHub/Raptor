//
// Created by Infp on 2024/3/26.
//

#ifndef RAPTORMASK_H
#define RAPTORMASK_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class RaptorMask; }
QT_END_NAMESPACE

class RaptorMask : public QWidget {
Q_OBJECT

public:
    explicit RaptorMask(QWidget *parent = nullptr);
    ~RaptorMask() override;

private:
    Ui::RaptorMask *ui;
};


#endif // RAPTORMASK_H
