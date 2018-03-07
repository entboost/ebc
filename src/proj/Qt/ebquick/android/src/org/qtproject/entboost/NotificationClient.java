/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtAndroidExtras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

package org.qtproject.entboost;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;

public class NotificationClient extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;
    private static NotificationClient m_instance;
    private static PendingIntent m_pendingIntent;

    public NotificationClient()
    {
        m_notificationManager = null;
        m_builder = null;
        m_instance = this;
    }

    public static void notify(int messageId, String text, String title, boolean enableSound, boolean enableVibrate)
    {
        /// 声音和震动
        int defaults = 0 | (enableSound?Notification.DEFAULT_SOUND:0) | (enableVibrate?Notification.DEFAULT_VIBRATE:0);

        if (m_notificationManager == null) {
            m_notificationManager = (NotificationManager)m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
            m_builder = new Notification.Builder(m_instance);
            m_builder.setSmallIcon(R.drawable.icon);
            /// 点击通知
            Intent notificationIntent = new Intent(m_instance, NotificationReceiver.class);
            m_pendingIntent = PendingIntent.getBroadcast(m_instance, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
            m_builder.setContentIntent(m_pendingIntent);

//            PendingIntent pendingIntent = null;
//            if (intentType==PENDINGINTENT_TYPE_ACTIVITY)
//                    pendingIntent = PendingIntent.getActivity(context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
//            else if (intentType==PENDINGINTENT_TYPE_SERVICE)
//                    pendingIntent = PendingIntent.getService(context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
//            else if (intentType==PENDINGINTENT_TYPE_BROADCASE)
//                    pendingIntent = PendingIntent.getBroadcast(context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        }
        m_builder.setContentTitle(title);
        m_builder.setContentText(text);
        m_builder.setWhen(System.currentTimeMillis());
        m_builder.setDefaults(defaults);
//        m_notificationManager.notify(1, m_builder.build());

        Notification mNotification = null;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                mNotification = m_builder.build();
        } else {
                mNotification = m_builder.getNotification();
        }
        mNotification.flags = Notification.FLAG_AUTO_CANCEL;
        /// 1=messageNotificationID
        m_notificationManager.notify(messageId, mNotification);
    }
}
