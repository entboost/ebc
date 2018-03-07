package com.entboost.im.push;

import android.content.Context;

import com.entboost.im.push.HuaweiPushApiAccessor.HuaweiPushApiAccessorListener;
import com.xiaomi.mipush.sdk.MiPushClient;

public class EntboostPushClient
{
    //第三方推送令牌
    public static String pushToken;

    public static void registerPushXiaomi(Context context, String pushAppId, String pushAppKey)
    {
        MiPushClient.registerPush(context, pushAppId, pushAppKey);
    }
    public static void registerPushHUAWEI(Context context)
    {
        final HuaweiPushApiAccessor hwpAccessor = HuaweiPushApiAccessor.getInstance(context);
        hwpAccessor.setListener(new HuaweiPushApiAccessorListener() {
                @Override
                public void onConnected() {
                        hwpAccessor.requestTokenFormHuaweiServer();
                        hwpAccessor.setPassByMsg(false);
                }
        });

        hwpAccessor.connect();
    }
    public static String getPushToken()
    {
        return EntboostPushClient.pushToken;
    }
}
