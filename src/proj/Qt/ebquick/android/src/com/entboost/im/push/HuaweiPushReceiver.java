package com.entboost.im.push;

import android.app.NotificationManager;
import android.content.Context;
import android.os.Bundle;

//import com.entboost.Log4jLog;
import com.huawei.hms.support.api.push.PushReceiver;

/**
 * 接收华为Push所有消息的广播接收器
 *
 */
public class HuaweiPushReceiver extends PushReceiver {
	
	private static String LONG_TAG = HuaweiPushReceiver.class.getName();
	
    @Override
    public void onToken(Context context, String token, Bundle extras) {
//        String belongId = extras.getString("belongId");
//        String content = "get token and belongId successful, token = " + token + ",belongId = " + belongId;
//        Log4jLog.i(LONG_TAG, content);

        /// ***
        EntboostPushClient.pushToken = token;
//        ThirdPartyPushHelper.pushToken = token;
//        ThirdPartyPushHelper.setPushToken(true);
    }
    
    @Override
    public boolean onPushMsg(Context context, byte[] msg, Bundle bundle) {
//        try {
//            String content = "Receive a Push pass-by message： " + new String(msg, "UTF-8");
//            Log4jLog.i(LONG_TAG, content);
//        } catch (Exception e) {
//        	Log4jLog.e(LONG_TAG, e);
//        }
        return false;
    }
    
    public void onEvent(Context context, Event event, Bundle extras) {
        if (Event.NOTIFICATION_OPENED.equals(event) || Event.NOTIFICATION_CLICK_BTN.equals(event)) {
            int notifyId = extras.getInt(BOUND_KEY.pushNotifyId, 0);
            if (0 != notifyId) {
                NotificationManager manager = (NotificationManager) context
                        .getSystemService(Context.NOTIFICATION_SERVICE);
                manager.cancel(notifyId);
            }
            String content = "receive extented notification message: " + extras.getString(BOUND_KEY.pushMsgKey);
//            Log4jLog.i(LONG_TAG, content);
        }
        super.onEvent(context, event, extras);
    }
    
    @Override
    public void onPushState(Context context, boolean pushState) {
        try {
            String content = "The current push status： " + (pushState ? "Connected" : "Disconnected");
//            Log4jLog.d(LONG_TAG, content);
        } catch (Exception e) {
//        	Log4jLog.e(LONG_TAG, e);
        }
    }
}
