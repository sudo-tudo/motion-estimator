#include <opencv2/opencv.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <video_path>\n";
        return 1;
    }

    cv::VideoCapture cap(argv[1]);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open video: " << argv[1] << "\n";
        return 1;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    int delay = fps > 0 ? static_cast<int>(1000.0 / fps) : 33;

    cv::Mat frame;
    cv::Ptr<cv::ORB> orb = cv::ORB::create(10);

    while (cap.read(frame)) {
        cv::imshow("Video", frame);

        cv::Mat gray_frame;
        cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        orb->detectAndCompute(gray_frame, cv::noArray(), keypoints, descriptors);

        cv::Mat vis;
        cv::drawKeypoints(gray_frame, keypoints, vis, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

        cv::imshow("ORB", vis);


        int key = cv::waitKey(delay);
        if (key == 27 || key == 'q') break;
    }

    return 0;
}
