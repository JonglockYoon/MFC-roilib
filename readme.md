
MFC-roilib compile 절차

----
<pre>
1. OpenCV 3.4.0
   Download the following three sources to create the X64 Library (opencv_world340d.lib and opencv_world340.lib)
   https://github.com/opencv/opencv/releases/tag/3.4.0       ==> Source code(zip)
   https://github.com/opencv/opencv_contrib/releases         ==> 3.4.0 zip
   http://eigen.tuxfamily.org/index.php?title=Main_Page      ==> latest stable release

   For instructions on how to use cmake, see http://webnasutes.tistory.com/1036.

   Install the result in c:\opencv.
    Set the OPENCV_BUILD environment variable. : c:\OpenCV\build
    Use the c:/opencv/build/x64/vc14/opencv_world340d.lib or c:/opencv/build/x64/vc14/opencv_world340.lib library.
    
2. pthread
   Create an X64 library using pthread\pthreads-w32-1-11-0-release\pthread.sln
    
3. Run the CxImage project using Visual Studio 2015 to create the X64 library.

4. tesseract compile
   // Download and prepare cppan.
   cd \roilib
   git clone https://github.com/tesseract-ocr/tesseract tesseract
   cd tesseract
   // src/viewer/svpaint.cpp 파일의 main()함수를 다른이름으로 rename해줍니다.
   (Rename main() function name to another name in src/viewer/svpaint.cpp file)
   mkdir Win64
   cd Win64
   cppan ..
   cmake .. -G "Visual Studio 14 2015 Win64"
   cppan --generate .
    또는 cmake gui program을 이용해서 Visual studio project를 생성합니다.
   (Or use the cmake gui program to create a Visual studio project.)

5. Visual studio 2015 를 이용하여 roilibapp project 실행해서 roilibapp.exe를 생성합니다.
   (Run roilibapp project with Visual Studio 2015 to create roilibapp.exe.)

</pre>

https://blog.naver.com/jerry1455/221381146963 - roilib tesseract with OpenCV (tesseract ocr)
https://blog.naver.com/jerry1455/221380408455 - roilib 바코드 Decode추가   

https://blog.naver.com/jerry1455/221379771615 - MFC-roilib 란?


