package eu.pasleto.sentinel.sentinel_bus_client

import android.app.admin.DeviceAdminReceiver
import android.content.ComponentName
import android.content.Context

class AdminReceiver : DeviceAdminReceiver() {
   companion object {
       fun getComponentName(context: Context): ComponentName {
           return ComponentName(context.applicationContext, AdminReceiver::class.java)
       }
   }
}