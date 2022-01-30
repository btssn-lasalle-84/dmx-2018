/****************************************************************************
** Copyright (c) 2000-2003 Wayne Roth
** Copyright (c) 2004-2007 Stefan Sander
** Copyright (c) 2007 Michal Policht
** Copyright (c) 2008 Brandon Fosdick
** Copyright (c) 2009-2010 Liam Staskawicz
** Copyright (c) 2011 Debao Zhang
** All right reserved.
** Web: http://code.google.com/p/qextserialport/
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/

#ifndef _QEXTSERIALENUMERATOR_H_
#define _QEXTSERIALENUMERATOR_H_

#include <QtCore/QList>
#include <QtCore/QObject>
#include "qextserialport_global.h"

/**
 * @brief
 *
 * @class QextPortInfo qextserialenumerator.h "qextserialport/qextserialenumerator.h"
 */
struct QextPortInfo {
    QString portName;   ///< Port name. /**< TODO */
    QString physName;   ///< Physical name. /**< TODO */
    QString friendName; ///< Friendly name. /**< TODO */
    QString enumName;   ///< Enumerator name. /**< TODO */
    int vendorID;       ///< Vendor ID. /**< TODO */
    int productID;      ///< Product ID /**< TODO */
};

class QextSerialEnumeratorPrivate;
/**
 * @brief
 *
 * @class QextSerialEnumerator qextserialenumerator.h "qextserialport/qextserialenumerator.h"
 */
class QEXTSERIALPORT_EXPORT QextSerialEnumerator : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QextSerialEnumerator)
public:
/**
 * @brief
 *
 * @fn QextSerialEnumerator
 * @param parent
 */
    QextSerialEnumerator(QObject *parent=0);
    /**
     * @brief
     *
     * @fn ~QextSerialEnumerator
     */
    ~QextSerialEnumerator();

    /**
     * @brief
     *
     * @fn getPorts
     * @return QList<QextPortInfo>
     */
    static QList<QextPortInfo> getPorts();
    /**
     * @brief
     *
     * @fn setUpNotifications
     */
    void setUpNotifications();

Q_SIGNALS:
    /**
     * @brief
     *
     * @fn deviceDiscovered
     * @param info
     */
    void deviceDiscovered(const QextPortInfo &info);
    /**
     * @brief
     *
     * @fn deviceRemoved
     * @param info
     */
    void deviceRemoved(const QextPortInfo &info);

private:
    Q_DISABLE_COPY(QextSerialEnumerator)
#if defined(Q_OS_LINUX) && !defined(QESP_NO_UDEV)
    Q_PRIVATE_SLOT(d_func(), void _q_deviceEvent())
#endif
    QextSerialEnumeratorPrivate *d_ptr; /**< TODO */
};

#endif /*_QEXTSERIALENUMERATOR_H_*/
