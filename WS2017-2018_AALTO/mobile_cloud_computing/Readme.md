#Individual Assignment of Mobile cloud computing

## [First assignment: Hello User](./MyApplication)
This is an introductory assignment to learn about mobile application programming with Android. The objective is to get familiar with basic concepts such as layouts and user interface elements (e.g., EditText and TextView). You can refer to this tutorial if you have not developed any Android application before.

###Task¶

Your task is to create an application that allows user input using an EditText. Such input has to be concatenated to a TextView once the user clicks on a button.

## [Second assignment: QR Code](./SecondAssignment)
The objective of this assignment is to learn how to generate barcodes with an Android application.

### Task

Your task is to generate QR codes based on the user input. To this end, you will use an EditText for the input and an ImageView for displaying the result, namely, the QR code that corresponds to the text.

##[Third assignment: Image List](./ThirdAssignment)
The objective of this assignment is to learn how to use a ListView to display images taken from the Internet.

### Task

Your task is to parse a JSON file given as an input by using an EditText. By parsing the file, you obtain a sequence of items, each containing a link to an image and the corresponding author’s name. These must be displayed in a ListView.

The JSON file structure is as follows.

```
[
    {photo:"", author:""},
    {photo:"", author:""},
    {photo:"", author:""},
    {photo:"", author:""},
    {photo:"", author:""}
]
```



You can use libraries such `OkHttp` for fetching the JSON file, and Picasso for showing the photos. You can also use any other libraries or write your own code if you prefer.

You can use following JSON file to test your application: http://www.mocky.io/v2/59a94ceb100000200c3e0a78

## [Fourth assignment: Image Detection](./ForthAssignment)
This assignment introduces you to the use of Mobile Vision API in an Android application.

>Note
>
>You have become familiar with layouts and control after completing the previous assignments. Thus, the objective of this assignment is to explore the Mobile Vision API for object recognition on images.

### Task

Your task is to create an application that recognizes objects given a source picture. The user interface should include an option to select the image from the gallery of the phone by clicking a button. Once the image is selected, it should be displayed on the screen as well as the number of faces recognized and if the image contains a barcode or not. Mobile Vision API has some limitations when objects are small, but we are not interested in such cases. The test cases will be high resolution pictures with clear faces or QR codes.

Your application should process images in a background thread to prevent blocking the User Interface thread. You should also consider proper resizing of images before displaying them, to prevent the application from running out of memory.

> Attention
> 
>Use the following Intent action and type to select a photo from the gallery. Our automated grading system relies on this to test your application with different photos.
>
>```
>Intent intent = new Intent();
>intent.setType("image/*");
>intent.setAction(Intent.ACTION_GET_CONTENT);
>```
