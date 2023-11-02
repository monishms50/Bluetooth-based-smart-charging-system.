package com.appblocky.smartcharger;

import androidx.appcompat.app.AppCompatActivity;

import android.content.ContextWrapper;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Bundle;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.List;
import java.util.UUID;


import android.annotation.SuppressLin
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.view.animation.Animation;
import android.view.animation.RotateAnimation;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;

import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;

    private byte AttinyOut;
    private boolean ledStat;
    private boolean connectStat = false;

    private Button connect_button;

    protected static final int MOVE_TIME = 80;
    private long lastWrite = 0;
    private AlertDialog aboutAlert;
    private View controlView;
    OnClickListener myClickListener;
    ProgressDialog myProgressDialog;
    private Toast failToast;
    private Handler mHandler;

    private SensorManager mySensorManager;
    private List<Sensor> sensors;
    private Sensor accSensor;

    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothSocket btSocket = null;
    private OutputStream outStream = null;
    private ConnectThread mConnectThread = null;
    private String deviceAddress = null;

    private static final UUID SPP_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private InputStream inStream = null;
    private Thread read_thread = null;

    boolean connect_flag = false;

    private static final String TAG = "CompassActivity";

    private ImageView arrowView;
    private TextView sotwLabel;  // SOTW is for "side of the world"

    private float currentAzimuth;
    public String sendString;
    
Handler hos;
TextView txtBattery;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        connect_button = (Button) findViewById(R.id.connect_button);
        mySensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        sensors = mySensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
        if (sensors.size() > 0)
            accSensor = sensors.get(0);
        myClickListener = new OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialog, int which)
            {
                switch (which)
                {
                    case DialogInterface.BUTTON_POSITIVE:
                        dialog.dismiss();
                        break;
                    default:
                        dialog.dismiss();
                }
            }
        };

        myProgressDialog = new ProgressDialog(this);
        failToast = Toast.makeText(this, R.string.failedToConnect, Toast.LENGTH_SHORT);

        mHandler = new Handler()
        {
            @Override
            public void handleMessage(Message msg)
            {
                if (myProgressDialog.isShowing())
                {
                    myProgressDialog.dismiss();
                }
                if (msg.what == 1)
                {
                    connectStat = true;
                    connect_button.setText(R.string.connected);
                    AttinyOut = 0;
                    ledStat = false;
                    write(AttinyOut);
                } else
                {
                    failToast.show();
                }
            }
        };

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null)
        {
            Toast.makeText(this, R.string.no_bt_device, Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        if (!mBluetoothAdapter.isEnabled())
        {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }

        connect_button.setOnClickListener(new View.OnClickListener()
        {

            @Override
            public void onClick(View v)
            {
                if (connectStat)
                {
                    disconnect();
                } else
                {
                    connect();
                }
            }
        });

        hos=new Handler();
        txtBattery=findViewById(R.id.txtBattery);


        new Thread(new Runnable()
        {

            @Override
            public void run() {
                // TODO Auto-generated method stub
                while(true)
                {
                    try
                    {
                        Thread.sleep(10000);
                        hos.post(new Runnable()
                        {

                            @Override
                            public void run() {
                                // TODO Auto-generated method stub

                                int bval=getBatteryLevel();
                                txtBattery.setText(String.valueOf(bval)+ "%");
                                sendValue(bval);
                            }

                        });

                    }
                    catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                }
            }

        }).start();

    }

    public void sendValue(int val) {
        if (!connect_flag)
        {
            connect_request();
            connect_flag = true;
        }
        String out_byte = val+"\n";
        write_byte(out_byte.getBytes());
        emptyOutStream();

    }

    private int getBatteryLevel() {
        int batteryLevel = -1;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            BatteryManager batteryManager = (BatteryManager) getSystemService(BATTERY_SERVICE);
            batteryLevel = batteryManager.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY);
        } else {
            Intent intent = new ContextWrapper(getApplicationContext()).
                    registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
            batteryLevel = (intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1) * 100) /
                    intent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
        }

        return batteryLevel;
    }
    public void btnClkRight(View view) {
        if (!connect_flag)
        {
            connect_request();
            connect_flag = true;
        }
        //	byte[] out_byte = "*1".getBytes();
        String out_byte = "*4";
        write_byte(out_byte.getBytes());
        emptyOutStream();
    }


    /** Thread used to connect to a specified Bluetooth Device */
    public class ConnectThread extends Thread
    {
        private String address;
        private boolean connectionStatus;

        ConnectThread(String MACaddress)
        {
            address = MACaddress;
            connectionStatus = true;
        }

        public void run()
        {

            try
            {
                BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);


                try
                {
                    btSocket = device.createRfcommSocketToServiceRecord(SPP_UUID);
                } catch (IOException e)
                {
                    connectionStatus = false;
                }
            } catch (IllegalArgumentException e)
            {
                connectionStatus = false;
            }


            mBluetoothAdapter.cancelDiscovery();


            try
            {
                btSocket.connect();
            } catch (IOException e1)
            {
                try
                {
                    btSocket.close();
                } catch (IOException e2)
                {
                }
            }


            try
            {
                outStream = btSocket.getOutputStream();
                inStream = btSocket.getInputStream();


            } catch (IOException e2)
            {
                connectionStatus = false;
            }


            if (connectionStatus)
            {
                mHandler.sendEmptyMessage(1);
            } else
            {
                mHandler.sendEmptyMessage(0);
            }
        }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case REQUEST_CONNECT_DEVICE:

                if (resultCode == Activity.RESULT_OK) {

                    myProgressDialog = ProgressDialog.show(this, getResources().getString(R.string.pleaseWait), getResources().getString(R.string.makingConnectionString), true);


                    deviceAddress = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);

                    mConnectThread = new ConnectThread(deviceAddress);
                    mConnectThread.start();

                } else {

                    Toast.makeText(this, R.string.macFailed, Toast.LENGTH_SHORT).show();
                }
                break;
            case REQUEST_ENABLE_BT:

                if (resultCode == Activity.RESULT_OK) {

                } else {

                    Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                    finish();
                }
        }
    }

    public void write(byte data)
    {
        if (outStream != null)
        {
            try
            {
                outStream.write(data);
            } catch (IOException e)
            {
            }
        }
    }

    public void write_byte(byte[] data)
    {
        if (outStream != null)
        {
            try
            {
                outStream.write(data);
            } catch (IOException e)
            {
                e.printStackTrace();
            }
        }
    }

    public void emptyOutStream()
    {
        if (outStream != null)
        {
            try
            {
                outStream.flush();
            } catch (IOException e)
            {
            }
        }
    }

    public void connect_request()
    {

        byte[] out_byte = "+C".getBytes();
        write_byte(out_byte);
        emptyOutStream();
    }

    public void connect()
    {

        Intent serverIntent = new Intent(this, DeviceListActivity.class);
        startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
    }

    public void disconnect()
    {
        if (outStream != null)
        {
            try
            {
                inStream.close();
                outStream.close();
                connectStat = false;
                connect_button.setText(R.string.disconnected);
            } catch (IOException e)
            {
            }
        }
    }




    @Override
    public void onDestroy()
    {
        emptyOutStream();
        disconnect();

        super.onDestroy();
    }
    ///////////////////****************Receive values  spliting place-start******////////////////////////////
    Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            Bundle b = msg.getData();
            String key = b.getString("data");


            Log.e("VV", key);//for checking enable this





        }
    };

}

