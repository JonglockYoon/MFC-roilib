
MFC-roilib compile 절차

----
<pre>
1. OpenCV 3.4.0
   아래 세개의 Source를 다운로드받아 X64 Library(opencv_world340d.lib 및 opencv_world340.lib)를 만듭니다
   https://github.com/opencv/opencv/releases/tag/3.4.0       ==> Source code(zip)
   https://github.com/opencv/opencv_contrib/releases         ==> 3.4.0 zip
   http://eigen.tuxfamily.org/index.php?title=Main_Page      ==> latest stable release

   cmake 이용방법은 http://webnautes.tistory.com/1036 를 참조하면 되겠습니다.

    
   결과를 c:\opencv에 설치를합니다.
   OPENCV_BUILD 환경변수를 설정합니다. : c:\opencv\build
   c:/opencv/build/x64/vc14/opencv_world340d.lib 또는 c:/opencv/build/x64/vc14/opencv_world340.lib 라이브러리를 이용합니다.
    
2. pthread
   pthread\pthreads-w32-1-11-0-release\pthread.sln 을 이용하여 X64 library를 만듭니다
    
3. Visual studio 2015 를 이용하여 CxImage project를 실행해서 X64 library를 만듭니다.

4. tesseract compile
   //cppan 을 다운로드해서 준비합니다.
   cd \roilib
   git clone https://github.com/tesseract-ocr/tesseract tesseract
   cd tesseract
   // src/viewer/svpaint.cpp 파일의 main()함수를 다른이름으로 rename해줍니다.
   mkdir Win64
   cd Win64
   cppan ..
   cmake .. -G "Visual Studio 14 2015 Win64"
   cppan --generate .
   또는 cmake gui program을 이용해서 Visual studio project를 생성합니다.

5. Visual studio 2015 를 이용하여 roilibapp project 실행해서 roilibapp.exe를 생성합니다.

</pre>

https://blog.naver.com/jerry1455/221381146963 - roilib tesseract with OpenCV
https://blog.naver.com/jerry1455/221380408455 - roilib 바코드 Decode추가   

https://blog.naver.com/jerry1455/221379771615 - MFC-roilib 란?
