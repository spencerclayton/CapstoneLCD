/*Capstone Image processor written by Spencer CLayton BCIT #A00858365
April 2014*/
#pragma warning(disable:4996)

//INCLUDED LIBRARIES
#include <stdio.h> //std i/o fns
#include <stdlib.h> //std lib fns
#include <ctype.h>//type functions
#include <string.h> //strings functionsfns
#include <iostream>//i/o stream functions
#include <opencv\cv.h> //OpenCV library fns
#include <opencv\highgui.h> //OpenCV highGUI functions

//DEFINED VALUES
#define MAX_CHARS 1000 //Max Chars for string buffer

#define MAX_THRESHOLD 255 //Max Threshold value (from 0) = max pixel brightness
#define MAX_ALPHA 255 //Max opacity for image mask (from 0)

const char SOURCE[]="Source";
const char GS_CONV[]="Greyscale Conversion";
const char THRESHOLD[]="Threshold";
const char THRESH_CONV[]="Threshold Conversion";
const char OPACITY[]="Maximum Opacity";
const char COMPOSITE[]="Composite";
const char ALPHA_MATTE[]="Alpha Matte";
const char CONTROL_PANEL[]="Capstone Image Processor";

const char TEST_FILE1[]="C:\\Users\\tspen_000\\Desktop\\BCIT\\Semester 2\\"
   "ROBT 1270\\CAPSTONE PROJECT\\TEST_MEDIA\\BridgeHeadlights.jpg";
const char TEST_FILE2[]="C:\\Users\\tspen_000\\Desktop\\BCIT\\Semester 2\\"
   "ROBT 1270\\CAPSTONE PROJECT\\TEST_MEDIA\\LamborghiniWet.jpg";
const char TEST_FILE3[]="C:\\Users\\tspen_000\\Desktop\\BCIT\\Semester 2\\"
   "ROBT 1270\\CAPSTONE PROJECT\\TEST_MEDIA\\nova.jpg";
const char TEST_FILE4[]="C:\\Users\\tspen_000\\Desktop\\BCIT\\Semester 2\\"
   "ROBT 1270\\CAPSTONE PROJECT\\TEST_MEDIA\\PoliceLEDs.png";
const char TEST_FILE5[]="C:\\Users\\tspen_000\\Desktop\\BCIT\\Semester 2\\"
   "ROBT 1270\\CAPSTONE PROJECT\\TEST_MEDIA\\e32parkade.jpg";
const char TEST_FILE6[]="C:\\Users\\tspen_000\\Desktop\\BCIT\\Semester 2\\"
   "ROBT 1270\\CAPSTONE PROJECT\\TEST_MEDIA\\arc_welding.jpg";


typedef struct MEDIA_CONTAINER
{
   cv::VideoCapture sourceCapture;//VideoCapture for Source Camera Stream
   cv::Mat sourceImg;//SOURCE image
   cv::Mat greyscaleImg;//Greyscale Image
   cv::Mat thresholdImg;//Hold Threshold Image
   cv::Mat alphaMatte;//2-channel image holding gresycale and alpha channels
   cv::Mat compositeImg;//Composite image
   double threshVal, tmpAlpha;//Threshold Value, temp loc for Alpha channel val
   //Threshold level, opacity level, threshold operation return value
   int pixels, gaussianKernel, opacity;
   char modeSelect;//source media type selection
}
MEDIA_CONTAINER;


//FUNCTION DEFINITIONS:--------------------------------------------------

char welcome();
//FN welcomes to image processor
//returns media mode selection

cv::VideoCapture cameraMode();
//FN to carry out steps to prepare image stream
//returns camera capture

void fileMode(struct MEDIA_CONTAINER* media4Fns);
//FN prepares file for image processing
//returns source image

void mediaProcess(struct MEDIA_CONTAINER* media4Fns);
//Composite FN that takes in source cv::Mat and calls subsequent
//image processing FNs

void printSourceAttributes(struct MEDIA_CONTAINER* media4Fns);
//FN to print attributes of chosen media

void makeAlphaMatte (struct MEDIA_CONTAINER* media4Fns);
//Make alphaMatte img

void makeCompImg(struct MEDIA_CONTAINER* media4Fns);
//makes composite image from sourceImg and alphaMatte

void showImages(struct MEDIA_CONTAINER* media4Fns);
//FN to make windows and display images
//openCV copies image header, but points to image data

void closeWindows();//Close all viewer windows

//END OF FUNCTION DEFINITIONS---------------------------------------------


//GLOBAL VARIABLES----------------------------
char buff[MAX_CHARS], *tok, seps[]=" ,.;\t";
//Variables for String Manipulation
//String buffer, strtok buffer & separators
//END OF GLOBAL VARIABLES---------------------


/*FN MODES----------
cv::namedWindow:
WINDOW_NORMAL
WINDOW_AUTOSIZE

THRESHOLD:
THRESH_BINARY
THRESH_BINARY_INV
THRESH_TRUNC
THRESH_TOZERO
THRESH_TOZERO_INV
------------------*/



void main()
{

   do
   {
      system("cls");

      MEDIA_CONTAINER media;//create media

      //initialize structure primitive variables
      media.modeSelect=0, media.pixels=0;
      media.threshVal=0.0, media.gaussianKernel=31;
      media.opacity=0;


      //START PROCESSOR
      media.modeSelect=welcome();//welcome

      //MEDIA MODE DEPENDANT FN CALLS
      if (media.modeSelect=='C')
      {
         media.sourceCapture=cameraMode();//get source capture
      }

      else //FILE MODE selected
      {
         fileMode(&media);//returns cv::Mat to media.sourceImg
      }

      mediaProcess(&media);//begin media process
      
      closeWindows();//close viewer windows

      do
      {
         printf("\n\tRestart the Processor? Y/N\n\t");
         //Choose FILE or CAMERA input

         gets(buff);
      }
      while (buff[0]==NULL);//Check for no input

      //Parse input
      tok=strtok(buff, seps);
      *tok=toupper(tok[0]);
   }while(*tok=='Y');

}//END MAIN



//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
//-------------------------FUNCTIONS--------------------------------------
//------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////


char welcome()
{//welcomes user to image processor, returns media mode selection
   char mode=NULL;
   printf("\n\n\tWelcome to the Capstone Image Processor\n");//WELCOME
   do
   {
      do
      {
         printf("\n\tUse Camera [C] or File [F] for input?\n\t");
         //Choose FILE or CAMERA input

         gets(buff);
      }
      while (buff[0]==NULL);//Check for no input

      //Parse input
      tok=strtok(buff, seps);
      mode=toupper(tok[0]);

      //IF No valid selection, repeat loop
      if (mode!='C' && mode!='F') printf("\n\tInvalid Entry...");//Error Message
   }
   while(mode!='C' && mode!='F');

   return mode;
}//END welcome


//------------------------------------------------------------------------


cv::VideoCapture cameraMode()
{//FN prepares and returns image stream / camera capture

   //CAMERA VARIABLES
   int camNum=0;//Holds Camera Selection

   do{
      //CHOOSE CAMERA: 0(rear) & 1 (front)
      do
      {
         printf("\n\tFront [1] or Rear [0]?\n\t");
         gets(buff);
      }
      while(buff[0]==NULL || (*buff!='0' && *buff!='1'));//check for no input

      tok=strtok(buff, seps);
      camNum=atoi(tok);//Get camera selection

      if (camNum!=0 && camNum!=1) printf("\n\tInvalid Entry...");
      //Error Message

   }while (camNum!=0 && camNum!=1);

   cv::VideoCapture camera(camNum);
   if (camera.isOpened()!=1)
   {
      printf("Error Opening Camera Stream\n");
      return -1;
   }

   return camera;

}//END cameraMode


//------------------------------------------------------------------------


void fileMode(struct MEDIA_CONTAINER* media4Fns)
{//FN prepares file for image processing
   //Define file & path

   //FILE VARIABLES
   char filePath[MAX_CHARS];// Holds File Path string

   do{
      do
      {
         //printf("\n\tSpecify file and path:\n\t");
         printf("\n\tSpecify Test File\n");
         printf("\n\t\t1: Bridge Headlights\n");
         printf("\n\t\t2: Lamborghini\n");
         printf("\n\t\t3: Supernova HID\n");
         printf("\n\t\t4: Police LEDs\n");
         printf("\n\t\t5: Parkade\n");
         printf("\n\t\t6: Welding\n\t");

         gets(buff);
      }
      while(buff[0]==NULL);//check for no input
      strcpy(filePath,buff);

      //COPY SELECTED TEST FILE PATH AS STRING
      if(buff[0]=='1') strcpy(filePath, TEST_FILE1);
      else if(buff[0]=='2') strcpy(filePath, TEST_FILE2);
      else if(buff[0]=='3') strcpy(filePath, TEST_FILE3);
      else if(buff[0]=='4') strcpy(filePath, TEST_FILE4);
      else if(buff[0]=='5') strcpy(filePath, TEST_FILE5);
      else if(buff[0]=='6') strcpy(filePath, TEST_FILE6);

      //Declare source pointer and initialize source image to
      //image file (filePath), loaded unchanged
      media4Fns->sourceImg=cv::imread(filePath, CV_LOAD_IMAGE_UNCHANGED);

      //check that the source image opened
      if(media4Fns->sourceImg.data==NULL) printf("\n\tError Loading Image file...\n");
   }//END OF "FILE INPUT" LOOP
   while (media4Fns->sourceImg.data==NULL);


}//END fileMode


//------------------------------------------------------------------------


void mediaProcess(struct MEDIA_CONTAINER* media4Fns)
{//Composite FN that takes in source media and calls subsequent image processing FNs
   int wait=0;

   if(media4Fns->modeSelect=='C')
   {
      // >> = "Read frame from camera into Mat"
      media4Fns->sourceCapture >> media4Fns->sourceImg;
   }

   media4Fns->pixels=media4Fns->sourceImg.rows*media4Fns->sourceImg.cols;

   //PRINT ATTRIBUTES FN
   printSourceAttributes(media4Fns);


   //Set Minimum threshold value
   do
   {
      printf("\n\tThreshold the brightest __%% of the image: \n\t");
      gets(buff);
   }
   while (buff[0]==NULL || ((int)(*buff)<=0 && (int)(*buff)>=100));//Check for no input
   tok=strtok(buff, seps);
   media4Fns->threshVal=255*((100-atof(tok))/100);//Set threshold value

   printf("\n\tMax pixel brightness to pass threshold:\n\t%d\n",
      (int)media4Fns->threshVal);//Print brightest pixel value not thresholded


   //Set Opacity value
   do
   {
      printf("\n\tDim the affected area by __%%: \n\t");
      gets(buff);
   }
   while (buff[0]==NULL || ((int)(*buff)<=0 || ((int)(*buff))>=100));//Check for no input
   tok=strtok(buff, seps);
   media4Fns->opacity=(int)(255*((atof(tok))/100));//Set threshold value
   printf("\n\tBaseline Dimming set to: %d/255\n",
      (int)media4Fns->opacity);//Print baseline dimming percentage


   printf("\n\n\tPress Enter to start the processor. . .\n");
   getchar();
   printf("\tProcessor Starting\n");


   do
   {
      if(media4Fns->modeSelect=='C')
      {
         // >> = "Read frame from camera into sourceImg"
         media4Fns->sourceCapture >> media4Fns->sourceImg;

      }

      //CONVERT SOURCEIMG TO GREYSCALE, WRITE TO GSIMG
      cv::cvtColor(media4Fns->sourceImg, media4Fns->greyscaleImg, CV_RGB2GRAY);

      //Blur GReyscale Image TWICE
      cv::GaussianBlur(media4Fns->greyscaleImg, media4Fns->greyscaleImg,
         cv::Size(media4Fns->gaussianKernel,media4Fns->gaussianKernel), 0, 0);

      cv::GaussianBlur(media4Fns->greyscaleImg, media4Fns->greyscaleImg,
         cv::Size(media4Fns->gaussianKernel,media4Fns->gaussianKernel), 0, 0);


      //THRESHOLD GREYSCALEIMG, WRITE TO THRESHOLDIMG
      //Threshold(input, output, value, max value, mode)
      cv::threshold(media4Fns->greyscaleImg,
         media4Fns->thresholdImg,media4Fns->threshVal, MAX_THRESHOLD,
         cv::THRESH_BINARY_INV);

      //Make alphaMatte Img & set alpha vals
      makeAlphaMatte(media4Fns);

      //Make compositeImg
      makeCompImg(media4Fns);

      showImages(media4Fns);

      if(media4Fns->modeSelect=='C')
      {
         wait = cvWaitKey(1);//wait 1 ms
         if ((char)wait==27) break;//if ESC key is pressed break from loop
      }
      else
      {
         wait = cvWaitKey(0);//wait for keystroke indefinitely, then proceed
      }

   }
   while(media4Fns->modeSelect=='C');//END while

}//END imageProcess


//------------------------------------------------------------------------


void printSourceAttributes(struct MEDIA_CONTAINER* media4Fns)
{//Prints attributes of Source Media, returns pixel count

   //Display Attributes of Source Image
   printf("\n\tAttributes of Source image:\n\t%d-channels\n\t%d by %d resolution\n\t"
      "%d picture elements\n\t", media4Fns->sourceImg.channels(),
      media4Fns->sourceImg.cols,
      media4Fns->sourceImg.rows, media4Fns->pixels);

}//END printSourceAttributes


//------------------------------------------------------------------------


void makeAlphaMatte (struct MEDIA_CONTAINER* media4Fns)
{//Make alphaMatte img 

   //convert greyscaleImg to 4-Channel BGRA
   cv::cvtColor(media4Fns->thresholdImg, media4Fns->alphaMatte, CV_GRAY2RGBA, 4);

   //Blur Threshold Image TWICE
   cv::GaussianBlur(media4Fns->alphaMatte, media4Fns->alphaMatte,
      cv::Size(media4Fns->gaussianKernel,media4Fns->gaussianKernel), 0, 0);

   cv::GaussianBlur(media4Fns->alphaMatte, media4Fns->alphaMatte,
      cv::Size(media4Fns->gaussianKernel,media4Fns->gaussianKernel), 0, 0);

   //Set Alpha Value in 4th channel
   int y=0;//rows
   int x=0;//columns

   int red=0, green=0, blue=0, valueDifference=0;
   bool alphaCont=0, gsCont=0;

   alphaCont= media4Fns->alphaMatte.isContinuous();
   gsCont= media4Fns->greyscaleImg.isContinuous();


   if (alphaCont==TRUE && gsCont==TRUE)
   {
      for(int y=0; y<media4Fns->alphaMatte.rows; y++)  
      {
         for(int x=0; x<media4Fns->alphaMatte.cols; x++)
         {
            //Get value of greyscale pixel at current position
            //difference=((pixel value)-((unsigned char)threshold value))

            valueDifference=(int)(media4Fns->greyscaleImg.at<unsigned char>(y,x)) - (int)media4Fns->threshVal;

            if (valueDifference>0)
            {
               //Step is # of elements occupied by each row, channels is # of channels per pixel
               //& numerical constant is the channel # offset
               media4Fns->alphaMatte.data
                  [media4Fns->alphaMatte.step*y + media4Fns->alphaMatte.channels()*x + 3]= (unsigned char)(valueDifference + media4Fns->opacity);

            }//End if
            else
            {
               (media4Fns->alphaMatte.data[media4Fns->alphaMatte.step*y + media4Fns->alphaMatte.channels()*x + 3])=0;

            }//End else

         }//End cols
      }//End Rows
   }//END if
}//END makeAlphaMatte


//------------------------------------------------------------------------


void makeCompImg(struct MEDIA_CONTAINER* media4Fns)
{
   //copy source to comp as bg
   media4Fns->sourceImg.copyTo(media4Fns->compositeImg);

   int y=0, x=0, channelNum=0;//loop variables
   unsigned char basePixel='0', overlayPixel='0';

   bool alphaCont=0, compCont=0;
   alphaCont= media4Fns->alphaMatte.isContinuous();
   compCont= media4Fns->compositeImg.isContinuous();

   if (alphaCont==TRUE && compCont==TRUE)
   {
      for (y=0;y<media4Fns->compositeImg.rows;y++)//for every row
      {
         for (x=0;x<media4Fns->compositeImg.cols;x++)//for every column
         {
            //If alpha channel value is greater than zero
            if ((int)(media4Fns->alphaMatte.data[media4Fns->alphaMatte.step*y
               + media4Fns->alphaMatte.channels()*x + 3])>0)
            {
               //set tmpAlpha value for current pixel using alphaMatte alpha (4th-channel) value
               //valueDifference/255 = percentage of max value
               media4Fns->tmpAlpha =
                  ((double)(media4Fns->alphaMatte.data
                  [media4Fns->alphaMatte.step*y + media4Fns->alphaMatte.channels()*x + 3])/ 255.0);

               //combine alphaMatte with existing compositeImg data
               for(channelNum=0; channelNum<media4Fns->compositeImg.channels(); ++channelNum)
               {
                  //copy current channel value for current pixel to uchar variable for easier math
                  overlayPixel=media4Fns->alphaMatte.data[media4Fns->alphaMatte.step*y
                     + media4Fns->alphaMatte.channels()*x + channelNum];

                  basePixel=media4Fns->compositeImg.data[media4Fns->compositeImg.step*y
                     + media4Fns->compositeImg.channels()*x + channelNum];

                  //Set pixel value for current channel at current location 
                  media4Fns->compositeImg.data
                     [y*media4Fns->compositeImg.step + media4Fns->compositeImg.channels()*x + channelNum] =
                     (unsigned char) (((double)basePixel * (1.0-media4Fns->tmpAlpha))
                     + ((double)overlayPixel * media4Fns->tmpAlpha));
               }//End Channel Processing
            }//End If
         }//End columns loop
      }//end rows loop
   }
   else
   {
      printf("Mmemory Error, quitting Processor\n");
   }

}//END makeCompImg


//------------------------------------------------------------------------


void showImages(struct MEDIA_CONTAINER* media4Fns)
{//Function to make windows and display images

   //CREATE windows for images

   //Display SOURCE Image
   cvNamedWindow(SOURCE, cv::WINDOW_NORMAL);//Create window for SOURCE
   cv::imshow(SOURCE, media4Fns->sourceImg);


   //Display GREYSCALE Image
   cvNamedWindow(GS_CONV, cv::WINDOW_NORMAL);//Create window for GREYSCALE
   cv::imshow(GS_CONV, media4Fns->greyscaleImg);


   //Display THRESHOLD Image
   cvNamedWindow(THRESH_CONV, cv::WINDOW_NORMAL);//Create window for THRESHOLD
   cv::imshow(THRESH_CONV, media4Fns->thresholdImg);


   //Display ALPHAMATTE Image
   cvNamedWindow(ALPHA_MATTE, cv::WINDOW_NORMAL);//Create window for THRESHOLD
   cv::imshow(ALPHA_MATTE, media4Fns->alphaMatte);

   //Display COMPOSITE Image
   cvNamedWindow(COMPOSITE, cv::WINDOW_NORMAL);//Create window for COMPOSITE
   cv::imshow(COMPOSITE, media4Fns->compositeImg);


   //printf("%d\n",loopNum);

   int x=0, y=0;

   if (media4Fns->modeSelect=='C')
   {
      x=640;
      y=480;

      cvResizeWindow(SOURCE, x, y);//RESIZE WINDOW
      cvResizeWindow(GS_CONV, x, y);//RESIZE WINDOW
      cvResizeWindow(THRESH_CONV, x, y);//RESIZE WINDOW
      cvResizeWindow(ALPHA_MATTE, x, y);//RESIZE WINDOW
      cvResizeWindow(COMPOSITE, x, y);//RESIZE WINDOW
   }

   else{
      if(media4Fns->sourceImg.cols>720 || media4Fns->sourceImg.rows>720)
      {
         x=media4Fns->sourceImg.cols/2;
         y=media4Fns->sourceImg.rows/2;

         cvResizeWindow(SOURCE, x, y);//RESIZE WINDOW
         cvResizeWindow(GS_CONV, x, y);//RESIZE WINDOW
         cvResizeWindow(THRESH_CONV, x, y);//RESIZE WINDOW
         cvResizeWindow(ALPHA_MATTE, x, y);//RESIZE WINDOW
         cvResizeWindow(COMPOSITE, x, y);//RESIZE WINDOW
      }
      else
      {
         x=media4Fns->sourceImg.cols;
         y=media4Fns->sourceImg.rows;

         cvResizeWindow(SOURCE, x, y);//RESIZE WINDOW
         cvResizeWindow(GS_CONV, x, y);//RESIZE WINDOW
         cvResizeWindow(THRESH_CONV, x, y);//RESIZE WINDOW
         cvResizeWindow(ALPHA_MATTE, x, y);//RESIZE WINDOW
         cvResizeWindow(COMPOSITE, x, y);//RESIZE WINDOW
      }
   }

}//END showImages


//------------------------------------------------------------------------


void closeWindows()
{
   //destroy viewer windows
   cv::destroyWindow(SOURCE);
   cv::destroyWindow(GS_CONV);
   cv::destroyWindow(THRESH_CONV);
   cv::destroyWindow(ALPHA_MATTE);
   cv::destroyWindow(COMPOSITE);

}//END close Windows