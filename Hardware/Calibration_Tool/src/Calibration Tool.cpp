#include "stdafx.h"
#include "ImageHandler.h"
#include "Snapshot.h"

#include <conio.h>

using namespace std;

//returns all filenames of files in input folder
vector<string> getFileNames(LPCWSTR folder){
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

//returns the selected Numbers from an inputstring like "1,3-7,14" with numofimgpairs as upper limit
vector<int> getSelectedNumbers(string input, int numOfImgPairs){
	int size = input.size();
	vector<int> list;
	if(size==0){//return all
		for(int i=0;i<numOfImgPairs;i++){
			list.push_back(i);
		}
	}
	else{
		//split inputstring
		istringstream stream(input);
		vector<string> tokens;
		for (string each; getline(stream, each, ','); tokens.push_back(each));
		//extract ints from tokens
		for(unsigned int i=0;i<tokens.size();i++){
			istringstream substream(tokens[i]);
			vector<string> subtokens;
			for (string each; getline(substream, each, '-'); subtokens.push_back(each));
			if(subtokens.size()==1){
				list.push_back(atoi(subtokens[0].c_str()));
			}
			else{
				int first = atoi(subtokens[0].c_str());
				int last = atoi(subtokens[1].c_str());
				for(int j=first;j<last+1;j++){
					list.push_back(j);
				}
			}
		}
	}
	vector<int> result;
	for(unsigned int i=0;i<list.size();i++){
		if(list[i]<=numOfImgPairs && list[i]>0){
			result.push_back(list[i]);
		}
	}

	return result;
}

//Clears console
void ClrScrn() {
    COORD pos = { 0, 0};
    DWORD written;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    unsigned size = 50 * 80;
    FillConsoleOutputCharacter(console, ' ', size, pos, &written);
    SetConsoleCursorPosition(console, pos);
}



int _tmain(int argc, _TCHAR* argv[])
{
	ImageHandler hand = ImageHandler();//interface between userinput and calibration classes
	int input=0;
	bool done=false;
	while(!done){
		input=0;
		//Main menu
		cout<<"Oculus Rift meets AR - Calibration Tool - Main menu\n\n";
		cout<<"\n1 - Take snapshots \n2 - Load images \n3 - Keypoint extraction and matching \n4 - Approximating the epipolar geometry \n5 - Visualize epipolar geometry \n";
		input = _getch();
		ClrScrn();

		if(input==27){//exit
			done=true;
			continue;
		}
		else if(input=='1'){//Take snapshots
			cout<<"Oculus Rift meets AR - Calibration Tool - Snapshot tool\n\n";
			Snapshot snap;
			cout<<"\nStarting snapshot tool...\n";
			if(snap.startCapture()==-1){
				cout<<"\nPlease connect both cameras to your system. Press Enter\n";
				cin.get();
			}
		}
		else if(input=='2'){//Load images
			cout<<"Oculus Rift meets AR - Calibration Tool - Imageloader\n\n";
			cout<<"\nSearching for images in ../media/images \n";
			vector<string> filenames = getFileNames(L"..\\media\\images\\*.*");
			cout<<filenames.size()/2<<" image pairs found: \n";
			if(filenames.size()!=0){
				for(unsigned int i=0;i<filenames.size();i++){
					cout<<filenames[i]<<", ";
				}
				cout<<"\n\nChoose image pairs for calibration ([]=all or e.g. 1-5,7,11-17) \n";
				string selection;
				vector<int> selected;
				cin.clear();
				//cin.ignore(numeric_limits<streamsize>::max(), '\n');

				input = _getch();
				if(input!=27){
					char firstnum = (char)input;
					cout<<firstnum;
					cin.putback(firstnum);
					if(cin.peek() == 13){
						for(unsigned int i=0;i<filenames.size()/2;i++){
							selected.push_back(i+1);
						}
					}
					else if(cin>>selection){
						selected = getSelectedNumbers(selection, filenames.size()/2);
					}

					cin.ignore();

					if(selected.size()==0){
						cout<<"\nNo images loaded.\nPress Enter\n";
						cin.get();
					}
					else{

						cout<<"\nLoading selected images...";
						for(unsigned int i=0;i<selected.size();i++){
							string pathleft = "../media/images/"+filenames[2*(selected[i]-1)];
							string pathright = "../media/images/"+filenames[2*selected[i]-1];
							hand.loadImagePair(pathleft,pathright);
							cout<<selected[i]<<", ";
						}
						cout<<"\nUndistort the selected images according to the intrinsic calibration parameters in calib_results_CAM*.txt...";
						hand.undistortAndRotate();
						cout<<"  done.\nPress Enter\n";
						cin.get();
					}
				}
			}
			else{//No files found
				cout<<"To create image pairs for calibration use option 1.\nPress Enter\n";
				cin.get();
			}
		}
		else if(input=='3'){//Keypoint extraction and matching
			cout<<"Oculus Rift meets AR - Calibration Tool - Keypoint extraction and matching\n\n";
			cout<<"\n1 - Load matches\n2 - Find new keypoints and matches\n3 - Save matches\n";
			input = _getch();
			if(input=='1'){
				hand.loadMatches();
			}
			else if(input=='2'){
				if(hand.numOfImages==0){//You need images to extract keypoints from
					cout<<"\nNo images have been loaded - use option 2 before matching.\nPress Enter\n";
					cin.get();
				}
				else{
					cout<<"\nChoose a method for keypoint extraction and (basic) matching:\n"<<"1 - SIFT features + kNN-Matcher\n"<<"2 - Chessboardmatching\n3 - Chessboard + SIFT + kNN-Matcher\n";
					int method = _getch();
					if(method=='1'){
						cout<<"\nDo you want to check the matches manually? (1=Yes,2=No)\n";
						method = _getch();
						if(method=='1'){
							hand.calculateMatches(1);
						}
						else if(method=='2'){
							hand.calculateMatches(2);
						}
					}
					else if(method=='2'){
						hand.calculateMatches(3);
					}
					else if(method=='3'){
						cout<<"\nDo you want to check the matches manually? (1=Yes,2=No)\n";
						method = _getch();
						if(method=='1'){
							hand.calculateMatches(4);
						}
						else if(method=='2'){
							hand.calculateMatches(5);
						}
					}
					cout<<"Press Enter\n";
					cin.get();
				}
			}
			else if(input=='3'){
				hand.saveMatches();
				cout<<"Matches saved.\nPress Enter\n";
				cin.get();
			}
		}
		else if(input=='4'){//Epipolar geometry estimation
			cout<<"Oculus Rift meets AR - Calibration Tool - Epipolar geometry estimation\n\n";
			cout<<"\n1 - Load previously determined epipolar geometry\n2 - New approximation of epipolar geometry\n3 - Save current epipolar geometry\n";
			input=_getch();
			if(input=='1'){
				hand.loadEpiGeo();
				cout<<"\nPress Enter\n";
				cin.get();
			}
			else if(input=='2'){
				if(hand.leftMatch.size()<8){
					cout<<"\nERROR: Not enough matches in order to approximate epipolar geometry. Generate matches first.";
				}
				else{
					int method;
					cout<<"\nChoose a method for estimation of the fundamental matrix\n"<<"1 - Hartley (8 Point/RANSAC)\n"<<"2 - ZMEK 2011 (using RANSAC)\n";
					method=_getch();
					if(method=='1'||method=='2'){
						hand.estimateGeometry(method-48);
					}
				}
				cout<<"\nPress Enter\n";
				cin.get();
			}
			else if(input=='3'){
				hand.saveEpiGeo();
				cout<<"\nPress Enter\n";
				cin.get();
			}
		}
		else if(input=='5'){//Show epipolarlines and save one rectified image pair in media folder
			cout<<"Oculus Rift meets AR - Calibration Tool - Visualization\n\n";
			hand.visualize();
		}

		ClrScrn();
	}
}