
MFC-roilib compile 절차

----
<pre>
1. OpenCV 3.4.3
    https://github.com/opencv/opencv/releases/tag/3.4.3 에서 opencv-3.4.3-vc14_vc15.exe를 download해서 설치합니다.
    
    c:\ 경로를 지정하면 c:\opencv에 설치가 됩니다.
    OPENCV_BUILD 환경변수를 설정합니다. : c:\opencv\build
    c:/opencv/build/x64/vc14/opencv_world343d.lib 또는 c:/opencv/build/x64/vc14/opencv_world343.lib 라이브러리를 이용합니다.
    
2. pthread
   pthread\pthreads-w32-1-11-0-release\pthread.sln 을 이용하여 X64 library를 만듭니다
    
3. Visual studio 2015 를 이용하여 CxImage project를 실행해서 X64 library를 만듭니다.

4. Visual studio 2015 를 이용하여 roilibapp project 실행해서 roilibapp.exe를 생성합니다.

</pre>

