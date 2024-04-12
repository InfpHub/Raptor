//
// Created by Infp on 2024/3/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_RaptorMask.h" resolved

#include "RaptorMask.h"
#include "ui_RaptorMask.h"


RaptorMask::RaptorMask(QWidget *parent) :
    QWidget(parent), ui(new Ui::RaptorMask) {
    ui->setupUi(this);
}

RaptorMask::~RaptorMask() {
    delete ui;
}
