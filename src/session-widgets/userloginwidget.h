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

#ifndef USERLOGINWIDGET_H
#define USERLOGINWIDGET_H

#include "sessionbasemodel.h"

#include <darrowrectangle.h>
#include <DBlurEffectWidget>
#include <DFloatingButton>

DWIDGET_USE_NAMESPACE

class QLabel;
class QPushButton;
class User;
class UserAvatar;
class LoginButton;
class LockPasswordWidget;
class FrameDataBind;
class OtherUserInput;
class QVBoxLayout;
class KbLayoutWidget;
class KeyboardMonitor;
class DPasswordEditEx;

class UserLoginWidget : public QWidget
{
    Q_OBJECT
public:
    enum WidgetShowType {
        NoPasswordType,         //无密码登录显示模式
        NormalType,             //正常显示模式-密码登录
        IDAndPasswordType,      //ID和密码登录显示模式
        UserFrameType,          //用户列表显示模式
        UserFrameLoginType      //用户列表,已登录
    };

    enum AvatarSize {
        AvatarSmallSize = 80,
        AvatarNormalSize = 90,
        AvatarLargeSize = 100
    };

    explicit UserLoginWidget(QWidget *parent = nullptr);
    ~UserLoginWidget() override;
    void resetAllState();
    void disablePassword(bool disable, uint lockNum = 0);
    void setFaildMessage(const QString &message);
    void setFaildTipMessage(const QString &message);
    void setWidgetShowType(WidgetShowType showType);
    void setName(const QString &name);
    void setAvatar(const QString &avatar);
    void setUserAvatarSize(const AvatarSize &avatarSize);
    void setWidgetWidth(int width);
    void setIsLogin(bool isLogin);
    bool getIsLogin();
    void setSelected(bool isSelected);
    bool getSelected();
    void updateKBLayout(const QStringList &list);
    void setDefaultKBLayout(const QString &layout);
    void hideKBLayout();
    void setKBLayoutList(QStringList kbLayoutList);
    void setDefKBLayout(QString defKBLayout);

signals:
    void requestAuthUser(const QString &password);
    void clicked();
    void requestUserKBLayoutChanged(const QString &layout);

public slots:
    void updateAuthType(SessionBaseModel::AuthType type);
    void refreshBlurEffectPosition();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();
    void initConnect();
    void updateUI();
    void onOtherPagePasswordChanged(const QVariant &value);
    void onOtherPageKBLayoutChanged(const QVariant &value);
    void toggleKBLayoutWidget();
    void refreshKBLayoutWidgetPosition();
    void capslockStatusChanged(bool on);
    void receiveUserKBLayoutChanged(const QString &layout);
    QImage generateImageFromString(const QString &name);

private:
    DBlurEffectWidget *m_blurEffectWidget;         //阴影窗体
    UserAvatar *m_userAvatar;                      //用户头像
    QLabel *m_nameLbl;                             //用户名
    DPasswordEditEx *m_passwordEdit;               //密码输入框
    LockPasswordWidget *m_lockPasswordWidget;      //密码锁定后,错误信息提示框
    QMap<uint, QString> m_passwords;
    SessionBaseModel::AuthType m_authType;         //认证类型
    OtherUserInput *m_otherUserInput;
    DFloatingButton *m_lockButton;                 //解锁按钮
    DArrowRectangle *m_kbLayoutBorder;             //键盘布局异性框类
    KbLayoutWidget *m_kbLayoutWidget;              //键盘布局窗体
    WidgetShowType m_showType;                     //窗体显示模式,分为无密码登录模式\正常模式\ID和密码登录模式
    QVBoxLayout *m_userLayout;                     //用户输入框布局
    QVBoxLayout *m_lockLayout;                     //解锁按钮布局
    bool m_isLock;                                 //解锁功能是否被锁定(连续5次密码输入错误锁定)
    bool m_isLogin;                                //是否登录（UserFrame中使用）
    bool m_isSelected;
    QStringList m_KBLayoutList;
    QString m_defkBLayout;
    QLabel *m_loginLabel;
    QAction *m_KBAction;                           //键盘布局Action
    QAction *m_capsAction;                         //大小写锁定Action
    KeyboardMonitor *m_capslockMonitor;
    QHBoxLayout *m_nameLayout;
    QFrame *m_nameFrame;
};

#endif // USERLOGINWIDGET_H
