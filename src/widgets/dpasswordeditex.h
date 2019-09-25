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

#ifndef DPASSWORDEDITEX_H
#define DPASSWORDEDITEX_H

#include <DLineEdit>

DWIDGET_USE_NAMESPACE

class QPropertyAnimation;
class LoadSlider : public QWidget
{
public:
    LoadSlider(QWidget *parent = nullptr);

public:
    QColor loadSliderColor() const;
    void setLoadSliderColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QColor m_loadSliderColor;
};

class DPasswordEditEx : public DLineEdit
{
    Q_OBJECT
public:
    DPasswordEditEx(QWidget *parent = nullptr);

public Q_SLOTS:
    void inputDone();
    void showLoadSlider();
    void hideLoadSlider();

protected:
    void paintEvent(QPaintEvent *event) override;

protected:
    Q_DISABLE_COPY(DPasswordEditEx)

private:
    LoadSlider *m_loadSlider;
    QPropertyAnimation *m_loadSliderAnim;

    bool m_loadAnimEnable;
    bool m_isLoading;

    const int LoadSliderWidth = 40;
};

#endif // DPASSWORDEDITEX_H
