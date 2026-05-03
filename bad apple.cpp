#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <cmath>
#include <SDL2/SDL.h>

using namespace std;
using namespace cv;
using namespace chrono;


int main(int argc, char* argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);

    string path = "folder/bad apple.mp4";
    VideoCapture cap(path);
    SDL_AudioSpec wavSpec;
    Uint8* wavBuffer;
    Uint32 wavLength;
    SDL_LoadWAV("folder/bad apple.wav", &wavSpec, &wavBuffer, &wavLength);
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

    double fps = cap.get(CAP_PROP_FPS);
    double delay =1000 /fps;
    Mat img;
    Mat imgResize;
    string asciiChars = " .:-=+*#%@";
    Mat blank = Mat::zeros(480, 640, CV_8UC3);
    auto startTime = steady_clock::now();
    int frameCounter = 0;

    SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);

    while (true){
        auto currentTime = steady_clock::now();
        double Seconds = duration_cast<duration<double>>(currentTime - startTime).count();
        int targetFrame = (int)(Seconds*fps);
        while (frameCounter < targetFrame) {
           cap.grab();
           frameCounter++;
        }
        cap.read(img);
        frameCounter++;

        if (img.empty()) break;
        resize(img, imgResize, Size(80, 60));
        Mat imgGray;
        cvtColor(imgResize, imgGray, COLOR_BGR2GRAY); 
        for(int x=0;x<imgGray.cols;x++){
            for(int y=0;y<imgGray.rows;y++){
                uchar pixel = imgGray.at<uchar>(y, x);
                int charIndex = pixel * (asciiChars.length() - 1) / 255;
                char c = asciiChars[charIndex];
                string s(1, c);
                Point org(x*8, y*8);
                int fontFace = FONT_HERSHEY_SIMPLEX;
                double fontScale = 0.3;
                Scalar color(255, 255, 255);
                double thickness = 0.03;
                int lineType = LINE_AA;
                putText(blank,s, org, fontFace, fontScale, color, thickness, lineType);
            }
        }
        imshow("bad apple",blank);
        blank.setTo(0);
        waitKey(delay);


    }
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();
    return 0;

}
