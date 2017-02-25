#include <stdio.h>
#include <opencv3/opencv.hpp>
#include "Eyes.hpp"

// KERNEL_WIDTH must be a positive, odd integer
#define KERNEL_WIDTH 5
// Tweaking these next parameters is a fairly inflexible way of affecting the dot counting;
// eventually filteredNumberOfCircles should probably be implemented better, making the 
// counter more accurate.
#define MINIMUM_OFFSET 3
#define MINIMUM_RADIUS 0
#define MAXIMUM_RADIUS 0
// These are black magic or something. Parameters for the internal Canny edge detector (used in HoughCircles).
#define LOWER_THRESHOLD 100
#define UPPER_THRESHOLD 200

static unsigned Eyes::filteredNumberOfCircles(vector<cv::Vec3f> circles) {
	unsigned count = 0;
	size_t length = circles.size();
	for(size_t i = 0; i < length; i++) {
		cv::Vec3f circle = circles[i]; // (x, y, r)
		float x = circle[0];
		// Filter out the outer 10 pixels in every direction, just in case. This could probably be done earlier (i.e. before processing the image).
		count += (!((x >= 150 || x <= 10) || (y >= 110 || y <= 10)));
	}
	return count; // This should probably look something like `count > 6 ? 6 : count;`, but that'll inhibit debugging a little
}
static unsigned Eyes::numberOfDots(void) { // An unsigned integer makes more sense here than does a signed one, but error reporting becomes harder
	Mat image = cv::imdecode(Camera::takePicture());
	vector<cv::Vec3f> circles; // HoughCircles will assign to this values of (x, y, r)
	cv::GaussianBlur(image, image, cv::Size(KERNEL_WIDTH, KERNEL_WIDTH), 0, 0); // (input, output, kernel size, sigma_x, sigma_y)
	cv::HoughCircles(image, circles, CV_HOUGH_GRADIENT, 1, MINIMUM_OFFSET, UPPER_THRESHOLD, LOWER_THRESHOLD, MINIMUM_RADIUS, MAXIMUM_RADIUS);
	return filteredNumberOfCircles(circles);
}