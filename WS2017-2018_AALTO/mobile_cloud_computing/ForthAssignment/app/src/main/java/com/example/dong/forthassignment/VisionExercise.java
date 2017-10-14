package com.example.dong.forthassignment;

import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.net.Uri;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.SparseArray;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.google.android.gms.vision.Detector;
import com.google.android.gms.vision.Frame;
import com.google.android.gms.vision.barcode.Barcode;
import com.google.android.gms.vision.barcode.BarcodeDetector;
import com.google.android.gms.vision.face.Face;
import com.google.android.gms.vision.face.FaceDetector;
import com.google.android.gms.vision.text.Text;

import java.io.InputStream;

public class VisionExercise extends AppCompatActivity {

    private static final int SELECT_PICTURE = 228;
    private static final int REQUIRE_HEIGHT = 200;
    private static final int REQUIRE_WIDTH = 200;
    private String selectedImagePath;
    Canvas canvas = new Canvas();
    TextView peopleCount;
    TextView barcodeDetect;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_vision_exercise);
        peopleCount = (TextView) findViewById(R.id.txtNumPeople);
        barcodeDetect = (TextView) findViewById(R.id.txtBarcode);
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == RESULT_OK) {
            if (requestCode == SELECT_PICTURE) {
                Uri selectedImageUri = data.getData();

                System.out.println("selectedImagePath: " + selectedImagePath);
                System.out.println("directly from getPath(): " + selectedImageUri.getPath().toString());
                ImageView imageView = (ImageView) findViewById(R.id.imgPreview);

                Bitmap imageBitmap = getOriginalBitMap(selectedImageUri);

                //face

                Frame frame = new Frame.Builder().setBitmap(imageBitmap).build();
                Detector<Face> detector = new FaceDetector.Builder(getApplicationContext())
                        .setTrackingEnabled(false)
                        .setLandmarkType(FaceDetector.ALL_LANDMARKS)
                        .setClassificationType(FaceDetector.ALL_CLASSIFICATIONS)
                        .build();
                SparseArray<Face> faces = detector.detect(frame);
                peopleCount.setText("" + faces.size());

                for(int i = 0; i < faces.size(); i++)
                {
                    Face eachFace = faces.valueAt(i);
                    drawBox(imageBitmap,eachFace.getPosition(),eachFace.getHeight(),eachFace.getWidth());
                }


                //barcode

                Frame barcodeFrame = new Frame.Builder().setBitmap(imageBitmap).build();
                BarcodeDetector barcodeDetector = new BarcodeDetector.Builder(getApplicationContext())
                        .setBarcodeFormats(Barcode.ALL_FORMATS).build();
                SparseArray<Barcode> barcodes =  barcodeDetector.detect(barcodeFrame);
                if (barcodes.size() > 0)
                {
                    barcodeDetect.setText("Yes");
                }
                else
                {
                    barcodeDetect.setText("No");
                }

                imageView.setImageBitmap(imageBitmap);
            }
        }
    }

    public void clickButton (View view)
    {
        Intent intent = new Intent();
        intent.setType("image/*");
        intent.setAction(Intent.ACTION_GET_CONTENT);
        startActivityForResult(Intent.createChooser(intent,"Select Picture"),SELECT_PICTURE);
    }


    /*
        draw a box on bitmap
     */
    public Bitmap drawBox(Bitmap imageBitmap, PointF position, float height, float width)
    {
        Paint myPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        myPaint.setStyle(Paint.Style.STROKE);
        myPaint.setColor(Color.rgb(0,255,0));
        myPaint.setStrokeWidth(5);

        canvas.setBitmap(imageBitmap);
        canvas.drawRect(position.x,position.y,position.x+width,position.y+height,myPaint); //TODO: use parameters
        return imageBitmap;
    }

    public Bitmap getOriginalBitMap(Uri imageURI){
        Bitmap bitmap = null;
        BitmapFactory.Options options = new BitmapFactory.Options();

        //An inputStream can only be used once, declare another one for the real loading
        InputStream imageStream = null;
        InputStream tmpImageStream = null;

        try {
            imageStream = getContentResolver().openInputStream(imageURI);
            tmpImageStream = getContentResolver().openInputStream(imageURI);


        }
        catch (Exception e)
        {
            e.printStackTrace();
        }



        options.inJustDecodeBounds = true;


        bitmap = BitmapFactory.decodeStream(tmpImageStream,null,options);

        options.inSampleSize = calculateInSampleSize(options, REQUIRE_WIDTH, REQUIRE_HEIGHT);

        options.inJustDecodeBounds = false;

        bitmap = BitmapFactory.decodeStream(imageStream,null,options);

        Bitmap myBitmap = Bitmap.createBitmap(bitmap.getWidth(),bitmap.getHeight(),Bitmap.Config.ARGB_8888);
        canvas.setBitmap(myBitmap);
        canvas.drawBitmap(bitmap,0,0,null);

        return myBitmap;
    }
    /*https://developer.android.com/topic/performance/graphics/load-bitmap.html*/
    public int calculateInSampleSize(
            BitmapFactory.Options options, int reqWidth, int reqHeight) {
        // Raw height and width of image
        final int height = options.outHeight;
        final int width = options.outWidth;
        int inSampleSize = 1;

        if (height > reqHeight || width > reqWidth) {

            final int halfHeight = height / 2;
            final int halfWidth = width / 2;

            // Calculate the largest inSampleSize value that is a power of 2 and keeps both
            // height and width larger than the requested height and width.
            while ((halfHeight / inSampleSize) >= reqHeight
                    && (halfWidth / inSampleSize) >= reqWidth) {
                inSampleSize *= 2;
            }
        }

        return inSampleSize;
    }

    public String getPath(Uri uri)
    {
        String[] filePathColumn = {MediaStore.Images.Media.DATA};
        Cursor cursor = getContentResolver().query(uri, filePathColumn, null, null, null);
        String yourRealPath = uri.getPath().toString();

        if(cursor.moveToFirst()){
            int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
            yourRealPath = cursor.getString(columnIndex);

        }
        cursor.close();
        return yourRealPath;

    }
}
