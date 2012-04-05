/* ============================================================
* SuplChecker - simple program to check a teacher's absencies at school
* Copyright (C) 2010-2012  David Rosca <david@rosca.cz>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "toolbar.h"
#include "suplchecker.h"

#include <QMouseEvent>

ToolBar::ToolBar(SuplChecker* parent)
    : QToolBar(parent)
    , m_mainWindow(parent)
{
}

void ToolBar::mousePressEvent(QMouseEvent* e)
{
    m_dragPosition = QPoint();

    QToolButton* button = qobject_cast<QToolButton*>(widgetForAction(actionAt(e->pos())));

    if (e->button() == Qt::LeftButton && !button) {
        m_dragPosition = e->globalPos() - m_mainWindow->frameGeometry().topLeft();
        e->accept();
    }

    QToolBar::mousePressEvent(e);
}

void ToolBar::mouseReleaseEvent(QMouseEvent* e)
{
    m_dragPosition = QPoint();

    QToolBar::mouseReleaseEvent(e);
}

void ToolBar::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton && !m_dragPosition.isNull()) {
        m_mainWindow->move(e->globalPos() - m_dragPosition);
        e->accept();
    }

    QToolBar::mouseMoveEvent(e);
}
