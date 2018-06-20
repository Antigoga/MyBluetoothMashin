package domain.my.bluetooth_interface_example;

import android.annotation.SuppressLint;

import android.graphics.drawable.Drawable;
import android.os.Bundle;

import java.io.OutputStream;
import java.lang.reflect.Method;

import android.util.Log;
import android.content.Intent;

import android.view.MotionEvent;
import android.view.View;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.widget.Button;
import android.widget.Toast;



public class MyActivity extends Activity implements View.OnTouchListener{

    BluetoothSocket clientSocket;//объявляем переменную bluetooth порта
    Button FB;// объявляем переменную кнопки вперёд
    Button BB;// объявляем переменную кнопки назад
    Button LB;// объявляем переменную кнопки влево
    Button RB;// объявляем переменную кнопки вправо
    Button blockRul;//объявляем переменную кнопки блокировки руля
    Button parking;//объявляем переменную кнопки стояночного тормоза
    OutputStream outStream;//переменная КС
    byte value;//переменная - капсула для пересылки команд

    //по следующим счётчикам выбирается фон соответствующих кнопок
    int countBlockRul = 1;//переменная счётчик блокировки руля
    int countParking = 1;//переменная - чётчик стояночного тормоза

        //этот метод вызывается при создании или перезапуске приложения
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my);

        FB = (Button) findViewById(R.id.forward);//инициализируем кнопку вперёд
        BB = (Button) findViewById(R.id.backgo);//инициализируем кнопку назад
        LB = (Button) findViewById(R.id.ef);//инициализируем кнопку влево
        RB = (Button) findViewById(R.id.rigth);//инициализируем кнопку вправо

        blockRul = (Button) findViewById(R.id.blockRulButtom);//инициализируем кнопку блокировка руля
        parking = (Button) findViewById(R.id.parkingButtom);//инициализируем кнопку стояночный тормоз

       //присваиваем обработчики кнопкам
        FB.setOnTouchListener(this);
        BB.setOnTouchListener(this);
        LB.setOnTouchListener(this);
        RB.setOnTouchListener(this);

        blockRul.setOnTouchListener(this);
        parking.setOnTouchListener(this);


    }


    public void сonnect(View v)
    {
        String enableBT = BluetoothAdapter.ACTION_REQUEST_ENABLE;
        startActivityForResult(new Intent(enableBT), 0);
        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        boolean connection = true;

        try
        {
            // 10:14:05:26:01:66 - адрес модуля его можно посмотреть в настройках подключения при соединении к ПК
            BluetoothDevice device = bluetoothAdapter.getRemoteDevice("00:18:E4:0A:00:01");

            //инициируем соединение с устройством
            Method m = device.getClass().getMethod(
                    "createRfcommSocket", int.class);

            clientSocket = (BluetoothSocket) m.invoke(device, 1);
            clientSocket.connect();
            outStream = clientSocket.getOutputStream();

        }

        catch (Exception e)
        {
            //оштбки выводим в лог
            Log.d("BLUETOOTH", e.getMessage());
            //выводим сообщение что не удалось установить соединение
            Toast.makeText(getApplicationContext(), "failed_to_connect", Toast.LENGTH_LONG).show();
            connection = false;
        }
        if (connection) {
            //выводим сообщение об успешном подключении
            Toast.makeText(getApplicationContext(), "CONNECTED", Toast.LENGTH_LONG).show();
        }
    }


   @SuppressLint("NewApi")
   @Override
    public boolean onTouch(View v, MotionEvent motionEvent) {

        //пытаемся послать данные
        try {

              //проверяем какая кнопка была зажата
            switch (motionEvent.getAction()) {

                case MotionEvent.ACTION_DOWN:
                    switch (v.getId()){
                        case R.id.forward:
                            value = '2';
                            outStream.write(value);
                            break;
                        case R.id.backgo:
                            value = '8';
                            outStream.write(value);
                            break;
                         case R.id.ef:
                            value = '4';
                            outStream.write(value);
                            break;
                        case R.id.rigth:
                            value = '6';
                            outStream.write(value);
                            break;
                        case R.id.blockRulButtom:
                            countBlockRul = (countBlockRul+1)%2;

                            if(countBlockRul==0)
                            {
                                blockRul.setBackgroundResource(R.drawable.blockrulfalse);
                            }

                            if(countBlockRul==1)
                            {
                               blockRul.setBackgroundResource(R.drawable.blockrultrue);
                            }

                            value = '7';
                            outStream.write(value);
                            break;
                        case R.id.parkingButtom:
                            countParking = (countParking+1)%2;

                            if(countParking==0)
                            {
                                parking.setBackgroundResource(R.drawable.parkingfalse);
                            }

                            if(countParking==1)
                            {
                                parking.setBackgroundResource(R.drawable.parkingtrue);
                            }

                            value = '3';
                            outStream.write(value);
                            break;
                    }
                    break;

                case MotionEvent.ACTION_UP:
                    switch (v.getId()) {
                        case R.id.forward:
                            value = '5';
                            outStream.write(value);
                            break;
                        case R.id.backgo:
                            value = '5';
                            outStream.write(value);
                            break;
                          case R.id.ef:
                            value = '9';
                            outStream.write(value);
                            break;
                        case R.id.rigth:
                            value = '9';
                            outStream.write(value);
                            break;
                    }
                    break;
            }


        }
        catch (Exception e)
        {
            //если есть ошибки, выводим их в лог
            //Log.d("BLUETOOTH", e.getMessage());
            //и выводим сообщение, что соединение не установлено
            //Toast.makeText(getApplicationContext(), "not_connected", Toast.LENGTH_LONG).show();
        }

        return false;
    }


}