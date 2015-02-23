#include "stdafx.h"
#include "ImageHandler.h"
#include "Snapshot.h"

using namespace std;


vector<string> getFileNames(LPCWSTR folder)
{
    vector<string> names;
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(folder, &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do {
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
				wchar_t* buf = fd.cFileName;
				//TODO: autoadjust stringlength "17" according to filenames
				wstring wstr(buf,buf+17);
				string str(wstr.begin(),wstr.end());
				names.push_back(str);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}


vector<int> getSelectedNumbers(string input, int numofimgpairs){
	int size = input.size();
	vector<int> result;
	if(size==0){//return all
		for(int i=0;i<numofimgpairs;i++){
			result.push_back(i);
		}
	}
	else{
		//split inputstring
		istringstream stream(input);
		vector<string> tokens;
		for (string each; getline(stream, each, ','); tokens.push_back(each));
		//extract ints from tokens
		for(int i=0;i<tokens.size();i++){
			istringstream substream(tokens[i]);
			vector<string> subtokens;
			for (string each; getline(substream, each, '-'); subtokens.push_back(each));
			if(subtokens.size()==1){
				result.push_back(atoi(subtokens[0].c_str()));
			}
			else{
				int first = atoi(subtokens[0].c_str());
				int last = atoi(subtokens[1].c_str());
				for(int j=first;j<last+1;j++){
					result.push_back(j);
				}
			}
		}
	}
	return result;
}



int _tmain(int argc, _TCHAR* argv[])
{
	ImageHandler hand = ImageHandler();
	int input=0;
	cout<<"Oculus Rift meets AR - Calibration Tool"<<"\n"<<"\n";
	while(true){
		input=0;
		cout<<"\n1 - Take snapshots \n2 - Load images \n3 - Keypoint extraction and matching \n4 - Approximating the epipolar geometry \n5 - Visualize epipolar geometry \n";
		cin>>input;
		if(input==1){
			Snapshot snap;
			cout<<"Starting snapshot tool...\n";
			snap.startCapture();
		}
		else if(input==2){
			cout<<"\nSearching for images in ../media/images \n";
			vector<string> filenames = getFileNames(L"..\\media\\images\\*.*");
			cout<<filenames.size()/2<<" image pairs found: \n";
			if(filenames.size()!=0){
				for(int i=0;i<filenames.size();i++){
					cout<<filenames[i]<<", ";
				}
				cout<<"\n\nChoose image pairs for calibration ([]=all or e.g. 1-5,7,11-17) \n";
				string selection;
				vector<int> selected;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				if(cin.peek() == '\n'){
					for(int i=0;i<filenames.size()/2;i++){
						selected.push_back(i+1);
					}
				}
				else if(cin>>selection){
					selected = getSelectedNumbers(selection, filenames.size()/2);
				}
				
				cout<<"\nLoading selected images...";
				for(int i=0;i<selected.size();i++){
					string pathleft = "../media/images/"+filenames[2*(selected[i]-1)];
					string pathright = "../media/images/"+filenames[2*selected[i]-1];
					hand.loadImagePair(pathleft,pathright);
				}
				cout<<"  done.\n"<<"Undistort the selected images according to the intrinsic calibration parameters in calib_results_CAM*.txt...";
				hand.undistortAndRotate();
				cout<<"  done.\nPress Enter\n";
				cin.ignore();
				cin.get();
			}
			else{
				cout<<"To create image pairs for calibration use option 1.\nPress Enter\n";
				cin.ignore();
				cin.get();
			}
		}
		else if(input==3){
			int method;
			cout<<"\n1 - Load matches\n2 - Find new keypoints and matches\n3 - Save matches\n";
			cin>>method;
			if(method==1){
				hand.loadMatches();
			}
			else if(method==2){
				if(hand.numOfImages==0){
					cout<<"\nNo images have been loaded - use option 2 before matching.\nPress Enter\n";
					cin.ignore();
					cin.get();
				}
				else{
					cout<<"\nChoose a method for keypoint extraction and (basic) matching:\n"<<"1 - SIFT features + kNN-Matcher\n"<<"2 - Chessboardmatching\n";
					cin>>method;
					if(method==1){
						cout<<"\nDo you want to check the matches manually? (1=Yes,2=No)\n";
						cin>>method;
						if(method==1){
							hand.calculateMatches(1);
						}
						else if(method==2){
							hand.calculateMatches(2);
						}
					}
					else if(method==2){
						hand.calculateMatches(3);
					}
				}
			}
			else if(method==3){
				hand.saveMatches();
				cout<<"Matches saved.\nPress Enter\n";
				cin.ignore();
				cin.get();
			}
		}
		else if(input==4){
			cout<<"\n"<<"1 - Load previously determined epipolar geometry\n"<<"2 - New approximation of epipolar geometry\n"<<"3 - Save current epipolar geometry\n";
			cin>>input;
			if(input==1){
				hand.loadEpiGeo();
			}
			else if(input==2){
				if(hand.leftMatch.size()<8){
					cout<<"\nERROR: Not enough matches in order to approximate epipolar geometry. Generate matches first.";
				}
				else{
					int method;
					cout<<"\nChoose a method for estimation of the fundamental matrix\n"<<"1 - Hartley (8 Point/RANSAC)\n"<<"2 - ZMEK 2011 (using RANSAC)\n";
					cin>>method;
					if(method==1||method==2){
						hand.estimateGeometry(method);
					}
				}
			}
			else if(input==3){
				hand.saveEpiGeo();
			}
			cout<<"\nPress Enter\n";
			cin.ignore();
			cin.get();
		}
		else if(input==5){
			hand.visualize();
		}
	}
}