#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <math.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector> 
using namespace std;

/*
 Code by OlegTim. Tested on Win7_64,  1024*768, fullscreen and windowed
devcpp 4.9.9.2  tools -compiler options - settings- linker - create console - (try n)
  tools - compiler options - compiler - add this to linker cmd -lgdi32 
*/

int pitch=45;
int roll=0;
int yaw =45;
int d = 10; //2*cube size
int ew = 5;
int ns =4;
int ud = 3;
vector<vector<vector<int> > > map;
int kx=0; //start position to draw x
int ky=0;	 //start position to draw y
int dm = 25; //menu line heightscr
int widthscr = 1024;
int heightscr = 768;
int rpre=0;  //if we need to change color on top side
int xmin,xmax,ymin,ymax,sx,sy,c3;
int mx =0; int my=0;
int scrx =0; int scry=0;
int scrx2 = 768; int scry2=768;
int yesscr, stepnums, delay, dpitch, droll,dyaw,dmx,dmy,dd;

HINSTANCE hInstance;
HWND hwnd;
HWND pitch_edit; 
HWND roll_edit; 
HWND  jaw_edit;
HWND track_pitch;
HWND track_roll;
HWND track_yaw;

int r,g,b,r1,g1,b1,r2,g2,b2; 
int xa1,xa2,xa3,xa4,xb1,xb2,xb3,xb4,xc1,xc2,xc3,xc4;
int ya1,ya2,ya3,ya4,yb1,yb2,yb3,yb4,yc1,yc2,yc3,yc4;
int cx,cy,cb,ce,cd,c2x,c2y,c2b,c2e,c2d,c3x,c3y,c3b,c3e,c3d;
string s = "";
int c3xk = 1; int c3yk = -1; // z reverse
string filename="nofile";
int scrname = 1; //rewrites on new session

void showinfo(){
MessageBox(hwnd,"Press <Enter> or 'show' to apply. <Esc> to quit.\n\
M and < to pitch. > and ? to roll. Tab/Caps for jaw. \nArrows to scroll. \n\
In app dir map_rotate.txt Tab separated: pitch, roll, yaw,\n\
cube_size, menu_size, x_scroll, y_scroll, file_name.\n\
SPACE for screenshot.\n\
You can open maps of Creative strategy (which is on java7)\n\n\
For step shots you need to create map_rotate2.txt\n\
Write 13 variables, separated with Tab: \n\
1/0 to make/not screenshot.\n\
number of steps (-1 unlim) sleep(ms)\n\
screen xleft ytop xright ydown\n\
delta for pitch roll yaw\n\
delta (pix) xmove ymove zoom",
"info_map_rotate_by_OlegTim",MB_OK); 	 
	 }

void stdvals(){pitch=45; roll=0; yaw =45; d = 10; mx=0; my=0;}

void stepgo(){
	ifstream file("./map_rotate2.txt");
if (file.is_open()) {	     					
    std::string line;
    while (std::getline(file, line)) {
        line.c_str();
        vector<string> arr;
  string delim("\t");
  size_t prev = 0;
  size_t next;
  size_t delta3 = delim.length();
  while( ( next = line.find( delim, prev ) ) != string::npos ){
    arr.push_back( line.substr( prev, next-prev ) );
    prev = next + delta3;
  }
arr.push_back( line.substr(prev ) );
yesscr =atoi(arr[0].c_str());
stepnums =atoi(arr[1].c_str());
delay=atoi(arr[2].c_str());
scrx=atoi(arr[3].c_str());
scry=atoi(arr[4].c_str());
scrx2=atoi(arr[5].c_str());
scry2=atoi(arr[6].c_str());
dpitch=atoi(arr[7].c_str());
droll=atoi(arr[8].c_str());
dyaw=atoi(arr[9].c_str());
dmx=atoi(arr[10].c_str());
dmy=atoi(arr[11].c_str());
dd=atoi(arr[12].c_str());
    }
file.close();} else{MessageBox(hwnd,"map_rotate2.txt  not found","e",MB_OK);}

void scrshot();  void go2();
while(stepnums!=0){//
if (yesscr!=0){scrshot();}
pitch+=dpitch;
roll+=droll;
yaw+=dyaw;
mx+=dmx;
my+=dmy;
d+=dd;
go2();	SendMessage(hwnd,WM_PAINT,0,0); 			   
Sleep(delay);
if (stepnums!=-1){stepnums--;}
				   }//
	 }

inline int GetFilePointer(HANDLE FileHandle){
    return SetFilePointer(FileHandle, 0, 0, FILE_CURRENT);
}
 
bool SaveBMPFile(char *filename, HBITMAP bitmap, HDC bitmapDC, int width, int height){
    bool Success=0;
    HBITMAP OffscrBmp=NULL;
    HDC OffscrDC=NULL;
    LPBITMAPINFO lpbi=NULL;
    LPVOID lpvBits=NULL;
    HANDLE BmpFile=INVALID_HANDLE_VALUE;
    BITMAPFILEHEADER bmfh;
    if ((OffscrBmp = CreateCompatibleBitmap(bitmapDC, width, height)) == NULL)
        return 0;
    if ((OffscrDC = CreateCompatibleDC(bitmapDC)) == NULL)
        return 0;
    HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
    BitBlt(OffscrDC, 0, 0, width, height, bitmapDC, 0, 0, SRCCOPY);
    if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)])) == NULL) 
        return 0;
    ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    SelectObject(OffscrDC, OldBmp);
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
        return 0;
    if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
        return 0;
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi, DIB_RGB_COLORS))
        return 0;
    if ((BmpFile = CreateFileA(filename,
                        GENERIC_WRITE,
                        0, NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL)) == INVALID_HANDLE_VALUE)
        return 0;
    DWORD Written;
    bmfh.bfType = 19778;
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        return 0;
    if (Written < sizeof(bmfh)) 
        return 0; 
    if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER), &Written, NULL)) 
        return 0;
    if (Written < sizeof(BITMAPINFOHEADER)) 
        return 0;
    int PalEntries;
    if (lpbi->bmiHeader.biCompression == BI_BITFIELDS) 
        PalEntries = 3;
    else PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
                      (int)(1 << lpbi->bmiHeader.biBitCount) : 0;
    if(lpbi->bmiHeader.biClrUsed) 
    PalEntries = lpbi->bmiHeader.biClrUsed;
    if(PalEntries){
    if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD), &Written, NULL)) 
        return 0;
        if (Written < PalEntries * sizeof(RGBQUAD)) 
            return 0;
    }
    bmfh.bfOffBits = GetFilePointer(BmpFile);
    if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage, &Written, NULL)) 
        return 0;
    if (Written < lpbi->bmiHeader.biSizeImage) 
        return 0;
    bmfh.bfSize = GetFilePointer(BmpFile);
    SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        return 0;
    if (Written < sizeof(bmfh)) 
        return 0;
    CloseHandle (BmpFile);
    delete [] (char*)lpvBits;
    delete [] lpbi;
    DeleteDC (OffscrDC);
    DeleteObject (OffscrBmp);
    return 1;
}
 
bool ScreenCapture(int x, int y, int width, int height, char *filename, HWND hwnd){
    HDC hDC= GetDC(hwnd);
    HDC hDc = CreateCompatibleDC(hDC);    
    HBITMAP hBmp = CreateCompatibleBitmap(hDC, width, height);   
    HGDIOBJ old= SelectObject(hDc, hBmp);   
    BitBlt(hDc, 0, 0, width, height, hDC, x, y, SRCCOPY);  
    bool ret = SaveBMPFile(filename, hBmp, hDc, width, height); 
    SelectObject(hDc, old);
    DeleteObject(hBmp);  
    DeleteDC (hDc);
    ReleaseDC (hwnd, hDC);
    return ret;
}
	 
void scrshot(){
stringstream ss; ss << scrname; string str = ss.str();
str+=".bmp";	
char *cstr = &str[0]; 	 
ScreenCapture(scrx, scry, scrx2, scry2, cstr, NULL);
scrname++;
	 }	 

void stdfill(){
map[0][1][4]=1;map[0][1][3]=1;map[0][1][2]=1;map[0][1][1]=1;map[0][1][0]=1;
map[0][0][1]=1;map[0][2][1]=1;map[0][3][2]=1;map[1][1][4]=1;map[1][1][3]=1;map[2][1][4]=1;
	 }	 

void fillmap(){
  for(int i=0;i<ud;i++)
  {
    vector<vector<int> > v2;
    for(int j=0;j<ns;j++)
    {
      vector<int> v1;
      for(int k=0;k<ew;k++)
      {
        v1.push_back(0); //start value
      }
      v2.push_back(v1);
    }
    map.push_back(v2);
  }	
	 }

void optionsread(){
	 	ifstream file("./map_rotate.txt");
if (file.is_open()) {	     					
    std::string line;
    while (std::getline(file, line)) {
        line.c_str();
        vector<string> arr;
  string delim("\t");
  size_t prev = 0;
  size_t next;
  size_t delta3 = delim.length();
  while( ( next = line.find( delim, prev ) ) != string::npos ){
    arr.push_back( line.substr( prev, next-prev ) );
    prev = next + delta3;
  }
arr.push_back( line.substr(prev ) );
pitch =atoi(arr[0].c_str());
roll =atoi(arr[1].c_str());
yaw =atoi(arr[2].c_str());
d =atoi(arr[3].c_str());
dm =atoi(arr[4].c_str());
mx =atoi(arr[5].c_str());
my =atoi(arr[6].c_str());
filename = arr[7];
    }
    file.close();
}
	 }

void optionswrite(){
ofstream out3("map_rotate.txt"); 
if(!out3) {
PostQuitMessage(0); 
}
out3<<pitch<<"\t"<<roll<<"\t"<<yaw<<"\t"<<d<<"\t"<<dm<<"\t"<<mx<<"\t"<<my<<"\t"<<filename;
out3.close();	 
	 }
	 
	 void openf2(){// better in the app's folder
ifstream file(filename.c_str());
if (file.is_open()) {
map.clear();  					
    std::string line;
    int counter = -2;
    int u,n,e;
    while (std::getline(file, line)&&counter!=-3) { 
		     counter++;
line.c_str();
vector<string> arr;
  string delim(" ");
  size_t prev = 0;
  size_t next;
  size_t delta3 = delim.length();
  while( ( next = line.find( delim, prev ) ) != string::npos ){
    arr.push_back( line.substr( prev, next-prev ) );
    prev = next + delta3;
  }
arr.push_back( line.substr(prev ) );	     		     
      if (counter==-1){
ud =atoi(arr[1].c_str());
ns =atoi(arr[2].c_str());
ew =atoi(arr[3].c_str());
fillmap();
	  	 			  }else{//
u = (ud-1)-(counter/(ns));
n = counter%ns;	  
for (int i = 0; i<ew; i++){if (atoi(arr[i].c_str())==1||atoi(arr[i].c_str())==2) 
 {map[u][n][i]=1;}}
if (counter>ns*ud-2){counter=-3;}		  
					  		  }//
    }
    file.close(); void go2(); go2();
}
else{
	 filename="nofile";optionswrite();
	 }
	 }//

void openf(){
OPENFILENAME ofn;       
TCHAR szFile[260] = { 0 };      
ZeroMemory(&ofn, sizeof(ofn));
ofn.lStructSize = sizeof(ofn);
ofn.hwndOwner = hwnd;
ofn.lpstrFile = szFile;
ofn.nMaxFile = sizeof(szFile);
ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
ofn.nFilterIndex = 1;
ofn.lpstrFileTitle = NULL;
ofn.nMaxFileTitle = 0;
ofn.lpstrInitialDir = NULL;
ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

if (GetOpenFileName(&ofn) == TRUE)
{
   	filename = 	szFile;	  optionswrite();  openf2();			  					  
}
	 }

void cal4(){
if (s.find('1')!=-1&&s.find('2')!=-1&&s.find('3')!=-1){
c3x=c3xk*(xb1-xb4)*c3+kx; c3y=c3yk*(yb3-yb2)*c3+ky;}
if (s.find('1')!=-1&&s.find('2')!=-1&&s.find('4')!=-1){
c3x=c3xk*(xb1-xb4)*c3+kx; c3y=c3yk*(yb3-yb2)*c3+ky;}
if (s.find('1')!=-1&&s.find('3')!=-1&&s.find('5')!=-1){
c3x=c3xk*(xb4-xb1)*c3+kx; c3y=c3yk*(yb3-yb2)*c3+ky;}
if (s.find('1')!=-1&&s.find('4')!=-1&&s.find('5')!=-1){
c3x=c3xk*(xb4-xb1)*(c3)+kx; c3y=c3yk*(yb1-yb4)*c3+ky;}	
if (s.find('2')!=-1&&s.find('3')!=-1&&s.find('6')!=-1){
c3x=(xb4-xb1)*(ud-c3-1)+kx; c3y=(yb3-yb2)*(ud-c3-1)+ky;}
if (s.find('2')!=-1&&s.find('4')!=-1&&s.find('6')!=-1){
c3x=(xa4-xa1)*(ud-c3-1)+kx; c3y=(ya4-ya1)*(ud-c3-1)+ky;}
 if (s.find('3')!=-1&&s.find('5')!=-1&&s.find('6')!=-1){
c3x=(xa4-xa1)*(ud-c3-1)+kx; c3y=(ya4-ya1)*(ud-c3-1)+ky;}
 if (s.find('4')!=-1&&s.find('5')!=-1&&s.find('6')!=-1){
c3x=(xa1-xa4)*(ud-c3-1)+kx; c3y=(ya1-ya4)*(ud-c3-1)+ky;}													
	 }

void cal3(){
int xt1,xt2,xt3,xt4,yt1,yt2,yt3,yt4;
 xt1= xa1+sx; yt1 = ya1+sy;
 xt2=xa2+sx; yt2= ya2+sy;
 xt3=xa3+sx; yt3=ya3+sy;
 xt4=xa4+sx; yt4 = ya4+sy;
if(xt1<xmin){xmin=xt1;}if(xt1>xmax){xmax=xt1;}if(yt1<ymin){ymin=yt1;}if(yt1>ymax){ymax=yt1;}
if(xt2<xmin){xmin=xt2;}if(xt2>xmax){xmax=xt2;}if(yt2<ymin){ymin=yt2;}if(yt2>ymax){ymax=yt2;} 
if(xt3<xmin){xmin=xt3;}if(xt3>xmax){xmax=xt3;}if(yt3<ymin){ymin=yt3;}if(yt3>ymax){ymax=yt3;}
if(xt4<xmin){xmin=xt4;}if(xt4>xmax){xmax=xt4;}if(yt4<ymin){ymin=yt4;}if(yt4>ymax){ymax=yt4;}
 xt1= xb1+sx; yt1 = yb1+sy;
 xt2=xb2+sx; yt2= yb2+sy;
 xt3=xb3+sx; yt3=yb3+sy;
 xt4=xb4+sx; yt4 = yb4+sy;
if(xt1<xmin){xmin=xt1;}if(xt1>xmax){xmax=xt1;}if(yt1<ymin){ymin=yt1;}if(yt1>ymax){ymax=yt1;}
if(xt2<xmin){xmin=xt2;}if(xt2>xmax){xmax=xt2;}if(yt2<ymin){ymin=yt2;}if(yt2>ymax){ymax=yt2;} 
if(xt3<xmin){xmin=xt3;}if(xt3>xmax){xmax=xt3;}if(yt3<ymin){ymin=yt3;}if(yt3>ymax){ymax=yt3;}
if(xt4<xmin){xmin=xt4;}if(xt4>xmax){xmax=xt4;}if(yt4<ymin){ymin=yt4;}if(yt4>ymax){ymax=yt4;}
 xt1= xc1+sx; yt1 = yc1+sy;
 xt2=xc2+sx; yt2= yc2+sy;
 xt3=xc3+sx; yt3=yc3+sy;
 xt4=xc4+sx; yt4 = yc4+sy;
if(xt1<xmin){xmin=xt1;}if(xt1>xmax){xmax=xt1;}if(yt1<ymin){ymin=yt1;}if(yt1>ymax){ymax=yt1;}
if(xt2<xmin){xmin=xt2;}if(xt2>xmax){xmax=xt2;}if(yt2<ymin){ymin=yt2;}if(yt2>ymax){ymax=yt2;} 
if(xt3<xmin){xmin=xt3;}if(xt3>xmax){xmax=xt3;}if(yt3<ymin){ymin=yt3;}if(yt3>ymax){ymax=yt3;}
if(xt4<xmin){xmin=xt4;}if(xt4>xmax){xmax=xt4;}if(yt4<ymin){ymin=yt4;}if(yt4>ymax){ymax=yt4;}	 
	 }
	 
void cal2(){
cx=cy=cb=ce=cd=c2x=c2y=c2b=c2e=c2d=c3x=c3y=c3b=c3e=c3d=kx=ky=0;
int cr,c2r,c3r; cr=c2r=c3r=0; 

if (s.find('1')!=-1&&s.find('2')!=-1&&s.find('3')!=-1){
cb=0; cd=1; ce = ew;  cx=-(xa1-xa2);cy=ya2-ya1;
c2b=0;c2d=1;c2e=ns;c2x=-(ew+1)*cx+(xa2-xa1)+(xa3-xa2); c2y=-(ew+1)*cy+(ya3-ya1);
c3b=0;c3d=1;c3e=ud; c3xk = 1; c3yk=-1;
}

if (s.find('1')!=-1&&s.find('2')!=-1&&s.find('4')!=-1){
cb=ew-1;cd =-1;ce=-1;cx=(xa1-xa2);cy=-(ya2-ya1);
c2b=0;c2d=1;c2e=ns;c2x=-(ew+1)*cx+(xa4-xa1)+(xa1-xa2); c2y=-(ew+1)*cy+(ya4-ya2);
c3b=0;c3d=1;c3e=ud; c3xk = 1; c3yk=-1;
 }
 
 if (s.find('1')!=-1&&s.find('3')!=-1&&s.find('5')!=-1){
cb=0; cd=1; ce = ew;  cx=-(xa1-xa2);cy=ya2-ya1;
c2b=ns-1;c2d=-1;c2e=-1;c2x=-(ew-1)*cx-(xa2-xa1)-(xa3-xa2); c2y=-(ew-1)*cy-(ya3-ya1);
c3b=0;c3d=1;c3e=ud; c3xk = -1; c3yk=-1;
 }

  if (s.find('1')!=-1&&s.find('4')!=-1&&s.find('5')!=-1){
cb=ew-1;cd =-1;ce=-1;  cx=-(xa2-xa1);cy=(ya1-ya2);
c2b=ns-1;c2d=-1;c2e=-1;c2x=-(ew+1)*cx-(xa2-xa1)-(xa3-xa2); c2y=-(ew+1)*cy-(ya3-ya1);
c3b=0;c3d=1;c3e=ud; c3xk = 1; c3yk=-1; 	
}

if (s.find('2')!=-1&&s.find('3')!=-1&&s.find('6')!=-1){
cb=0; cd=1; ce = ew;  cx=-(xa1-xa2);cy=ya2-ya1;c2b=0;c2d=1;c2e=ns;
c2x=-(ew+1)*cx+(xa2-xa1)+(xa3-xa2)-(xb2-xb1)-(xb3-xb2); 
c2y=-(ew+1)*cy-(yb2-yb1)+(yc3-yc2);
c3b=ud-1;c3d=-1;c3e=-1; c3xk = 1; c3yk=-1;
}

if (s.find('2')!=-1&&s.find('4')!=-1&&s.find('6')!=-1){
cb=ew-1;cd =-1;ce=-1;  cx=(xa1-xa2);cy=-(ya2-ya1);
c2b=0;c2d=1;c2e=ns;c2x=-(ew+1)*cx+(xb2-xb1)+(xa1-xa2); c2y=-(ew+1)*cy+(yb2-yb1)-(yc3-yc2);
c3b=ud-1;c3d=-1;c3e=-1; c3xk = 1; c3yk=-1;
}

 if (s.find('3')!=-1&&s.find('5')!=-1&&s.find('6')!=-1){
cb=0; cd=1; ce = ew;  cx=(xb4-xb1);cy=yb4-yb1;
c2b=ns-1;c2d=-1;c2e=-1;c2x=-(ew+1)*cx+(xb3-xb1)+(xa4-xa1)+(xc2-xc1); 
c2y=-(ew+1)*cy+(yc3-yc1);
c3b=ud-1;c3d=-1;c3e=-1; c3xk = 1; c3yk=-1;
 }
 
 if (s.find('4')!=-1&&s.find('5')!=-1&&s.find('6')!=-1){
cb=ew-1;cd =-1;ce=-1;   cx=-(xc4-xc1);cy=-(yb4-yb1);
c2b=ns-1;c2d=-1;c2e=-1;
c2x=-(ew+1)*cx-(xc4-xc1)+(xc2-xc1); 
c2y=-(ew+1)*cy-(yc4-yc2);
c3b=ud-1;c3d=-1;c3e=-1; c3xk = 1; c3yk=-1;
}

xmin=ymin=99999;
xmax=ymax=-99999; 
sx=kx;  sy=ky; c3=0;

int c2, c; 
c3=c2=c=0;

c3=c3b;
while (c3!=c3e) { 
c2=c2b;
while (c2!=c2e) {
c=cb;
while (c!=ce) { 
cal3();
c+=cd;
sx+=cx; sy+=cy;
} 
c2+=c2d;
sx+=c2x; sy+=c2y;
}
c3+=c3d;
cal4();
cout<<c3<<"\n";	 
sx=c3x; sy=c3y;
}												 
 kx=-xmin+mx; ky = - ymin +my;
}	 

void cal(){
s="";	  
kx=0; ky=0; 
	 
r=g=b=r1=g1=b1=r2=g2=b2=0;
xa1=xa2=xa3=xa4=xb1=xb2=xb3=xb4=xc1=xc2=xc3=xc4=0;
ya1=ya2=ya3=ya4=yb1=yb2=yb3=yb4=yc1=yc2=yc3=yc4=0;

double coord[8][3];
double camx, camy, camz; camx =camy= 0; camz=3;
coord[0][0]=-1; coord[0][1]=1; coord[0][2]=1;
coord[1][0]=1; coord[1][1]=1; coord[1][2]=1;
coord[2][0]=1; coord[2][1]=-1; coord[2][2]=1;
coord[3][0]=-1; coord[3][1]=-1; coord[3][2]=1;
coord[4][0]=-1; coord[4][1]=1; coord[4][2]=-1;
coord[5][0]=1; coord[5][1]=1; coord[5][2]=-1;
coord[6][0]=1; coord[6][1]=-1; coord[6][2]=-1;
coord[7][0]=-1; coord[7][1]=-1; coord[7][2]=-1;

//to radian, clock-wise
double yawr; 
double tx; int ix;

yawr= (360-yaw)/57.3; 
for (int i=0; i<8; i++){
tx = coord[i][0];
coord[i][0]= tx*cos(yawr)-coord[i][1]*sin(yawr);
coord[i][1]= tx*sin(yawr)+coord[i][1]*cos(yawr); }

yawr = (360-roll)/57.3; 
for (int i=0; i<8; i++){
tx = coord[i][0];
coord[i][0]= tx*cos(yawr)-coord[i][2]*sin(yawr);
coord[i][2]= tx*sin(yawr)+coord[i][2]*cos(yawr); }

yawr = (360-pitch)/57.3; 
for (int i=0; i<8; i++){
tx = coord[i][1];
coord[i][1]= tx*cos(yawr)-coord[i][2]*sin(yawr);
coord[i][2]= tx*sin(yawr)+coord[i][2]*cos(yawr); }

double dist[8];
double dclose = 5;
for (int i=0; i<8; i++){
dist[i]=sqrt((camz-coord[i][2])*(camz-coord[i][2])+(camy-coord[i][1])*(camy-coord[i][1]));
dist[i]=sqrt(dist[i]*dist[i]+(camx-coord[i][0])*(camx-coord[i][0]));
if (dist[i]<dclose){dclose= dist[i];}}
dclose+=0.00001; //double badly equals
vector <int> ivector; // (3+8)*11 = 33
  																  																		
if (dist[0]<dclose||dist[1]<dclose||dist[2]<dclose||dist[3]<dclose){ //upper side 
s+="1";
ivector.push_back(0);  ivector.push_back(0); ivector.push_back(253);
tx = coord[0][0]*d+kx; ix=(int)tx; ivector.push_back(ix); 
tx=-coord[0][1]*d+ky; ix = (int)tx; ivector.push_back(ix);
tx = coord[1][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[1][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[2][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[2][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[3][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[3][1]*d+ky; ix=(int)tx; ivector.push_back(ix);  } 

if (dist[2]<dclose||dist[3]<dclose||dist[6]<dclose||dist[7]<dclose){ //front, white
s+="2";
ivector.push_back(254);  ivector.push_back(254); ivector.push_back(254);
tx = coord[3][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[3][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[2][0]*d+kx; ix=(int)tx; ivector.push_back(ix); 
tx=-coord[2][1]*d+ky; ix = (int)tx; ivector.push_back(ix);
tx = coord[6][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[6][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[7][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[7][1]*d+ky; ix=(int)tx; ivector.push_back(ix);  }  

if (dist[2]<dclose||dist[1]<dclose||dist[5]<dclose||dist[6]<dclose){ //right, lgray
s+="3";
ivector.push_back(170);  ivector.push_back(170); ivector.push_back(170);
tx = coord[2][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[2][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[1][0]*d+kx; ix=(int)tx; ivector.push_back(ix); 
tx=-coord[1][1]*d+ky; ix = (int)tx; ivector.push_back(ix);
tx = coord[5][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[5][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[6][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[6][1]*d+ky; ix=(int)tx; ivector.push_back(ix);  } 

if (dist[4]<dclose||dist[7]<dclose||dist[3]<dclose||dist[0]<dclose){ //left, mgray
s+="4";
ivector.push_back(85);  ivector.push_back(85); ivector.push_back(85);
tx = coord[4][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[4][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[7][0]*d+kx; ix=(int)tx; ivector.push_back(ix); 
tx=-coord[7][1]*d+ky; ix = (int)tx; ivector.push_back(ix);
tx = coord[3][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[3][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[0][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[0][1]*d+ky; ix=(int)tx; ivector.push_back(ix);  } 

if (dist[4]<dclose||dist[0]<dclose||dist[1]<dclose||dist[5]<dclose){ //back, black
s+="5";
ivector.push_back(0);  ivector.push_back(0); ivector.push_back(0);
tx = coord[4][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[4][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[0][0]*d+kx; ix=(int)tx; ivector.push_back(ix); 
tx=-coord[0][1]*d+ky; ix = (int)tx; ivector.push_back(ix);
tx = coord[1][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[1][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[5][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[5][1]*d+ky; ix=(int)tx; ivector.push_back(ix);  } 

if (dist[7]<dclose||dist[4]<dclose||dist[5]<dclose||dist[6]<dclose){ //bottom, viol
s+="6";
ivector.push_back(163);  ivector.push_back(73); ivector.push_back(164);
tx = coord[7][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[7][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[4][0]*d+kx; ix=(int)tx; ivector.push_back(ix); 
tx=-coord[4][1]*d+ky; ix = (int)tx; ivector.push_back(ix);
tx = coord[5][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[5][1]*d+ky; ix=(int)tx; ivector.push_back(ix);
tx = coord[6][0]*d+kx; ix=(int)tx; ivector.push_back(ix);
tx=-coord[6][1]*d+ky; ix=(int)tx; ivector.push_back(ix);  } 

if (ivector.size()>0){
r=ivector[0]; g = ivector[1] ;b = ivector[2];
xa1=ivector[3]; ya1=ivector[4]; xa2= ivector[5]	; ya2=ivector[6];			  
 xa3=ivector[7]; ya3=ivector[8]; xa4= ivector[9]	; ya4=ivector[10];   }
 if (ivector.size()>15){
r1=ivector[11]; g1 = ivector[12] ;b1 = ivector[13];
xb1=ivector[14]; yb1=ivector[15]; xb2= ivector[16]	; yb2=ivector[17];			  
 xb3=ivector[18]; yb3=ivector[19]; xb4= ivector[20]	; yb4=ivector[21];   }
  if (ivector.size()>25){
r2=ivector[22]; g2 = ivector[23] ;b2 = ivector[24];
xc1=ivector[25]; yc1=ivector[26]; xc2= ivector[27]	; yc2=ivector[28];			  
 xc3=ivector[29]; yc3=ivector[30]; xc4= ivector[31]	; yc4=ivector[32];   }	 
 rpre=b;
 
 cal2();
	 }

void go2(){
	 std::basic_string<TCHAR> text;
if (pitch <-719|| pitch>719) {pitch = 0;} 
if (pitch <0) {pitch = 360+pitch;} 
if (pitch >359) {pitch = pitch-360;} 
std::ostringstream strs;
strs << pitch;
 text = strs.str(); 
 SetWindowText(pitch_edit, text.c_str()); 
 
 std::basic_string<TCHAR> text3;
if (roll <-719|| roll>719) {roll = 0;} 
if (roll <0) {roll = 360+roll;} 
if (roll >359) {roll = roll-360;} 
std::ostringstream strs3;
strs3 << roll;
 text3 = strs3.str(); 
 SetWindowText(roll_edit, text3.c_str()); 
 
  	 std::basic_string<TCHAR> text2;
if (yaw <-719|| yaw>719) {yaw = 0;} 
if (yaw <0) {yaw = 360+yaw;} 
if (yaw >359) {yaw = yaw-360;} 
std::ostringstream strs2;
strs2 << yaw;
 text2 = strs2.str(); 
 SetWindowText(jaw_edit, text2.c_str()); 
 
 SendMessage(track_pitch, TBM_SETPOS, TRUE, pitch);
 SendMessage(track_roll, TBM_SETPOS, TRUE, roll);
 SendMessage(track_yaw, TBM_SETPOS, TRUE, yaw);

cal();
RedrawWindow(hwnd,NULL,NULL,RDW_INVALIDATE );
SetFocus(hwnd);	 
	 }

void go(){
	 std::basic_string<TCHAR> text;
TCHAR buff[1024] = {0};
GetWindowText(pitch_edit, buff, 1024); 
text = buff;  
pitch = atoi(text.c_str());
if (pitch <-719|| pitch>719) {pitch = 0;} 
if (pitch <0) {pitch = 360+pitch;} 
if (pitch >359) {pitch = pitch-360;} 
std::ostringstream strs;
strs << pitch;
 text = strs.str(); 
 SetWindowText(pitch_edit, text.c_str()); 
 
 std::basic_string<TCHAR> text3;
TCHAR buff3[1024] = {0};
GetWindowText(roll_edit, buff3, 1024); 
text3 = buff3;  
roll = atoi(text3.c_str());
if (roll <-719|| roll>719) {roll = 0;} 
if (roll <0) {roll = 360+roll;} 
if (roll >359) {roll = roll-360;} 
std::ostringstream strs3;
strs3 << roll;
 text3 = strs3.str(); 
 SetWindowText(roll_edit, text3.c_str()); 
 
  	 std::basic_string<TCHAR> text2;
TCHAR buff2[1024] = {0};
GetWindowText(jaw_edit, buff2, 1024); 
text2 = buff2;  
yaw = atoi(text2.c_str());
if (yaw <-719|| yaw>719) {yaw = 0;} 
if (yaw <0) {yaw = 360+yaw;} 
if (yaw >359) {yaw = yaw-360;} 
std::ostringstream strs2;
strs2 << yaw;
 text2 = strs2.str(); 
 SetWindowText(jaw_edit, text2.c_str()); 
 
 SendMessage(track_pitch, TBM_SETPOS, TRUE, pitch);
 SendMessage(track_roll, TBM_SETPOS, TRUE, roll);
 SendMessage(track_yaw, TBM_SETPOS, TRUE, yaw);
cal(); 
RedrawWindow(hwnd,NULL,NULL,RDW_INVALIDATE );
SetFocus(hwnd);	 
	 }
 
 
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM LParam) {
  
    switch (msg) {
		   
		   case WM_HSCROLL:
        {   	  //
if((HWND)LParam == track_yaw ) { 
 int  dwPos = SendMessage(track_yaw, TBM_GETPOS, 0, 0);
if (dwPos!=yaw) {yaw = dwPos;   go2();	 }
        break ;
		}
		if((HWND)LParam == track_roll) { 
 int  dwPos = SendMessage(track_roll, TBM_GETPOS, 0, 0);
if (dwPos!=roll) {roll = dwPos;   go2();	 }
        break ;
		}
		if((HWND)LParam == track_pitch ) { 
 int  dwPos = SendMessage(track_pitch, TBM_GETPOS, 0, 0);
if (dwPos!=pitch) {pitch = dwPos;   go2();	 }
        break ;
		}
    }//
		   		   
case WM_KEYDOWN:
	 switch(wParam)	
	 { 
case VK_TAB: yaw++; go2();  break;
case VK_CAPITAL: yaw--; go2();  break;
case 0X4D: pitch++; go2();  break;
case 0XBC: pitch--; go2();  break;
case 0XBE: roll++; go2();  break;
case 0XBF: roll--; go2();  break;
case VK_LEFT: mx+=d; go2();  break;
case VK_RIGHT: mx-=d; go2();  break;
case VK_UP: my+=d; go2();  break;
case VK_DOWN: my-=d; go2();  break;
case VK_SPACE: scrshot();  break; 
					   }    
break;	   
		   
    case WM_COMMAND:
if (wParam == 100) {go();	}
if (wParam == 101) {showinfo();	}
if (wParam == 102) {openf();	}
if (wParam == 103) {d-=d/5; if (d<1){d=1;} optionswrite(); go2();	}
if (wParam == 104) {d+=d/5; if (d/5<1){d+=1;} optionswrite(); go2();	}
if (wParam == 105) {stdvals(); optionswrite();	go2();}
if (wParam == 106) {stepgo();}
        break;

    case WM_PAINT:
       {	   	 		  		  
        PAINTSTRUCT ps;
    HDC hdc; 
        hdc = BeginPaint(hWnd, &ps);
        
HDC memDC = CreateCompatibleDC( hdc );
RECT rcClientRect;
        GetClientRect( hWnd, &rcClientRect );
    HBITMAP bmp = CreateCompatibleBitmap( hdc, 
            heightscr, 
            heightscr );
			 HBITMAP oldBmp = (HBITMAP)SelectObject( memDC, bmp );		        
       
RECT re;
re.top=0;
re.left=0;
re.right=heightscr;
re.bottom=heightscr;
FillRect(memDC, &re, HBRUSH(CreateSolidBrush(RGB(212,208,200)))); 

HBRUSH hOldBrush;
HBRUSH hBrush1 = CreateSolidBrush(RGB(r, g, b));
HBRUSH hBrush2 = CreateSolidBrush(RGB(r1, g1, b1));
HBRUSH hBrush3 = CreateSolidBrush(RGB(r2, g2, b2));
HPEN hPen=CreatePen(PS_NULL, 3, RGB(0, 0, 0)); 
SelectObject(memDC, hPen);

 sx=kx;  sy=ky;
int c2, c; 
c3=c2=c=0;
int xt1,xt2,xt3,xt4,yt1,yt2,yt3,yt4;
xt1=xt2=xt3=xt4=yt1=yt2=yt3=yt4=0;

c3=c3b;
while (c3!=c3e) {
if (rpre==253){//if we draw upper side start
if (c3==1){r=0; g=151; b = 254; hBrush1 = CreateSolidBrush(RGB(r, g, b));}  
if (c3==2){r=42; g=247; b = 245; hBrush1 = CreateSolidBrush(RGB(r, g, b));} 
if (c3==3){r=105; g=185; b = 159; hBrush1 = CreateSolidBrush(RGB(r, g, b));}  
if (c3==4){r=123; g=144; b = 0; hBrush1 = CreateSolidBrush(RGB(r, g, b));} 
if (c3==5){r=84; g=254; b = 0; hBrush1 = CreateSolidBrush(RGB(r, g, b));}   
if (c3==6){r=254; g=245; b = 0; hBrush1 = CreateSolidBrush(RGB(r, g, b));}   
if (c3==7){r=254; g=157; b = 0; hBrush1 = CreateSolidBrush(RGB(r, g, b));}   
if (c3==8){r=247; g=15; b = 15; hBrush1 = CreateSolidBrush(RGB(r, g, b));}   
if (c3>8){r=129; g=0; b = 0; hBrush1 = CreateSolidBrush(RGB(r, g, b));}     			   
				  } //if we draw upper side end	  
c2=c2b;
while (c2!=c2e) {
c=cb;
while (c!=ce) { //ew line draw start
if (map[c3][c2][c]==1){   										    
 hOldBrush = SelectBrush(memDC, hBrush1);
 xt1= xa1+sx; yt1 = ya1+sy;
 xt2=xa2+sx; yt2= ya2+sy;
 xt3=xa3+sx; yt3=ya3+sy;
 xt4=xa4+sx; yt4 = ya4+sy;
POINT vertices[] = { {xt1, yt1},{xt2, yt2}, {xt3, yt3}, {xt4, yt4} };
Polygon(memDC, vertices, sizeof(vertices) / sizeof(vertices[0]));

 hOldBrush = SelectBrush(memDC, hBrush2);
 xt1= xb1+sx; yt1 = yb1+sy;
 xt2=xb2+sx; yt2= yb2+sy;
 xt3=xb3+sx; yt3=yb3+sy;
 xt4=xb4+sx; yt4 = yb4+sy;
POINT vertices2[] = { {xt1, yt1},{xt2, yt2}, {xt3, yt3}, {xt4, yt4} };
Polygon(memDC, vertices2, sizeof(vertices2) / sizeof(vertices2[0]));

 hOldBrush = SelectBrush(memDC, hBrush3);
 xt1= xc1+sx; yt1 = yc1+sy;
 xt2=xc2+sx; yt2= yc2+sy;
 xt3=xc3+sx; yt3=yc3+sy;
 xt4=xc4+sx; yt4 = yc4+sy;
POINT vertices3[] = { {xt1, yt1},{xt2, yt2}, {xt3, yt3}, {xt4, yt4} };
Polygon(memDC, vertices3, sizeof(vertices3) / sizeof(vertices3[0]));
}
c+=cd;
sx+=cx; sy+=cy;
} //ew line draw end
c2+=c2d;
sx+=c2x; sy+=c2y;
}
c3+=c3d;
cal4();
sx=c3x; sy=c3y;
}

BitBlt( hdc, 0, 0, heightscr, heightscr, memDC, 0, 0, SRCCOPY );

SelectObject( memDC, oldBmp );
        DeleteObject( bmp ); 
        DeleteDC( memDC );
DeleteObject(hPen); 
DeleteObject(hBrush1);
DeleteObject(hBrush2);
DeleteObject(hBrush3);
DeleteObject(hOldBrush);		            
ReleaseDC(hWnd, hdc);  
        EndPaint(hWnd, &ps);
        
        }
        break;
 
    case WM_DESTROY:
		 optionswrite();
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, LParam);
}
 
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE h2, LPSTR cmd, int cmShow) {

ifstream in("map_rotate.txt", ios::in | ios::binary);
if(!in) {
optionswrite();
}
in.close();	

	fillmap();
	stdfill();
	optionsread();
    openf2();
	
    WNDCLASS wc = {0};
    wc.lpszClassName = TEXT("Myclass");
    wc.lpfnWndProc = WndProc;
    wc.hbrBackground = CreateSolidBrush(RGB(212, 208, 200));
    wc.hCursor=LoadCursor(NULL,IDC_ARROW); 
    RegisterClass(&wc);
 
 hwnd = CreateWindow(wc.lpszClassName,"Map_rotate_by_OlegTim",
 WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0,0,widthscr,heightscr,0,0,hInstance,0);

SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), //fullscreen w tab
GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED); 

WINDOWPLACEMENT wpc; //fullscreen without tab and borders
GetWindowPlacement(hwnd,&wpc);
                SetWindowLong(hwnd,GWL_STYLE,WS_POPUP);
                SetWindowLong(hwnd,GWL_EXSTYLE,WS_EX_TOPMOST);
                ShowWindow(hwnd,SW_SHOWMAXIMIZED);


RECT rect; 
if(GetWindowRect(hwnd, &rect))
{
   widthscr = rect.right - rect.left;
   heightscr = rect.bottom - rect.top;
}

int x = heightscr+5;
int y = 5;

HWND hwndButton = CreateWindow("BUTTON", "show", 
WS_CHILD | WS_VISIBLE, x, y, dm*2, dm, hwnd, (HMENU)100, hInstance, 0);
y+=2*dm;

CreateWindow(TEXT("STATIC"),TEXT("Pitch: "),  WS_VISIBLE | WS_CHILD,
  x,y,2*dm,dm-5,     hwnd,(HMENU)4,NULL,NULL  );
  x+=3*dm; 
 std::basic_string<TCHAR> text;
std::ostringstream strs;
strs << pitch;
text = strs.str();  
 pitch_edit=CreateWindow(TEXT("edit"),TEXT(text.c_str()), 
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        x,y,dm*2,dm,hwnd,(HMENU)5,NULL,NULL );
y+=dm*2;  x = heightscr+5;
track_pitch = CreateWindowEx(0, TRACKBAR_CLASS, NULL,
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,  x, y, 8*dm, dm,
    hwnd, NULL, hInstance, NULL);    
SendMessage(track_pitch, TBM_SETRANGE, (WPARAM) TRUE,  (LPARAM) MAKELONG(0, 359));
y+=dm*2;  x = heightscr+5;

CreateWindow(TEXT("STATIC"),TEXT("Roll: "),  WS_VISIBLE | WS_CHILD,
  x,y,2*dm,dm-5,     hwnd,(HMENU)6,NULL,NULL  );
  x+=3*dm; 
 std::basic_string<TCHAR> text3;
std::ostringstream strs3;
strs3 << roll;
text3 = strs3.str();  
 roll_edit=CreateWindow(TEXT("edit"),TEXT(text3.c_str()), 
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        x,y,dm*2,dm,hwnd,(HMENU)7,NULL,NULL );
y+=dm*2;  x = heightscr+5;  
track_roll = CreateWindowEx(0, TRACKBAR_CLASS, NULL,
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,  x, y, 8*dm, dm,
    hwnd, NULL, hInstance, NULL);    
SendMessage(track_roll, TBM_SETRANGE, (WPARAM) TRUE,  (LPARAM) MAKELONG(0, 359));
y+=dm*2;  x = heightscr+5;

CreateWindow(TEXT("STATIC"),TEXT("Yaw: "),  WS_VISIBLE | WS_CHILD,
  x,y,2*dm,dm-5,     hwnd,(HMENU)8,NULL,NULL  );
  x+=3*dm; 
 std::basic_string<TCHAR> text2;
std::ostringstream strs2;
strs2 << yaw;
text2 = strs2.str();  
 jaw_edit=CreateWindow(TEXT("edit"),TEXT(text2.c_str()), 
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        x,y,dm*2,dm,hwnd,(HMENU)9,NULL,NULL );
y+=dm*2;  x = heightscr+5;    

// InitCommonControls();  // do, if Comctl32.dll version < 6.0
track_yaw = CreateWindowEx(0, TRACKBAR_CLASS, NULL,
    WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,  x, y, 8*dm, dm,
    hwnd, NULL, hInstance, NULL);    
SendMessage(track_yaw, TBM_SETRANGE, (WPARAM) TRUE,  (LPARAM) MAKELONG(0, 359));

y+=dm*3;  x = heightscr+5;  
HWND hwndButton2 = CreateWindow("BUTTON", "info", 
WS_CHILD | WS_VISIBLE, x, y, dm*2, dm, hwnd, (HMENU)101, hInstance, 0);
x+=3*dm;
HWND hwndButton3 = CreateWindow("BUTTON", "open", 
WS_CHILD | WS_VISIBLE, x, y, dm*2, dm, hwnd, (HMENU)102, hInstance, 0);
x = heightscr+5; y+=2*dm;
HWND hwndButton4 = CreateWindow("BUTTON", "-", 
WS_CHILD | WS_VISIBLE, x, y, dm, dm, hwnd, (HMENU)103, hInstance, 0);
x+=2*dm;
HWND hwndButton5 = CreateWindow("BUTTON", "+", 
WS_CHILD | WS_VISIBLE, x, y, dm, dm, hwnd, (HMENU)104, hInstance, 0);
x+=2*dm;
HWND hwndButton6 = CreateWindow("BUTTON", "std", 
WS_CHILD | WS_VISIBLE, x, y, dm*2, dm, hwnd, (HMENU)105, hInstance, 0);
y+=3*dm; x = heightscr+5; 
HWND hwndButton7 = CreateWindow("BUTTON", "step", 
WS_CHILD | WS_VISIBLE, x, y, dm*2, dm, hwnd, (HMENU)106, hInstance, 0);
  
go2();//to show on first run                
  MSG msg;
    while (GetMessage(&msg, NULL , 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
           if(msg.message==WM_KEYDOWN)
             {
                   if(msg.wParam==VK_ESCAPE)
                   {
				   		optionswrite();					
                        PostQuitMessage(0); 
                   }
				   if(msg.wParam==VK_RETURN)
                                       {
									   		go();  
									   		}
				   }
    }
    return 0;
}
