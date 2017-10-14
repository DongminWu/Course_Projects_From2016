package com.example.dong.thirdassignment;

import android.content.Context;
import android.graphics.drawable.ColorDrawable;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import com.google.gson.Gson;
import com.squareup.picasso.Picasso;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;

public class ImageList extends AppCompatActivity {

    /**
     * TODO: illegal URL
     */

    String serverResponse = "";
    String serverURL = "";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_list);


    }

    public void getDataFromURL(String url)
    {
        Response response;
        String ret = "";
        Call call;
        System.out.println("url:" + url);

        OkHttpClient client = new OkHttpClient();


        try{
            Request request = new Request.Builder()
                    .url(url)
                    .build();
            call = client.newCall(request);
            call.enqueue(new Callback()
            {
                @Override public void onFailure(Call call, IOException e) {
                    e.printStackTrace();
                }
                @Override public void onResponse(Call call, Response response) throws IOException {
                    ResponseBody responseBody = response.body();
                    if (!response.isSuccessful()) throw new IOException("Unexpected code " + response);
                    serverResponse = responseBody.string();
                    ImageList.this.runOnUiThread(new Runnable(){
                        @Override
                        public void run(){

                            updateList(serverResponse);
                        }
                    });


                }
            });

        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

    }

    public void loadImageButton(View view)
    {

        TextView input = (TextView) findViewById(R.id.txtUrl);
        serverURL = input.getText().toString();

        if (TextUtils.isEmpty(input.getText()))
        {
            serverURL = "http://www.mocky.io/v2/59a94ceb100000200c3e0a78";
        }

        System.out.println("editText:" + input.getText().toString());

        getDataFromURL(serverURL);


    }

    public void updateList(String s)
    {
        ListView listView = (ListView) findViewById(R.id.myList);
        Gson gson = new Gson();

        ServerData[] data = gson.fromJson(s,ServerData[].class);

        final ArrayAdapter adapter =  new myAdapter(this,data);
        listView.setAdapter(adapter);
    }


    private class ServerData
    {
        String photo;
        String author;
    }



    private class myAdapter extends ArrayAdapter<ServerData>{
        private final Context context;
        private ServerData[] data;

        public myAdapter(Context context, ServerData[] data)
        {
            super(context,-1,data);
            this.context = context;
            this.data = data;
        }

        @Override
        public View getView(int position, View oldView, ViewGroup parent)
        {
            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
//            LayoutInflater inflater = (LayoutInflater) getLayoutInflater();

            View rowView = inflater.inflate(R.layout.list_elements, parent, false);
            TextView textView = (TextView) rowView.findViewById(R.id.listText);
            ImageView imageView = (ImageView) rowView.findViewById(R.id.listImage);
            textView.setText(data[position].author);
            Picasso.with(context)
                    .load(data[position].photo)
                    .error(new ColorDrawable(0))
                    .resize(72 ,72 )
                    .into(imageView);
            return rowView;
        }

    }
}

