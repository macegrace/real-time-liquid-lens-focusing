/* Title: Real-time Liquid Lens Focusing            */
/* Program: Non-embedded camera system auto-focus   */
/* Module name: BackgroundProcessing                */
/* Module author: Martin Zaťovič                    */
/* Date: 4/2022                                     */
/* Description: Class for thread handling. Puts     */
/* frames into a buffer for displaying and image    */ 
/* processing. Different threads are used to        */
/* execute these tasks.                             */

#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>

class BackgroundProcessing {
	public:
		BackgroundProcessing(std::function <void(cv::Mat)> processFrameFnc) : processFrameFnc(processFrameFnc), finished(false){
			thread = std::make_shared<std::thread>(&BackgroundProcessing::run, this);
		}
		~BackgroundProcessing(){
			finished = true;    // set flag
			thread->join();     // destroy thread
		}
		void frameReady(cv::Mat mat){
			std::lock_guard<std::mutex> guard(m);
			pending.push_back(mat.clone());     // put frame into pending buffer
		}
	private:
		void run(){     // test capture
			while (!finished){
					
				std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait before next capture
				cv::Mat current;
				{
					std::lock_guard<std::mutex> guard(m);
					if (!pending.empty()){
						current = pending.back();
						pending.clear();
					}
				}
				if (!current.empty()){
					processFrameFnc(current);
				}
			}
			 
		}
	private:
		
		std::function <void(cv::Mat)> processFrameFnc;
		bool finished;
		std::deque <cv::Mat> pending;
		std::shared_ptr<std::thread> thread;
		std::mutex m;
};

