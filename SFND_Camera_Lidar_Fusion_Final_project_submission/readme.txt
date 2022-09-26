TASK: FP.1


matchBoundingBoxes:

In outer loop iterating over through all the kepoint matches in the current frame.
extract the keypoints of the previous frame with queryIdx and current frame by trainIdx which is in cv::keypoint type. later extract the points of each frame in point2f by .pt method.
Now loop over the bounding boxes of the previous frame and check if the previous frame keypoint is present in that bounding box by using .roi.contains() method. If so in the inner loop check the same for the bounding boxes of the current frame using same .roi.contains() method. Then count how many points are present in this bounding box(both prev and current) then up store the number of keypoints in the matrix which is of size prevFrame.boundingBoxes.size(); and currFrame.boundingBoxes.size();
This will give the matrix which has number of keypoints for each bounding box which can be indexed.
Loop over this matrix and check for the max value in the row and store the column number. Do this for all the rows and later return the pair or the map of row number with max value index of the column. Return this map.
For example the output of mapping look like this

 r    c
[0] = 0
[1] = 1
[2] = 2
[3] = 3
[4] = 4
[5] = 5
[6] = 0
[7] = 1
[8] = 5

 








TASK: FP.2


computeTTCLidar:

extract the x coordinate value of the lidar points in previous frame and current frame. Note that the lidar points are only extracted for one bounding box which was extracted by FP.1 and further steps in the main function for loop.

Later sort them based on the x coordinate value for both current and previous frame lidar points. Take the median of x coordinate of lidar point in current and previous frame.

calculate the TTC using the fromula  TTC = medCur * (1/frameRate) / (medPrev - medCur);










TASK: FP.3

clusterKptMatchesWithROI:

Loop over the kptMatches and extract the prev keypoints and current keypoints using query Idx and trainIdx.Check if these points present in the bounding box with .roi.contains method.
if these currkpt and prevkpt are both present in the boudingBox then add this to vector lists and add also add  the kptmathes list. (list_prevkpts , list_currkpts , list_matches)
Calculate the euclidain distance between these list_prevkts and list_currkpts by looping over the size of the list_matches. Euclidian distance is calculated using cv::norm.
push all of these distance to a vector of double named euclidianDIsts;

sort these distances and calculate the median among all these distance values in a vector of double.
Later check each distance value is greater than ( median distance value - tolerance) and distance value is less than( median distance value + tolerance);
One can adujust tolerance from 1 to 3. I have checked for 1 to 3 and TTC is good when tolerance is 3.0 and comparable to TTC lidar.
push the kptMacths only if the above condition satisfied. kptmathces are pushed to the BoundingBOX struct.










TASK: FP.4

computeTTCCamera:

Now loop over the kptMatches that were found in the last task and added to BoudningBox struct. Get the current and previous keypoint using .trainIdx and .queryIdx method. 
In the inner loop with new iterator for the same kptmathes extract again the current and previous keypoint using .trainIdx and queryIdx method. 
Calculate the euclidian distance b/w the outloop point and inner loop point. Do this for both previous and current keypoints. Later calculate the distance ratio b/w current euclidian distance and previous euclidian distance. Make sure to not divide 0/0 and current distance should be atlease 100 by implementing if loop to checking this condition(distPrev > std::numeric_limits<double>::epsilon() && distCurr >= minDist).

Push this distance ratio to vector of double which is now containing the distance ratio values. 

Sort these distance ratios. calculate the median for these distance ratio vector.

calculate the TTC using this formula TTC = -dT / (1 -medianDistanceRatio);










TASK: FP.5

Based on the obseravation of the TTC lidar, the measurements have not much faulty in it except the fact that it is using the constant velocity model to calculate the TTC. This method is not reasonable for example when the car is accelerating or decelerating.

During the implementation with the constant velocity model 2 or 3 frames had the bad results in TTC. This was because the lidar points are located on the other objects which was suppose to be on the preceding vehicle.

The measurment accuracy is correlated to reflectiveness of each Lidar point choosen for TTC. And a small number of points located behind the tailgate are very problematic.










TASK: FP.6

I would suggest Fast detector with different descriptor combination. Becasue its combination computation time is less in comparision to other detector/descriptor combination as well as the TTC results are stable and comparable to TTC Lidar for each frame.

At some places the camera based TTC is way off for example when the faulty(false) mathes occurs. This will lead to the random value of TTC in that frame. However we are remvoing all the outliers but still there exists some of the faulty matches.

In real world if the camera is not stable in situations like speed brakers or sudden steep or up, this will lead to the false keypoint matches and hence the TTC results are bad. 

constant velocity models are not reliable hence we can use constant acceleration model.

as we know d0/d1= h1/h0; and d1/d2= h2/h1 ;
acceleration model can be derived for TTC    TTC= -2 &t/((h1/h2)-(h1/h0));
