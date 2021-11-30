#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "C:/opencvCodeblocks/opencv_contrib-4.2.0/modules/xfeatures2d/include/opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include "C:/opencvCodeblocks/opencv_contrib-4.2.0/modules/xfeatures2d/include/opencv2/xfeatures2d/nonfree.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/stitching.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

/** @function main */

int main(int argc, char** argv)

{
    //Here, select a set of images by changing the number to either 1 or 2 or 3
    const string setNumber = "4";

    //First, load the tile-images that we need to make into a Panorama
    Mat img_1 = imread("InputImages/Set" + setNumber + "/Image1.png");
    Mat img_2 = imread("InputImages/Set" + setNumber + "/Image2.png");


    //Next, we detect the keypoints using Agast Feature Detector
    Ptr<AgastFeatureDetector> detector = AgastFeatureDetector::create();
    std::vector<KeyPoint> keypoints_1, keypoints_2;

    //Detect Keypoints using our feature detector
    detector->detect(img_1, keypoints_1);
    detector->detect(img_2, keypoints_2);

    //Now, we draw the detected keypoints for visual purposes
    Mat img_keypoints_1;
    Mat img_keypoints_2;

    //Drawing the keypoints
    drawKeypoints(img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

    //Now, we show the detected and drawn keypoints
    imshow("Keypoints 1", img_keypoints_1);
    imshow("Keypoints 2", img_keypoints_2);

    //We will also save the images for later reference
    imwrite("OutputImages/Set" + setNumber + "/Image1_Keypoints.png", img_keypoints_1);
    imwrite("OutputImages/Set" + setNumber + "/Image2_Keypoints.png", img_keypoints_2);

    //Then, we create our feature descriptor, in this case we are using FREAK
    //We don't need to set any parameters for the FREAK detector
    Ptr<FREAK> desc_comp = FREAK::create();
    Mat desc1, desc2;

    //Compute the feature descriptors for each image
    desc_comp->compute(img_1, keypoints_1, desc1);
    desc_comp->compute(img_2, keypoints_2, desc2);

    //Next, we need to match the descriptors in each image
    //We chose BFMatcher, Brute-Force Matcher to try all the possible matches and find the best ones
    Ptr<cv::DescriptorMatcher> bfMatcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE);
    vector<vector<DMatch>> matches;

    //Now, we use the knnMatch function to find the two nearest matches
    //It basically finds the k best matches for each descriptor from a query set, in this case 2
    bfMatcher->knnMatch(desc1, desc2, matches, 2);

    //Then, we define a vector that contains the actual/useful good matches
    vector<cv::DMatch> validMatches;

    //We need a certain threshold/ratio to identify the "good matches"
    //Through tests and experiments, we found that a threshold of 0.75 was valid
    const float thresholdValue = 0.75f;

    //We iterate over the matches and compare each one against our ratio
    //The matches that comply with our ratio are then pushed into our "valid matches" vector
    for (int i = 0; i < matches.size(); ++i){
        if (matches[i][0].distance < thresholdValue * matches[i][1].distance){
            validMatches.push_back(matches[i][0]);
        }
    }

    //Now we define two vectors, one for each image, where we store each image's best matches
    std::vector< Point2f > image1ValidMatches;
    std::vector< Point2f > image2ValidMatches;

    //Then, we iterate over the "valid matches" and save each image's
    //best matches into the previously defined vectors

    for (int i = 0; i < validMatches.size(); i++){

        //Obtaining the keypoints from the good matches in the first image
        image1ValidMatches.push_back(keypoints_1[validMatches[i].queryIdx].pt);

        //Obtaining the keypoints from the good matches in the second image
        image2ValidMatches.push_back(keypoints_2[validMatches[i].trainIdx].pt);
    }

    //Now, we draw the valid/good matches
    Mat img_validMatches;
    drawMatches(img_1, keypoints_1, img_2, keypoints_2, validMatches, img_validMatches, Scalar::all(-1), Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //Showing the valid matches
    //Also saving them for later reference
    imshow("Valid matches", img_validMatches);
    imwrite("OutputImages/Set" + setNumber + "/Valid_matches_Set" + setNumber + ".png", img_validMatches);

    //Now, we define the Homography matrix for the two images, where H will be the transformation
    cv::Mat H;

    //Then, we compute this Homography Matrix for the pair of images
    //Here, we used RANSAC to remove the outliers
    H = findHomography(image1ValidMatches, image2ValidMatches, RANSAC);

    //Now, we need to stitch these images using the computed homography matrix
    //The first step is to define a matrix that will contain our result
    cv::Mat result;

    //Next, we use warpPerspective to transform the source image using the specified matrix
    //It basically applies a perspective transformation to an image
    warpPerspective(img_1, result, H, cv::Size(img_1.cols + img_2.cols, img_1.rows));
    cv::Mat half(result, cv::Rect(0, 0, img_2.cols, img_2.rows));
    img_2.copyTo(half);

    //Now that we have stitched our images together, we are ready to display the final result
    //We will also save the images for later reference
    imshow("Final result: Images stitched", result);
    imwrite("OutputImages/Set" + setNumber + "/Panorama.jpeg", result);

    //Next, we will remove the "black" region from the image by finding the largest contour and applying masks
    Mat img_gray;
    img_gray = result.clone();
    img_gray.convertTo(img_gray, CV_8UC1);
    cvtColor(img_gray, img_gray, COLOR_BGR2GRAY);

    //We set the threshold to gray
    threshold(img_gray, img_gray, 25, 255, THRESH_BINARY);

    //We create a vector that will store the contour
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //Then, we use the function findContours to find the contours in the image
    findContours(img_gray, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

    //Define some variables that will be useful for finding the biggest area and later applying a mask
    int biggestArea = 0;
    int biggestContourIndex = 0;
    Rect boundsRect;

    //Then, we iterate over each contour
    for (int i = 0; i< contours.size(); i++){

        //First, we find the area of the contour
        double currentArea = contourArea(contours[i], false);

        if (currentArea>biggestArea){

                biggestArea = currentArea;

                //We store the index of largest contour
                biggestContourIndex = i;

                //Then, we find the bounding rectangle for biggest contour
                boundsRect = boundingRect(contours[i]);
        }
    }

    //Finally, we apply our mask
    result = result(Rect(boundsRect.x, boundsRect.y, boundsRect.width, boundsRect.height));

    //Now that we have cropped our image, we are ready to display the final result
    //We will also save the images for later reference
    imshow("Final result: Images stitched and cropped", result);
    imwrite("OutputImages/Set" + setNumber + "/PanoramaCropped.jpeg", result);

    waitKey(0);
    return 0;
}
