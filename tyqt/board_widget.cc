/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QPainter>

#include "ty.h"
#include "board.hh"
#include "board_widget.hh"

using namespace std;

BoardWidget::BoardWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

void BoardWidget::setModel(const QString &model)
{
    modelLabel->setText(model);
}

void BoardWidget::setCapabilities(const QString &capabilities)
{
    capabilityLabel->setText(capabilities);
}

void BoardWidget::setTag(const QString &tag)
{
    tagLabel->setText(tag);
}

void BoardWidget::setAvailable(bool available)
{
    boardIcon->setEnabled(available);
}

void BoardWidget::setProgress(unsigned int progress, unsigned int total)
{
    if (total) {
        stackedWidget->setCurrentIndex(1);

        taskProgress->setRange(0, total);
        taskProgress->setValue(progress);
    } else {
        stackedWidget->setCurrentIndex(0);
    }
}

QString BoardWidget::model() const
{
    return modelLabel->text();
}

QString BoardWidget::capabilities() const
{
    return capabilityLabel->text();
}

QString BoardWidget::tag() const
{
    return tagLabel->text();
}

bool BoardWidget::available() const
{
    return boardIcon->isEnabled();
}

BoardItemDelegate::BoardItemDelegate(Manager *model)
    : QItemDelegate(model), model_(model)
{
}

void BoardItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.row() >= static_cast<int>(model_->boardCount()))
        return;

    auto board = model_->board(index.row());

    widget_.resize(option.rect.size());

    widget_.setAvailable(board->state() == TYB_BOARD_STATE_ONLINE);

    widget_.setModel(board->modelName());
    widget_.setCapabilities(Board::makeCapabilityString(board->capabilities(), tr("(none)")));
    widget_.setTag(board->tag());

    auto task = board->runningTask();
    if (task.status() == TY_TASK_STATUS_RUNNING) {
        widget_.setProgress(task.progress(), task.progressMaximum());
    } else {
        widget_.setProgress(0, 0);
    }

    QPalette pal = option.palette;
    if (option.state & QStyle::State_Selected) {
        pal.setBrush(QPalette::Window, option.palette.brush(QPalette::Highlight));
        pal.setColor(QPalette::WindowText, option.palette.color(QPalette::HighlightedText));
    } else {
        pal.setBrush(QPalette::Window, QBrush(QColor(Qt::transparent)));
    }
    widget_.setPalette(pal);

    painter->save();
    painter->translate(option.rect.topLeft());

    widget_.render(painter);

    painter->restore();
}

QSize BoardItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    TY_UNUSED(option);
    TY_UNUSED(index);

    return QSize(widget_.minimumWidth(), widget_.height());
}