package com.example.dong.myapplication;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class Dongmin extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dongmin);
    }


    public void changeText(View view)
    {
        TextView txtInput = (TextView) findViewById(R.id.txtInput);
        TextView txtResult = (TextView) findViewById(R.id.txtResult);
        txtResult.setText("Hello " + txtInput.getText());

    }
}
