package com.entboost.utils;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import android.Manifest;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.content.Context;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Build;
import android.telephony.TelephonyManager;

//import com.entboost.Log4jLog;

/**
 * 注意：需要如下权限 <uses-permission
 * android:name="android.permission.READ_PHONE_STATE"/> <uses-permission
 * android:name="android.permission.ACCESS_NETWORK_STATE"/>
 * 
 * @author simon
 * 
 */
public class PhoneInfo {
	
	/** The tag. */
	private static String TAG = PhoneInfo.class.getSimpleName();
	private static String LONG_TAG = PhoneInfo.class.getName();
	
	
	private static final String FILE_MEMORY = "/proc/meminfo";
	private static final String FILE_CPU = "/proc/cpuinfo";
	public String mIMEI;
	public int mPhoneType;
	public int mSysVersion;
	public String mNetWorkCountryIso;
	public String mNetWorkOperator;
	public String mNetWorkOperatorName;
	public int mNetWorkType;
	public boolean mIsOnLine;
	public String mConnectTypeName;
	public long mFreeMem;
	public long mTotalMem;
	public String mCupInfo;
	public String mProductName;
	public String mModelName;
	public String mManufacturerName;

	/**
	 * private constructor
	 */
	private PhoneInfo() {

	}

	public static String getPhoneNumber(Context context) {
		TelephonyManager tm = (TelephonyManager) context
				.getSystemService(Context.TELEPHONY_SERVICE);
		return tm.getLine1Number();
	}

	/**
	 * get imei
	 * 
	 * @return
	 */
	public static String getIMEI(Context context) {
		TelephonyManager manager = (TelephonyManager) context
				.getSystemService(Activity.TELEPHONY_SERVICE);
		// check if has the permission
		if (PackageManager.PERMISSION_GRANTED == context.getPackageManager()
				.checkPermission(Manifest.permission.READ_PHONE_STATE,
						context.getPackageName())) {
			return manager.getDeviceId();
		} else {
			return null;
		}
	}

	/**
	 * get phone type,like :GSM��CDMA��SIP��NONE
	 * 
	 * @param context
	 * @return
	 */
	public static int getPhoneType(Context context) {
		TelephonyManager manager = (TelephonyManager) context
				.getSystemService(Activity.TELEPHONY_SERVICE);
		return manager.getPhoneType();
	}

	/**
	 * get phone sys version
	 * 
	 * @return
	 */
	public static int getSysVersion() {
		return Build.VERSION.SDK_INT;
	}

	/**
	 * Returns the ISO country code equivalent of the current registered
	 * operator's MCC (Mobile Country Code).
	 * 
	 * @param context
	 * @return
	 */
	public static String getNetWorkCountryIso(Context context) {
		TelephonyManager manager = (TelephonyManager) context
				.getSystemService(Activity.TELEPHONY_SERVICE);
		return manager.getNetworkCountryIso();
	}

	/**
	 * Returns the numeric name (MCC+MNC) of current registered operator.may not
	 * work on CDMA phone
	 * 
	 * @param context
	 * @return
	 */
	public static String getNetWorkOperator(Context context) {
		TelephonyManager manager = (TelephonyManager) context
				.getSystemService(Activity.TELEPHONY_SERVICE);
		return manager.getNetworkOperator();
	}

	/**
	 * Returns the alphabetic name of current registered operator.may not work
	 * on CDMA phone
	 * 
	 * @param context
	 * @return
	 */
	public static String getNetWorkOperatorName(Context context) {
		TelephonyManager manager = (TelephonyManager) context
				.getSystemService(Activity.TELEPHONY_SERVICE);
		return manager.getNetworkOperatorName();
	}

	/**
	 * get type of current network
	 * 
	 * @param context
	 * @return
	 */
	public static int getNetworkType(Context context) {
		TelephonyManager manager = (TelephonyManager) context
				.getSystemService(Activity.TELEPHONY_SERVICE);
		return manager.getNetworkType();
	}

	/**
	 * is webservice aviliable
	 * 
	 * @param context
	 * @return
	 */
	public static boolean isOnline(Context context) {
		ConnectivityManager manager = (ConnectivityManager) context
				.getSystemService(Activity.CONNECTIVITY_SERVICE);
		NetworkInfo info = manager.getActiveNetworkInfo();
		if (info != null && info.isConnected()) {
			return true;
		}
		return false;
	}

	/**
	 * get current data connection type name ,like ,Mobile��WIFI��OFFLINE
	 * 
	 * @param context
	 * @return
	 */
	public static String getConnectTypeName(Context context) {
		if (!isOnline(context)) {
			return "OFFLINE";
		}
		ConnectivityManager manager = (ConnectivityManager) context
				.getSystemService(Activity.CONNECTIVITY_SERVICE);
		NetworkInfo info = manager.getActiveNetworkInfo();
		if (info != null) {
			return info.getTypeName();
		} else {
			return "OFFLINE";
		}
	}

	/**
	 * get free memory of phone, in M
	 * 
	 * @param context
	 * @return
	 */
	public static long getFreeMem(Context context) {
		ActivityManager manager = (ActivityManager) context
				.getSystemService(Activity.ACTIVITY_SERVICE);
		MemoryInfo info = new MemoryInfo();
		manager.getMemoryInfo(info);
		long free = info.availMem / 1024 / 1024;
		return free;
	}

	/**
	 * get total memory of phone , in M
	 * 
	 * @param context
	 * @return
	 */
	public static long getTotalMem(Context context) {
		try {
			FileReader fr = new FileReader(FILE_MEMORY);
			BufferedReader br = new BufferedReader(fr);
			String text = br.readLine();
			String[] array = text.split("\\s+");
//			Log4jLog.w(LONG_TAG, text);
			return Long.valueOf(array[1]) / 1024;
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return -1;
	}

	public static String getCpuInfo() {
		try {
			FileReader fr = new FileReader(FILE_CPU);
			BufferedReader br = new BufferedReader(fr);
			String text = br.readLine();
			String[] array = text.split(":\\s+", 2);
//			for (int i = 0; i < array.length; i++) {
//				Log4jLog.w(LONG_TAG, " .....  " + array[i]);
//			}
//			Log4jLog.w(LONG_TAG, text);
			return array[1];
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * get product name of phone
	 * 
	 * @return
	 */
	public static String getProductName() {
		return Build.PRODUCT;
	}

	/**
	 * get model of phone
	 * 
	 * @return
	 */
	public static String getModelName() {
		return Build.MODEL;
	}

	/**
	 * get Manufacturer Name of phone
	 * 
	 * @return
	 */
	public static String getManufacturerName() {
                return Build.MANUFACTURER;
	}

	public static PhoneInfo getPhoneInfo(Context context) {
		PhoneInfo result = new PhoneInfo();
		result.mIMEI = getIMEI(context);
		result.mPhoneType = getPhoneType(context);
		result.mSysVersion = getSysVersion();
		result.mNetWorkCountryIso = getNetWorkCountryIso(context);
		result.mNetWorkOperator = getNetWorkOperator(context);
		result.mNetWorkOperatorName = getNetWorkOperatorName(context);
		result.mNetWorkType = getNetworkType(context);
		result.mIsOnLine = isOnline(context);
		result.mConnectTypeName = getConnectTypeName(context);
		result.mFreeMem = getFreeMem(context);
		result.mTotalMem = getTotalMem(context);
		result.mCupInfo = getCpuInfo();
		result.mProductName = getProductName();
		result.mModelName = getModelName();
		result.mManufacturerName = getManufacturerName();
		return result;
	}

	@Override
	public String toString() {
		StringBuilder builder = new StringBuilder();
		builder.append("IMEI : " + mIMEI + "\n");
		builder.append("mPhoneType : " + mPhoneType + "\n");
		builder.append("mSysVersion : " + mSysVersion + "\n");
		builder.append("mNetWorkCountryIso : " + mNetWorkCountryIso + "\n");
		builder.append("mNetWorkOperator : " + mNetWorkOperator + "\n");
		builder.append("mNetWorkOperatorName : " + mNetWorkOperatorName + "\n");
		builder.append("mNetWorkType : " + mNetWorkType + "\n");
		builder.append("mIsOnLine : " + mIsOnLine + "\n");
		builder.append("mConnectTypeName : " + mConnectTypeName + "\n");
		builder.append("mFreeMem : " + mFreeMem + "M\n");
		builder.append("mTotalMem : " + mTotalMem + "M\n");
		builder.append("mCupInfo : " + mCupInfo + "\n");
		builder.append("mProductName : " + mProductName + "\n");
		builder.append("mModelName : " + mModelName + "\n");
		builder.append("mManufacturerName : " + mManufacturerName + "\n");
		return builder.toString();
	}
}
