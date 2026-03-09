
package com.example.smarthome;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

import org.json.JSONObject;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity {

    String ESP32_IP = "http://192.168.1.45";   // CHANGE THIS

    TextView statusText;
    TextView tempText;

    OkHttpClient client = new OkHttpClient();

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnOn = findViewById(R.id.btnOn);
        Button btnOff = findViewById(R.id.btnOff);
        Button btnStatus = findViewById(R.id.btnStatus);

        statusText = findViewById(R.id.statusText);
        tempText = findViewById(R.id.tempText);

        btnOn.setOnClickListener(v -> sendCommand("/on"));
        btnOff.setOnClickListener(v -> sendCommand("/off"));
        btnStatus.setOnClickListener(v -> getStatus());
    }

    void sendCommand(String command){

        new Thread(() -> {

            Request request = new Request.Builder()
                    .url(ESP32_IP + command)
                    .build();

            try {
                client.newCall(request).execute();
            } catch (Exception e) {
                e.printStackTrace();
            }

        }).start();
    }

    void getStatus(){

        new Thread(() -> {

            Request request = new Request.Builder()
                    .url(ESP32_IP + "/status")
                    .build();

            try {

                Response response = client.newCall(request).execute();
                String data = response.body().string();

                JSONObject json = new JSONObject(data);

                String light = json.getString("light");
                int temp = json.getInt("temperature");

                runOnUiThread(() -> {
                    statusText.setText("Light: " + light);
                    tempText.setText("Temperature: " + temp + "°C");
                });

            } catch (Exception e) {
                e.printStackTrace();
            }

        }).start();
    }
}
