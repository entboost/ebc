//package com.entboost.im.global;
package org.qtproject.entboost;

import android.annotation.TargetApi;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;

//import com.entboost.im.R;
import org.qtproject.entboost.NotificationReceiver;

public class NotificationUtils {
	
	public static int messageNotificationID = 100;
	
	public static final String EXTRA_BUNDLE = "main_activity_extra_bundle";
	public static final String INTENT_DYNAMIC_NEWS_TYPE = "dynamic_news_type";
	
	//通知分类
	public static String PENDINGINTENT_TYPE_ACTIVITY 	=	"activity";
	public static String PENDINGINTENT_TYPE_SERVICE 	= 	"service";
	public static String PENDINGINTENT_TYPE_BROADCASE 	= 	"broadcase";
	
	//提醒类型
	public static int NOTIFICATION_SETTING_MESSAGE_DETAILS = 2;	//通知显示消息详情
	public static int NOTIFICATION_SETTING_MESSAGE_SOUND = 4;		//声音
	public static int NOTIFICATION_SETTING_MESSAGE_VIBRATE = 8;	//振动	
	
	@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
	/**
	 * 发送通知栏消息
	 * @param context 上下文对象
	 * @param icon 图标资源
	 * @param title 标题
	 * @param content 内容
	 * @param number 未读数量
	 * @param notificationIntent
	 * @param intentType
	 */
	public static void sendNotificationMsg(Context context, int icon, CharSequence title, 
			CharSequence content, int number, Intent notificationIntent, String intentType) {
		
		//检查通知提醒的配置
		boolean enableSound = true;
		boolean enableVibrate = true;
		
		SharedPreferences preferences = context.getSharedPreferences("notificationSetting", Context.MODE_PRIVATE);
		enableSound = preferences.getBoolean(String.valueOf(NOTIFICATION_SETTING_MESSAGE_SOUND), true);
		enableVibrate = preferences.getBoolean(String.valueOf(NOTIFICATION_SETTING_MESSAGE_VIBRATE), true);
		
		//声音和震动
		int defaults = 0 | (enableSound?Notification.DEFAULT_SOUND:0) | (enableVibrate?Notification.DEFAULT_VIBRATE:0);
		
		// 定义NotificationManager
		NotificationManager mNotificationManager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
		
		PendingIntent pendingIntent = null;
		if (intentType==PENDINGINTENT_TYPE_ACTIVITY)
			pendingIntent = PendingIntent.getActivity(context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
		else if (intentType==PENDINGINTENT_TYPE_SERVICE)
			pendingIntent = PendingIntent.getService(context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
		else if (intentType==PENDINGINTENT_TYPE_BROADCASE)
			pendingIntent = PendingIntent.getBroadcast(context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
		
		Notification.Builder builder = new Notification.Builder(context);
		builder.setContentTitle(title);
		//builder.setContentInfo("");
		builder.setContentText(content);
		builder.setSmallIcon(icon);
		//builder.setTicker("新消息");
		//builder.setAutoCancel(true);
		builder.setWhen(System.currentTimeMillis());
		builder.setContentIntent(pendingIntent);
		builder.setDefaults(defaults);
		
		//Notification mNotification = new Notification(icon, "[" + title + "]" + content, System.currentTimeMillis());
		//mNotification.setLatestEventInfo(context, title, content, PendingIntent.getActivity(context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT));
		//mNotification.defaults = Notification.DEFAULT_SOUND;
		Notification mNotification = null;
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
			mNotification = builder.build();
		} else {
			mNotification = builder.getNotification();
		}
		
		mNotification.flags = Notification.FLAG_AUTO_CANCEL;
		
		mNotificationManager.notify(messageNotificationID, mNotification);
	}
	
	/**
	 * 删除通知栏关于本应用的全部通知
	 * @param context
	 */
	public static void cancelNotificationMsg(Context context) {
		NotificationManager mNotificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
		mNotificationManager.cancel(messageNotificationID);
	}

        public static int sendMsg(Context context) {
//		Context context = null;
		Intent intent = new Intent(context, NotificationReceiver.class);
		
		Bundle bundle = new Bundle();
//		bundle.putInt(INTENT_DYNAMIC_NEWS_TYPE, news.getType());
//		bundle.putString(ChatActivity.INTENT_TITLE, news.getTitle());
//		bundle.putLong(ChatActivity.INTENT_TOID, news.getSender());
		intent.putExtra(EXTRA_BUNDLE, bundle);
		
                NotificationUtils.sendNotificationMsg(context, 1,
                "您有一条未读通知", "1234内容", 0, intent, PENDINGINTENT_TYPE_BROADCASE);
                return 1;
	}
//    public static void main(String[] args) {
//        Context context = null;
//        Intent intent = new Intent(context, NotificationReceiver.class);

//        Bundle bundle = new Bundle();
//        //		bundle.putInt(INTENT_DYNAMIC_NEWS_TYPE, news.getType());
//        //		bundle.putString(ChatActivity.INTENT_TITLE, news.getTitle());
//        //		bundle.putLong(ChatActivity.INTENT_TOID, news.getSender());
//        intent.putExtra(EXTRA_BUNDLE, bundle);

//        UIUtils.sendNotificationMsg(context, R.drawable.notify,
//        "您有一条未读通知", "1234内容", 0, intent, PENDINGINTENT_TYPE_BROADCASE);
//        }
}
