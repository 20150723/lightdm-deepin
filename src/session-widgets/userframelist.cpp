/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lixin <lixin_cm@deepin.com>
 *
 * Maintainer: lixin <lixin_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "userframelist.h"
#include "sessionbasemodel.h"
#include "userinfo.h"
#include "userloginwidget.h"

#include "dflowlayout.h"
#include "framedatabind.h"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QMouseEvent>
#include <QScrollBar>

const int UserFrameHeight = 174;
const int UserFrameWidth = 226;
const int UserFrameSpaceing = 40;

UserFrameList::UserFrameList(QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(new QScrollArea(this))
    , m_frameDataBind(FrameDataBind::Instance())
{
    initUI();
}

//设置SessionBaseModel，创建用户列表窗体
void UserFrameList::setModel(SessionBaseModel *model)
{
    m_model = model;

    connect(model, &SessionBaseModel::onUserAdded, this, &UserFrameList::addUser);
    connect(model, &SessionBaseModel::onUserRemoved, this, &UserFrameList::removeUser);

    QList<std::shared_ptr<User>> userList = m_model->userList();
    for (auto user : userList) {
        addUser(user);
    }
}

//创建用户窗体
void UserFrameList::addUser(std::shared_ptr<User> user)
{
    UserLoginWidget *widget = new UserLoginWidget(this);
    widget->setFixedSize(QSize(UserFrameWidth, UserFrameHeight));
    connect(widget, &UserLoginWidget::clicked, this, &UserFrameList::onUserClicked);
    widget->setUserAvatarSize(UserLoginWidget::AvatarSmallSize);
    widget->setAvatar(user->avatarPath());
    widget->setName(user->displayName());
    widget->setIsLogin(user->isLogin());
    widget->setSelected(m_model->currentUser()->uid() == user->uid());

    if (user->isLogin()) {
        widget->setWidgetShowType(UserLoginWidget::UserFrameLoginType);
    } else {
        widget->setWidgetShowType(UserLoginWidget::UserFrameType);
    }

    m_userLoginWidgets[user->uid()] = widget;
    m_folwLayout->addWidget(widget);
}

//删除用户
void UserFrameList::removeUser(const uint uid)
{
    UserLoginWidget *widget = m_userLoginWidgets[uid];
    m_userLoginWidgets.remove(uid);
    widget->deleteLater();
}

//点击用户
void UserFrameList::onUserClicked()
{
    UserLoginWidget *widget = static_cast<UserLoginWidget *>(sender());
    if (!widget) return;

    for (UserLoginWidget *tempWidget : m_userLoginWidgets) {
        tempWidget->setSelected(false);
    }
    widget->setSelected(true);

    emit requestSwitchUser(m_model->findUserByUid(m_userLoginWidgets.key(widget)));

    emit clicked();
}


void UserFrameList::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    //处理窗体数量小于5个时的居中显示，取 窗体数量*窗体宽度 和 最大宽度 的较小值，设置为m_centerWidget的宽度
    int maxWidth = this->width();
    int maxHeight = this->height();
    m_colCount = maxWidth / (UserFrameWidth + UserFrameSpaceing);
    m_colCount = m_colCount > 5 ? 5 : m_colCount;
    m_rowCount = maxHeight / (UserFrameHeight + UserFrameSpaceing);
    m_rowCount = m_rowCount > 2 ? 2 : m_rowCount;

    m_scrollArea->setFixedHeight((UserFrameHeight + UserFrameSpaceing) * m_rowCount);
    int width = qMin((UserFrameWidth + UserFrameSpaceing) * m_colCount, m_userLoginWidgets.size() * (UserFrameWidth + UserFrameSpaceing));
    m_centerWidget->setFixedWidth(width);
    m_scrollArea->setFixedWidth(width + 10);
}

void UserFrameList::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked();
    hide();

    return QWidget::mouseReleaseEvent(event);
}

void UserFrameList::hideEvent(QHideEvent *event)
{
    releaseKeyboard();

    return QWidget::hideEvent(event);
}

void UserFrameList::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        switchPreviousUser();
        break;
    case Qt::Key_Right:
        switchNextUser();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        for (auto it = m_userLoginWidgets.constBegin(); it != m_userLoginWidgets.constEnd(); ++it) {
            if (it.value()->getSelected()) {
                emit it.value()->clicked();
                break;
            }
        }
        break;
    case Qt::Key_Escape:
        emit clicked();
        break;
    default:
        break;
    }
    return QWidget::keyPressEvent(event);
}

void UserFrameList::initUI()
{
    setFocusPolicy(Qt::NoFocus);
    m_colCount = 5;
    m_rowCount = 2;

    std::function<void (QVariant)> function = std::bind(&UserFrameList::onOtherPageChanged, this, std::placeholders::_1);
    int index = m_frameDataBind->registerFunction("UserFrameList", function);

    connect(this, &UserFrameList::destroyed, this, [ = ] {
        m_frameDataBind->unRegisterFunction("UserFrameList", index);
    });

    m_folwLayout = new DFlowLayout;
    m_folwLayout->setFlow(QListView::LeftToRight);
    m_folwLayout->setMargin(0);
    m_folwLayout->setSpacing(UserFrameSpaceing);

    m_centerWidget = new QWidget;
    m_centerWidget->setLayout(m_folwLayout);

    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //m_scrollArea纵向滑动条在需要时显示，横向不显示
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_centerWidget);

    QVBoxLayout *mainLayout;
    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_scrollArea, 0, Qt::AlignCenter);
    setLayout(mainLayout);
}

//切换下一个用户
void UserFrameList::switchNextUser()
{
    QList<UserLoginWidget *> widgets = m_userLoginWidgets.values();

    for (int i = 0; i != widgets.size(); ++i) {
        if (widgets[i]->getSelected()) {
            widgets[i]->setSelected(false);
            if (i == (widgets.size() - 1)) {
                widgets.first()->setSelected(true);
                //处理m_scrollArea翻页显示
                m_scrollArea->verticalScrollBar()->setValue(0);
                m_frameDataBind->updateValue("UserFrameList", m_userLoginWidgets.key(m_userLoginWidgets.first()));
            } else {
                //处理m_scrollArea翻页显示
                int selectedRight = widgets[i]->geometry().right();
                int scrollRight = m_scrollArea->widget()->geometry().right();
                if (selectedRight + UserFrameSpaceing == scrollRight) {
                    QPoint topLeft;
                    if(m_rowCount == 1) {
                        topLeft = widgets[i + 1]->geometry().topLeft();
                    } else {
                        topLeft = widgets[i]->geometry().topLeft();
                    }
                    m_scrollArea->verticalScrollBar()->setValue(topLeft.y());
                }
                widgets[i + 1]->setSelected(true);
                m_frameDataBind->updateValue("UserFrameList", m_userLoginWidgets.key(widgets[i + 1]));
            }
            break;
        }
    }
}

//切换上一个用户
void UserFrameList::switchPreviousUser()
{
    QList<UserLoginWidget *> widgets = m_userLoginWidgets.values();

    for (int i = 0; i != widgets.size(); ++i) {
        if (widgets[i]->getSelected()) {
            widgets[i]->setSelected(false);
            if (i == 0) {
                widgets.last()->setSelected(true);
                //处理m_scrollArea翻页显示
                m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
                m_frameDataBind->updateValue("UserFrameList", m_userLoginWidgets.key(m_userLoginWidgets.last()));
            } else {
                //处理m_scrollArea翻页显示
                QPoint topLeft = widgets[i]->geometry().topLeft();
                if (topLeft.x() == 0) {
                    m_scrollArea->verticalScrollBar()->setValue(widgets[i - 1]->geometry().topLeft().y());
                }
                widgets[i - 1]->setSelected(true);
                m_frameDataBind->updateValue("UserFrameList", m_userLoginWidgets.key(widgets[i - 1]));
            }
            break;
        }
    }
}

void UserFrameList::onOtherPageChanged(const QVariant &value)
{
    QList<UserLoginWidget *> widgets = m_userLoginWidgets.values();

    for (UserLoginWidget *widget : widgets) {
        widget->setSelected(false);
    }
    m_userLoginWidgets[value.toUInt()]->setSelected(true);
}
