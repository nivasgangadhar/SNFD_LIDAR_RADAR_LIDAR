#include <numeric>
#include "matching2D.hpp"

using namespace std;

// Find best matches for keypoints in two camera images based on several matching methods
void matchDescriptors(std::vector<cv::KeyPoint> &kPtsSource, std::vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef,
                      std::vector<cv::DMatch> &matches, std::string descriptorType, std::string matcherType, std::string selectorType)
{
    // configure matcher
    bool crossCheck = false;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0)
    {
        
        int normType = descriptorType.compare("DES_BINARY") == 0 ? cv::NORM_HAMMING : cv::NORM_L2;
        matcher = cv::BFMatcher::create(normType, crossCheck);
    }
    else if (matcherType.compare("MAT_FLANN") == 0)
    {
        
        descSource.convertTo(descSource, CV_32F);
        descRef.convertTo(descRef, CV_32F);
        
        matcher=cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)

        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
       /* double minDistRatio=0.8;
        for(auto it=matches.begin(); it!=matches.end();++it)
        {
            if(it->distance<minDistRatio*((it+1)->distance))
            {
                matches.push_back(*it);
            }
        }*/
    }
    else if (selectorType.compare("SEL_KNN") == 0)
    { // k nearest neighbors (k=2)

        vector<vector<cv::DMatch>> knn_match;
        matcher->knnMatch(descSource,descRef,knn_match,2);
        double minDescDistRatio = 0.8;
        for (auto it = knn_match.begin(); it != knn_match.end(); ++it)
        {

            if ((*it)[0].distance < minDescDistRatio * (*it)[1].distance)
            {
                matches.push_back((*it)[0]);
            }
        }
        cout<<"Number of mathes with distance ratio 0.8 is      "<<matches.size()<<" mathes"<<endl;
        

    }

}

// Use one of several types of state-of-art descriptors to uniquely identify keypoints
void descKeypoints(vector<cv::KeyPoint> &keypoints, cv::Mat &img, cv::Mat &descriptors, string descriptorType)
{
    // select appropriate descriptor
    
    if (descriptorType.compare("BRISK") == 0)
    {
        cv::Ptr<cv::DescriptorExtractor> extractor;

        int threshold = 30;        // FAST/AGAST detection threshold score.
        int octaves = 3;           // detection octaves (use 0 to do single scale)
        float patternScale = 1.0f; // apply this scale to the pattern used for sampling the neighbourhood of a keypoint.

        extractor = cv::BRISK::create(threshold, octaves, patternScale);
        double t = (double)cv::getTickCount();
        extractor->compute(img, keypoints, descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
    }
    else if (descriptorType.compare("SIFT")==0)
    {

        cv::Ptr<cv::SiftDescriptorExtractor> extractor=cv::SiftDescriptorExtractor::create();
        double t = (double)cv::getTickCount();
        extractor->compute(img, keypoints, descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
        
    }
    else if(descriptorType.compare("BRIEF")==0)
    {
        cv::Ptr<cv::xfeatures2d::BriefDescriptorExtractor> extractor=cv::xfeatures2d::BriefDescriptorExtractor::create();
        double t = (double)cv::getTickCount();
        extractor->compute(img, keypoints, descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
    }

    else if(descriptorType.compare("ORB")==0)
    {
        cv::Ptr<cv::DescriptorExtractor> extractor=cv::ORB::create();
        double t = (double)cv::getTickCount();
        extractor->compute(img, keypoints, descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
    }

    else if(descriptorType.compare("FREAK")==0)
    {
        cv::Ptr<cv::xfeatures2d::FREAK> extractor=cv::xfeatures2d::FREAK::create();
        double t = (double)cv::getTickCount();
        extractor->compute(img, keypoints, descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
    }
    else if(descriptorType.compare("AKAZE")==0)
    {
        cv::Ptr<cv::AKAZE> extractor=cv::AKAZE::create();
        double t = (double)cv::getTickCount();
        extractor->compute(img, keypoints, descriptors);
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
    }
    

    // perform feature description
    /*double t = (double)cv::getTickCount();
    extractor->compute(img, keypoints, descriptors);
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;*/
}

// Detect keypoints in image using the traditional Shi-Thomasi detector
void detKeypointsShiTomasi(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
{
    // compute detector parameters based on image size
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;

    // Apply corner detection
    double t = (double)cv::getTickCount();
    vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, false, k);

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it)
    {

        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Shi-Tomasi detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Shi-Tomasi Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsHarris(std::vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
{
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;
    bool harrismethod=true;

    // Apply corner detection
    double t = (double)cv::getTickCount();
    vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, harrismethod, k);//using harris extraction of keypoints

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it)
    {

        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Harris keypoint detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Harris Corner Detector Results";
        cv::namedWindow(windowName, 7);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}


void detKeypointsFAST(std::vector<cv::KeyPoint>&keypoints,cv::Mat &img,bool bVis)
{
    int threshold = 30;                                                              
    bool bNMS = true;                                                                
    cv::Ptr<cv::FeatureDetector> detector = cv::FastFeatureDetector::create(threshold, bNMS,cv::FastFeatureDetector::TYPE_9_16);
    double t = (double)cv::getTickCount();
    detector->detect(img, keypoints);
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "FAST with n= " << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "FAST keypoint detector";
        cv::namedWindow(windowName, 8);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsBRISK(std::vector<cv::KeyPoint>&keypoints,cv::Mat &img,bool bVis)
{
    cv::Ptr<cv::FeatureDetector> detector = cv::BRISK::create();
    double t = (double)cv::getTickCount();
    detector->detect(img, keypoints);
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "BRISK detector with n= " << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "BRISK keypoint detector";
        cv::namedWindow(windowName, 9);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsSIFT(std::vector<cv::KeyPoint>&keypoints,cv::Mat &img,bool bVis)
{
    cv::Ptr<cv::SiftFeatureDetector> detector=cv::SiftFeatureDetector::create();
    double t=(double)cv::getTickCount();
    detector->detect(img,keypoints);
    t=((double)cv::getTickCount()-t)/cv::getTickFrequency();
    cout<<"SIFT detector with n= "<<keypoints.size()<<" keypoints in "<<1000*t/1.0<<" ms"<<endl;
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "BRISK keypoint detector";
        cv::namedWindow(windowName, 10);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsORB(std::vector<cv::KeyPoint>&keypoints,cv::Mat &img,bool bVis)
{
    cv::Ptr<cv::FeatureDetector> detector=cv::ORB::create();
    double t=(double)cv::getTickCount();
    detector->detect(img,keypoints);
    t=((double)cv::getTickCount()-t)/cv::getTickFrequency();
    cout<<"ORB detector with n= "<<keypoints.size()<<" keypoints in "<<1000*t/1.0<<" ms"<<endl;
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "ORB keypoint detector";
        cv::namedWindow(windowName, 11);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}
void detKeypointsAKAZE(std::vector<cv::KeyPoint>&keypoints,cv::Mat &img,bool bVis)
{
    cv::Ptr<cv::FeatureDetector> detector=cv::AKAZE::create();
    //cv::Mat descriptors;
    double t=(double)cv::getTickCount();
    //detector->detectAndCompute(img,cv::noArray(),keypoints,descriptors)
    detector->detect(img,keypoints,cv::noArray());
    t=((double)cv::getTickCount()-t)/cv::getTickFrequency();
    cout<<"AKAZE detector with n= "<<keypoints.size()<<" keypoints in "<<1000*t/1.0<<" ms"<<endl;
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "AKAZE keypoint detector";
        cv::namedWindow(windowName, 12);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}




/*void detKeypointsHarris(std::vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)

{
    int blockSize = 2;     // for every pixel, a blockSize ?? blockSize neighborhood is considered
    int apertureSize = 3;  // aperture parameter for Sobel operator (must be odd)
    int minResponse = 100; // minimum value for a corner in the 8bit scaled response matrix
    double k = 0.04;       // Harris parameter (see equation for details)

    // Detect Harris corners and normalize output
    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros(img.size(), CV_32FC1);
    double t = (double)cv::getTickCount();
    cv::cornerHarris(img, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    //cv::convertScaleAbs(dst_norm, dst_norm_scaled);
    //vector<cv::KeyPoint> keypoints;
    double maxOverlap = 0.0;
    for (size_t j = 0; j < dst_norm.rows; j++)
    {
        for (size_t i = 0; i < dst_norm.cols; i++)
        {
            int response = (int)dst_norm.at<float>(j, i);
            if (response > minResponse)
            { // only store points above a threshold

                cv::KeyPoint newKeyPoint;
                newKeyPoint.pt = cv::Point2f(i, j);
                newKeyPoint.size = 2 * apertureSize;
                newKeyPoint.response = response;

                // perform non-maximum suppression (NMS) in local neighbourhood around new key point
                bool bOverlap = false;
                for (auto it = keypoints.begin(); it != keypoints.end(); ++it)
                {
                    double kptOverlap = cv::KeyPoint::overlap(newKeyPoint, *it);
                    if (kptOverlap > maxOverlap)
                    {
                        bOverlap = true;
                        if (newKeyPoint.response > (*it).response)
                        {                      // if overlap is >t AND response is higher for new kpt
                            *it = newKeyPoint; // replace old key point with new one
                            break;             // quit loop over keypoints
                        }
                    }
                }
                if (!bOverlap)
                {                                     // only add new key point if no overlap has been found in previous NMS
                    keypoints.push_back(newKeyPoint); // store new keypoint in dynamic list
                }
            }
        } // eof loop over cols
    }     // eof loop over rows

    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Harris detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "HARRIS method of keypoint extraction";
        cv::namedWindow(windowName, 7);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }

}*/