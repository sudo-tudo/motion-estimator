#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>

namespace {
constexpr size_t kMinTrackedPoints = 100;
constexpr int kMaxCorners = 400;

std::vector<cv::Point2f> detectCorners(const cv::Mat& gray) {
    std::vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(gray, corners, kMaxCorners, 0.01, 10.0, cv::noArray(), 3, false);
    return corners;
}
}  // namespace

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
    cv::Mat prev_gray_frame;
    std::vector<cv::Point2f> prev_corners;

    while (cap.read(frame)) {
        cv::Mat gray_frame;
        cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

        if (prev_gray_frame.empty() || prev_corners.empty()) {
            prev_corners = detectCorners(gray_frame);
            prev_gray_frame = gray_frame;
            continue;
        }

        std::vector<cv::Point2f> corners;
        std::vector<uchar> status;
        std::vector<float> error;
        cv::calcOpticalFlowPyrLK(prev_gray_frame, gray_frame, prev_corners, corners, status, error);

        std::vector<cv::Point2f> good_prev, good_curr;
        for (size_t i = 0; i < prev_corners.size(); i++) {
            if (status[i]) {
                good_prev.push_back(prev_corners[i]);
                good_curr.push_back(corners[i]);
            }
        }

        cv::Mat display = frame.clone();
        if (good_prev.size() >= 4) {
            cv::Mat inliner_mask;
            cv::Mat homography = cv::findHomography(good_prev, good_curr, inliner_mask, cv::RANSAC);
            if (!homography.empty()) {
                int inliers = 0;
                for (size_t i = 0; i < good_curr.size(); i++) {
                    bool is_inlier = inliner_mask.at<uchar>(i);
                    inliers += is_inlier;
                    cv::circle(display, good_curr[i], 3,
                               is_inlier ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), -1);
                }
                std::cout << "inliers: " << inliers << "/" << good_curr.size() << " ("
                          << 100.0 * inliers / good_curr.size() << "%)\n";
            }
        }

        cv::imshow("motion", display);

        prev_gray_frame = gray_frame;
        prev_corners = good_curr.size() < kMinTrackedPoints ? detectCorners(gray_frame) : good_curr;

        int key = cv::waitKey(delay);
        if (key == 27 || key == 'q') break;
    }

    return 0;
}
