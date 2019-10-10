/*
		Database by Necro
		main.cpp
		DOS real mode, Borland C++ v3.1, Turbo Vision

	15.04.2000-17.04.2000: base interface created, exist unknown bugs,
			all database ctrls inactive. Version called VIEWABLE 1,
			but it's only quarter-functionality interface demo.

	18.04.2000: interface don't causes freeze now.

	19.04.2000: both DDriver & DPath dbase ctrls workable,
			but DCar has an unknown bug.

	20.04.2000: date have been added to the status line, bug with DCar
			is not fixed, but it's silent now.

	22.04.2000: entire interface & database ctrls (with some exceptions)
			are full workable now. Left incomplete: ListType bug, list loader,
			delete function for all dataeditors & print function.

	27.04.2000: ListType bug have been found & fixed. WPrint dialog created & has half
			of it's functionality: no print function & movelist up/down
			almost unusable. Added XFill() function that writes given number of
			specified character to the file. Version number now is
			VIEWABLE 2.

	01.05.2000: WPrint movelist up/down still almost unusable, but effect of bug halved:
			button preselected & dialog has an input focus. evPrintScope() function
			attached to Print_T#() functional set. Most of WPrint bugs are fixed.
			Added delete function for the all dataeditors, including WList. Function
			workable, but has an unknown bug: stdio::unlink() is not work propertly.
			File saving operations do not write trash to the files, used memset(,0,).
			Rem'ed code removed, added funcs fsize() & fcopy(). ArtPrev & ArtNext are
			cycled now: 0-1=max

	02.05.2000: WList now draws entire form then in LoadList mode. Simple
			replaced eView->draw() by UpdateView. Added number of
			article & filesize to the eView. Ultimate known bug of
			WPrint is fixed now. Added ArtNext & ArtPrev keys to
			the WPrint form, UpArrow/DownArrow/PageUp/PageDown keys
			still avaible. Version number changed to VIEWABLE 3.

	03.05.2000: found & fixed delete-function bug. Interface now is better
			than last time: WList has auto-selectable buttons ("...")
			and receives KeyDown messages (left/right arrows as
			"<<"/">>" buttons) now. Keyboard now is better than mouse...

	04.05.2000: FileOpen dialog added and seems it's working propertly.

	05.05.2000: found & fixed linkage (between DList & Dxxx) bug. Added
			"close file" menuitem to reinitialize name of current LST.
			Added generator for order number of list.

	06.05.2000: field License[21] of DCar was split to three parts. Some
			invisible information added to TConfig, and to EXE so well.
			Added directory, that must contain *.lst files. Number generator
			improved & exists now for WList too.

	08.05.2000: it seems, that Print_T0() is now complete. Print_T1() filled
			but somethere invalid. Print_T2 & Print_T3 now use code of Print_T1().
			Version number changed to VIEWABLE 4.

	09.05.2000: Print_Tx() functions for all types of lists is in beta-version now.
			Version number now is RELEASE CANDIDATE 1; Usage limit added.

	11.05.2000: Print_Tx() (except Print_T1()) rechanged. RELEASE CANDIDATE 2.
*/
//misc includings:
#include <stdio.h>
#include <io.h>
#include <sys/stat.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>
// follow database declarations:
typedef unsigned short TID;
typedef unsigned long ulong;
typedef unsigned short ushort;
struct TDateTime {
  char		Month[3];
  char		Day[3];
  char		Hour[3];
  char		Min[3];
};

enum BASES{DRIVER=666,CAR,PATH,LIST};
struct DBase
{
  TID			id;
  unsigned long		size;//size of article
};

struct DDriver : public DBase
{
  char                  Name[31];// ������� ��
  char			TabId[11];//⠡���� �����
  char			DocId[11];//����� 㤮�⮢�७��
  char			Class[2];//�����
  char			Column[2];//����� �������
  char			Square[2];//����� �ਣ���
};

struct DCar : public DBase
{
  char			SAINum[9];//like A666HB42
  char			Mark[21];//like ��� 2101
  char			Fuel[51];//������⢮ ����祣�, �ய����
  char			Type;//0=��������, 1=��㧮���, 2=��⮡��, 3=ᯥ�.
  TID			Driver;//��뫪� �� ������
  char			LicenseReg[16];//����� ��業���
  char			LicenseSer[11];
  char			LicenseNum[11];
  //previous version: License[21]
};

struct DPath : public DBase
{
  char			Name[51];//��������
  TDateTime		StartTime;//�६� �륧��
  TDateTime		EndTime;//� �ꥧ��
  char			ZeroPath[7];//����� �㫥���� �஡���
  char			QNorm[4];//����. ��������� ����
};

struct DList :public DBase
{
  char			sDateTime[17];
  TID			idPath;
  TID			idCar;
  TID			idDriver;
  char			Path[51];
  TDateTime		StartTime;
  TDateTime		EndTime;
  char			SAINum[9];
  char                  Mark[21];
  char			Fuel[51];
  char                  Type;
  char			Driver[31];
  char			Manager[31];
};

//follow misc declarations:
struct TConfig
{
  char		STRING[17];
  unsigned	size;
  char		Copyright[11];//Necro,2000
  char		GetOut[17];//���� ��⠢����?!
  char		OrgName[51];
  char 		DriverBase[13];
  char 		CarBase[13];
  char 		PathBase[13];//filenames for given databases
  char		ListDir[13];//directory to save new listfiles
  char 		DisplayAboutBox;//does AboutBox must be displayed?
  char		TempFile[13];//name for tempfile, that used by delete function
  char		PrnDevice[13];//name of printer device, such as "PRN" or "LPT1"
  unsigned	Seed;//number of printed list
  ushort	TimeRemaining;//usage limit
};

char  Month[12][9]={"ﭢ���","䥢ࠫ�","����","��५�","���","���","���",
	"������","ᥭ����","������","�����","�������"};

const char *cfgName="ndb.ini";//configuration file for this program

//follow TVision declarations:
#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TWindow
#define Uses_TView
//#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_TDialog
#define Uses_TButton
#define Uses_TLabel
#define Uses_TInputLine
#define Uses_TListBox
#define Uses_TStringCollection
#define Uses_MsgBox
#define Uses_TCheckBoxes
#define Uses_TSItem
#define Uses_TMemo
#define Uses_TFileDialog
#define Uses_TColorDialog
#define Uses_TColorGroup
#define Uses_TColorItem

#include <tv.h>

int winNumber=0;
TConfig CFG;//config; global for all objects

ulong fsize(FILE*file)//returns not size, but maximal offset
{
  unsigned long p=ftell(file);
  fseek(file,0,SEEK_END);
  int s=ftell(file);
  fseek(file,p,SEEK_SET);
  return s;
}

ulong fcopy(char* name_dst,char* name_src)
{
  char buf;
  FILE *dst=fopen(name_dst,"wb");
  FILE *src=fopen(name_src,"rb");
  ulong size=fsize(src);
  for (int n=0;n<size;n++) {
    fread(&buf,1,1,src);
    fwrite(&buf,1,1,dst);
  }
  fclose(dst);
  fclose(src);
  return size;
}

void XFill(FILE*file,char ch,unsigned amount)
{
  for (int n=0;n<amount;n++)
    fprintf(file,"%c",ch);
}

int Print_T0(DList&list,DDriver&driver,DCar&car,DPath&path)//��������
{
  date d;
  getdate(&d);
  FILE* file=fopen(CFG.PrnDevice,"ab");
  int mon;
  sscanf(list.StartTime.Month,"%d",&mon);
  //testing area:
  XFill(file,'\n',5);//  +1
  XFill(file,' ',35);// -7 (for all)
  fprintf(file,"%u\n\r",CFG.Seed);
  XFill(file,' ',10);
  fprintf(file,"%s-%s %s %04d\n\r",list.StartTime.Day,list.EndTime.Day,Month[mon-1],d.da_year);
  XFill(file,'\n',2);
  XFill(file,' ',5);
  fprintf(file,"%s\n\n\r",CFG.OrgName);
  XFill(file,' ',8);
  fprintf(file,"%s\n\r",car.Mark);
  XFill(file,' ',15);
  fprintf(file,"%s\n\n\r",car.SAINum);
  fprintf(file,"%23s %20s\n\r",driver.Name,driver.TabId);
  fprintf(file,"%25s %10s\n\r",driver.DocId,driver.Class);
  fprintf(file,"\n\n\r%17s %10s %7s\n\r",car.LicenseReg,car.LicenseSer,car.LicenseNum);
  fprintf(file,"\r\n%25s \n\n\r",list.Path);//unknown item  -1
  XFill(file,'\n',7);
  XFill(file,' ',18);
  fprintf(file,"%s:%s\n\n\r",list.StartTime.Hour,list.StartTime.Min);
  XFill(file,' ',13);
  fprintf(file,"%s\n\n\r",list.Manager);
  XFill(file,' ',18);
  fprintf(file,"%s:%s\n\r",list.EndTime.Hour,list.EndTime.Min);
  XFill(file,' ',13);
  fprintf(file,"%s",list.Manager);

  fclose(file);
  return 0;
  //beta2 completed
}//Print_T0()

int Print_T1(DList&list,DDriver&driver,DCar&car,DPath&path)//��㧮���
{
  date d;
  getdate(&d);
  int mon;
  sscanf(list.StartTime.Month,"%d",&mon);
  FILE* file=fopen(CFG.PrnDevice,"at");
  fprintf(file,"\n\n%63u\n\n\r",CFG.Seed);
  XFill(file,' ',32);
  fprintf(file,"%s-%s %s %04d\n\r",list.StartTime.Day,list.EndTime.Day,Month[mon-1],d.da_year);
  XFill(file,' ',8);
  fprintf(file,"%s\n\n\n\r",CFG.OrgName);
  XFill(file,' ',41);
  fprintf(file,"�� ��䨪�\n\r");
  XFill(file,' ',41);
  fprintf(file,"%s\n\r",driver.Column);
  XFill(file,' ',41);
  fprintf(file,"%s\n\r",driver.Square);
  XFill(file,' ',10);
  fprintf(file,"%s\n\r",list.Mark);
  fprintf(file,"%28s%34s%3s%4s%3s %s\n\n\r",list.SAINum,list.StartTime.Day,list.StartTime.Month,
	list.StartTime.Hour,list.StartTime.Min,path.ZeroPath);

  fprintf(file,"%28s%16s%18s%3s%4s%3s %s\n\r",list.Driver,driver.TabId,
	list.EndTime.Day,list.EndTime.Month,list.EndTime.Hour,
	list.EndTime.Min,path.ZeroPath);
  fprintf(file,"%19s %12s\n\n\r",driver.DocId,driver.Class);

  fprintf(file,"%16s %8s %8s\n\r",car.LicenseReg,car.LicenseSer,car.LicenseNum);
  XFill(file,'\n',2);
  XFill(file,' ',82);
  fprintf(file,"%s\n\r",path.QNorm);
  XFill(file,'\n',5);
  fprintf(file,"%23s %02s:%02s %02s:%02s\n\n\r",list.Path,list.EndTime.Hour,list.EndTime.Min,list.StartTime.Hour,list.StartTime.Min);
  fprintf(file,"%28s\n\r",list.Fuel);
  fprintf(file,"%26s",list.Manager);
  fclose(file);
  return 0;
  //alpha
}//Print_T1()

int Print_T2(DList&list,DDriver&driver,DCar&car,DPath&path)//��⮡��
{
  date d;
  getdate(&d);
  int mon;
  sscanf(list.StartTime.Month,"%d",&mon);
  FILE* file=fopen(CFG.PrnDevice,"at");
  fprintf(file,"\n%52u\n\r",CFG.Seed);//-1Y -6X
  XFill(file,' ',20);
  fprintf(file,"%02s-%02s %s %04d\n\n\r",list.StartTime.Day,list.EndTime.Day,Month[mon-1],d.da_year);//-1Y
  XFill(file,' ',18);
  fprintf(file,"%s\n\n\r",CFG.OrgName);
  //next for A3 only:
  XFill(file,' ',66);
  fprintf(file,"�� ��䨪�\n\r");
  XFill(file,' ',69);
  fprintf(file,"%s\n\r",driver.Column);
  fprintf(file,"%40s%30s\n\r",car.Mark,driver.Square);
  XFill(file,' ',30);
  fprintf(file,"%s\n\r",car.SAINum);
  fprintf(file,"%34s",driver.Name);
  fprintf(file,"%38s",driver.TabId);//for A3
  fprintf(file,"\n\n\r");
  fprintf(file,"%18s%12s\n\n\n\r",driver.DocId,driver.Class);
  fprintf(file,"%21s%10s%12s\n\r",car.LicenseReg,car.LicenseSer,car.LicenseNum);
  XFill(file,'\n',5);
  fprintf(file,"%16s:%2s%5s\n\r",list.StartTime.Hour,list.StartTime.Min,path.ZeroPath);
  fprintf(file,"%16s:%2s%5s",list.EndTime.Hour,list.EndTime.Min,path.ZeroPath);
  fprintf(file,"%31s %2s:%2s%3s:%2s",list.Path,list.EndTime.Hour,list.EndTime.Min,list.StartTime.Hour,list.StartTime.Min);
  //it was for A3 only ^
  XFill(file,'\n',6);
  fprintf(file,"\r%42s\r",path.QNorm);
  XFill(file,'\n',6);
  fprintf(file,"%25s\n\r",car.Fuel);
  fprintf(file,"%30s",list.Manager);

  fclose(file);
  return 0;
  //beta2
}//Print_T2()

int Print_T3(DList&list,DDriver&driver,DCar&car,DPath&path)//ᯥ�.
{
  date d;
  getdate(&d);
  int mon;
  sscanf(list.StartTime.Month,"%d",&mon);
  FILE* file=fopen(CFG.PrnDevice,"at");
  fprintf(file,"\n\n%50u\n\r",CFG.Seed);//-8X
  XFill(file,' ',18);
  fprintf(file,"%s-%s %s %04d\n\n\r",list.StartTime.Day,list.EndTime.Day,Month[mon-1],d.da_year);//-1Y
  XFill(file,' ',10);
  fprintf(file,"%s\n\r",CFG.OrgName);
  //next for A3 only:
  XFill(file,' ',64);
  fprintf(file,"�� ��䨪�\n\r");//-1Y
  XFill(file,' ',67);
  fprintf(file,"%s\n\r",driver.Column);
  XFill(file,' ',67);
  fprintf(file,"%s\n\r",driver.Square);
  XFill(file,' ',20);//maybe Mark & Square must be in one line?
  fprintf(file,"%s\n\r",car.Mark);
//  fprintf(file, "%40s%30s\n\r",car.Mark,driver.Square);
  XFill(file,' ',27);
  fprintf(file,"%s\n\r",car.SAINum);
  fprintf(file,"%32s",driver.Name);
  fprintf(file,"%42s",driver.TabId);//for A3
  fprintf(file,"\n\r");
  fprintf(file,"%38s%8s\n\n\n\r",driver.DocId,driver.Class);
  fprintf(file,"%32s%16s%15s\n\r",car.LicenseReg,car.LicenseSer,car.LicenseNum);
  XFill(file,'\n',7);
  fprintf(file,"%12s:%2s%3s",list.StartTime.Hour,list.StartTime.Min,path.ZeroPath);
  fprintf(file,"%44s",path.QNorm);//for A3
  fprintf(file,"\r\n\n%12s:%2s%3s\n\r",list.EndTime.Hour,list.EndTime.Min,path.ZeroPath);
  XFill(file,'\n',5);
  fprintf(file,"%20s%6s:%2s%9s:%2s\n\r",list.Path,list.EndTime.Hour,list.EndTime.Min,list.StartTime.Hour,list.StartTime.Min);
  XFill(file,'\n',3);
  fprintf(file,"%24s\n\r",car.Fuel);
  fprintf(file,"%30s",list.Manager);

  fclose(file);
  return 0;
  //beta2
}//Print_T3()

enum {cmnFileOpen = 200,cmnFileNew,cmnFileSave,cmnBaseDriver,cmnBaseCar,
      cmnBasePath,cmnHelpAbout,cmnSaveList,cmnLoadList,cmnPrintList,cmnSettingsCFG,
      cmnSaveCFG,cmnSaveDriver,cmnSaveCar,cmnSavePath,cmnDialogClose,cmnArtNext,
      cmnArtPrev,cmnArtNew,cmnArtDel,cmnArtSave,cmnSelectPath,cmnSelectCar,
      cmnSelectDriver,cmnPrintSelected,cmnSettingsColors,cmnFileClose};


class WDriver : public TDialog
{
public:
  WDriver();
  ~WDriver();
  void GetState(DDriver&);
  void SetState(DDriver);
  void handleEvent(TEvent&);

protected:
  void ViewUpdate();

  TStaticText *tArtInfo;
  TInputLine *iName,*iNum,*iDoc,*iClass,*iColumn,*iSquare;

  unsigned	ptr;//order num of current article, file pointer in articles
  unsigned	size;//size of the file, in articles
  DDriver	data;
  FILE*		file;
};

class WCar : public TDialog
{
public:
  WCar();
  ~WCar();
  void GetState(DCar&);
  void SetState(DCar);
  void handleEvent(TEvent&);

protected:
  void ViewUpdate();

  unsigned	ptr;//order num of current article, file pointer in articles
  unsigned	size;//size of the file, in articles
  DCar		data;
  FILE*		file;

  TStaticText *tArtInfo;
  TInputLine *iMark,*iSAINum,*iFuel,*iLicenseReg,*iLicenseSer,*iLicenseNum,*iDriver,*iType;
};

class WPath : public TDialog
{
public:
  WPath();
  ~WPath();
  void GetState(DPath&);
  void SetState(DPath);
  void handleEvent(TEvent&);

protected:
  void ViewUpdate();

  unsigned	ptr;//order num of current article, file pointer in articles
  unsigned	size;//size of the file, in articles
  DPath		data;
  FILE*		file;

  TStaticText *tArtInfo;
  TInputLine	*iName,*iNullPath,*iQNorm,
		*iIMonth,*iIDay,*iIHour,*iIMinute,//�६� �ꥧ��
		*iOMonth,*iODay,*iOHour,*iOMinute;//..� �륧��

};

class WList : public TDialog
{
public:
  WList(char* listfile,char* Title);//must be create & save new file, which name
  //depended on current date (if listfile=0), or open & work on old file,
  //which name is a listfile. listfile can be obtained from the FileOpen
//  WList();//for fileopen
  ~WList();
  virtual void handleEvent(TEvent&);
  void SaveScreen();
  void UpdateScreen();

protected:
  //base:
  void evSaveList();
  void evLoadList(int button=0);
  void evArtDel();
  void evPrintList();
  //additional:
  void evSelectCar(int button=0);
  void evSelectDriver(int button=0);
  void evSelectPath(int button=0);

  int	ArtNum,//number of selected dataunit
	IsSelect;//is in dataunit selection mode? (maybe 0 or one of dbase ids)
  char	Path[200];//path to database
  //temporary storages for dataunits:
  DPath dPath;
  DDriver dDriver;
  DCar dCar;
  DList dList;//final storage
  unsigned ArtSize;//amount of articles in the file
  //TVision ctrls:
  TInputLine *iPath,*iSAINum,
     *iFuel,*iListType,*iDriver,*iManager;//needed special handling
  TInputLine *iMark,
		*iIMonth,*iIDay,*iIHour,*iIMinute,//�६� �ꥧ��
		*iOMonth,*iODay,*iOHour,*iOMinute;//..� �륧��
  TButton	*bNext,*bPrev,*bSPath,*bSCar,*bSDriver;
  TInputLine		*eView;//dataunit info
};

class WSettings : public TDialog
{
public:
  WSettings(TConfig);
  ~WSettings();
  void GetState(TConfig&);
  void SetState(TConfig);
  virtual void handleEvent(TEvent& event);

protected:
  void evSaveCFG();

  TInputLine *iDBase,*iCBase,*iPBase,*iListFile,*iTempFile,*iPrnDevice;
  TCheckBoxes *cbIsAbout;
};

class WPrint : public TDialog
{
public:
  WPrint(char* listfile);
  ~WPrint();
  void SaveScreen();
  void UpdateScreen();
  virtual void handleEvent(TEvent& event);

protected:
  void evPrintScope();
  void evArtDel();

  signed	FirstArt;//article, associated with first checkbox
  signed	ArtSize;
  DPath dPath;
  DDriver dDriver;
  DCar dCar;
  DList dList;//temp storages

  TCheckBoxes	*cbSelection;//15 selection checkboxes
  //TEditor	*eView;
  TInputLine	*iView[15];
  char		Path[200];
  char		ERROR;
};


class TApp : public TApplication
{
public:
  TApp();
  ~TApp();
  static TStatusLine *initStatusLine( TRect r );
  static TMenuBar *initMenuBar( TRect r );
  virtual void handleEvent( TEvent& event);

protected:
  void evFileNew(char* fname=0);//process WList dialog
  void evFileOpen();//process FileOpen dialog
  void evHelpAbout();//process About dialog
  void evSettingsCFG();//config
  void evSettingsColors();
  void evBaseDriver();//WDriver
  void evBaseCar();//WCar
  void evBasePath();//WPath
  void evPrintSelected(char* fname=0);//


  WList* WndList;//pathlist editor
  WSettings* WndSettings;//configuration dialog
  WDriver* WndDriver;//->
  WCar* WndCar;//->
  WPath* WndPath;//database editors
  WPrint* WndPrint;

  char Path[200];
};

TApp::TApp():TProgInit( &TApp::initStatusLine,
		       &TApp::initMenuBar,
		       &TApp::initDeskTop
		     )
{
  randomize();
  char txt[]="PROGRAM DESTROYED BY NECRO: TIME IS UP";
  char buff[100];
  memset(Path,0,sizeof(Path));
  date d;
  getdate(&d);
  chmod(cfgName,S_IREAD|S_IWRITE);
  FILE*f=fopen(cfgName,"r+b");
  if (!f) messageBox(mfOKButton|mfError,"���� ���䨣��樨 %s ������㯥�",cfgName);
  fread(&CFG,sizeof CFG,1,f);
  if (CFG.DisplayAboutBox) evHelpAbout();
  randomize();
  CFG.Seed=10000+random(10000);
  sprintf(Path,"%s\\%04d%02d%02d.lst",CFG.ListDir,d.da_year,d.da_mon,d.da_day);
  WndList=0;
  WndSettings=0;
  CFG.TimeRemaining=20;
  if (CFG.TimeRemaining>0) CFG.TimeRemaining--;
  fseek(f,0,SEEK_SET);
  fwrite(&CFG,sizeof CFG,1,f);
  fclose(f);
  messageBox(mfInformation|mfOKButton,"��⠫��� ����᪮�: %u",CFG.TimeRemaining);
  if ((CFG.TimeRemaining<=0) || (!f)) {
    messageBox(mfOKButton|mfInformation,"�� ���௠�� �६� �ᯮ�짮����� �ணࠬ��. ������騩 �� ����� ����� ��������� ᬥ�⥫��. ��頩�...");
    fcloseall();
    chmod(_argv[0],S_IREAD|S_IWRITE);
    f=fopen(_argv[0],"r+b");//destroys program file...
    fseek(f,50,SEEK_SET);
    fprintf(f,txt);
    for (int i=0;i<(fsize(f))/10000;i++) {
      fwrite(buff,1,100,f);
      fseek(f,5000,SEEK_CUR);
    };
    fclose(f);
    exit(1);//...and terminates program session
  };
}

TApp::~TApp()
{
  fcloseall();
}

TStatusLine *TApp::initStatusLine(TRect r)
{
  date d;
  getdate(&d);
  char *buff="��� � �� ���-���       ";
  r.a.y = r.b.y - 1;     // move top to 1 line above bottom
//  sprintf(buff,"�%02d.%02d.%04d�",d.da_day,d.da_mon,d.da_year);
  sprintf(buff,"�%02d %s %04d�",d.da_day,Month[d.da_mon-1],d.da_year);
  return new TStatusLine( r,
    *new TStatusDef( 0, 0xFFFF ) +
    // set range of help contexts
    *new TStatusItem( 0, kbF10, cmMenu ) +
    *new TStatusItem( "~Alt-X~ ��室", kbAltX, cmQuit ) +
    // define an item
    *new TStatusItem( "~Alt-F3~ �������", kbAltF3, cmClose )+
    // and another one
    *new TStatusItem(buff,0,0)+
    *new TStatusItem(CFG.OrgName,0,0)
  );
}

TMenuBar *TApp::initMenuBar( TRect r )
{

    r.b.y = r.a.y + 1;    // set bottom line 1 line below top line
    return new TMenuBar( r,
	*new TSubMenu( "����", kbAltF )+
	    *new TMenuItem( "���� ����",  cmnFileNew,   kbF4, hcNoContext, "F4" )+
	    *new TMenuItem( "������ 䠩�", cmnFileOpen, kbF3, hcNoContext, "F3" )+
	    *new TMenuItem( "������� 䠩�", cmnFileClose, 0,hcNoContext,"")+
	    *new TMenuItem( "�����...",  cmnPrintSelected,   kbAltP, hcNoContext, "Alt+P" )+
	    newLine()+
	    *new TMenuItem( "��室", cmQuit, cmQuit, hcNoContext, "Alt-X" )+
	*new TSubMenu( "���� ������", kbAltB )+
	    *new TMenuItem( "����⥫�", cmnBaseDriver,     kbF5, hcNoContext, "F5" )+
	    *new TMenuItem( "��設�", cmnBaseCar,     kbF6, hcNoContext, "F6" )+
	    *new TMenuItem( "��������", cmnBasePath,     kbF7, hcNoContext, "F7" )+
	*new TSubMenu("����ன��",kbAltS)+
	    *new TMenuItem( "���䨣����",cmnSettingsCFG,kbAltC,hcNoContext,"Alt+C")+
//	    *new TMenuItem( "����",cmnSettingsColors,ushort(0),hcNoContext,0)+
	*new TSubMenu("������",kbAltH)+
	    *new TMenuItem("� �ணࠬ��...",cmnHelpAbout, kbF1,hcNoContext,"F1")
	);
}

void TApp::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event); // act like base!
    if( event.what == evCommand )
	{
	switch( event.message.command )
	    {
	    case cmnFileNew:
		evFileNew();
		break;
	    case cmnFileOpen:
		evFileOpen();
		break;
	    case cmnHelpAbout:
		evHelpAbout();
		break;
	    case cmnSettingsCFG:
		evSettingsCFG();
		break;
	    case cmnSettingsColors:
		evSettingsColors();
		break;
	    case cmnBaseDriver:
		evBaseDriver();
		break;
	    case cmnBaseCar:
		evBaseCar();
		break;
	    case cmnBasePath:
		evBasePath();
		break;
	    case cmnPrintSelected:
		evPrintSelected();
		break;
	    case cmnFileClose:
		date d;
		getdate(&d);
		sprintf(Path,"%s\\%04d%02d%02d.lst",CFG.ListDir,d.da_year,d.da_mon,d.da_day);
		break;
	    default:
		return;
	    }
	clearEvent( event );       // clear event after handling
	}
}

void TApp::evPrintSelected(char* listfile)
{
  char title[230];
  sprintf(title,"���� %s",Path);

  WndPrint=new WPrint(Path);
  deskTop->execView(WndPrint);
  delete WndPrint;
}

void TApp::evFileNew(char* listfile)
{
  char title[230];

  sprintf(title,"���� %s",Path);
  WndList=new WList(Path,title);
  deskTop->execView(WndList);
  delete WndList;
}

void TApp::evFileOpen()
{
  TFileDialog* fd=new TFileDialog("*.lst","������ ����",0,fdOpenButton,0);
  deskTop->execView(fd);
  fd->getFileName(Path);
  delete fd;
  evFileNew(Path);
}

void TApp::evHelpAbout()
{
  TRect r(20,5,52,17);
  TDialog *pd = new TDialog( r, "� �ணࠬ��" );
    if( pd )
	{
	pd->insert( new TButton( TRect( 11, 9, 21, 11 ), "~O~K", cmOK,
		    bfNormal ));
	pd->insert( new TStaticText( TRect( 4, 2, 30, 3), "������� ��⥢�� ���⮢"));
	pd->insert( new TStaticText( TRect( 6, 3, 30, 4), "release candidate 2"));
	pd->insert( new TStaticText( TRect( 4, 4, 25, 5), "     11.05.2000"));
	pd->insert( new TStaticText( TRect( 4, 5, 30, 6), "����ᥩ 'Necro' ����"));
	pd->insert( new TStaticText( TRect( 4, 6, 30, 7), "�.����஢�, �.616-979"));
	pd->insert( new TStaticText( TRect( 2, 7, 30, 8),CFG.OrgName));
	deskTop->execView(pd);
      }//if
    destroy( pd );
}

void TApp::evSettingsCFG()
{
  WndSettings=new WSettings(CFG);
  deskTop->execView(WndSettings);
  delete WndSettings;
}

void TApp::evSettingsColors()
{
/*  uchar pal;
  TColorDialog* WColor=new TColorDialog(&pal,new TColorGroup("Group1",new TColorItem("Color1",1)));
  deskTop->execView(WColor);
  delete WColor;*/
}

void TApp::evBaseDriver()
{
  WDriver *d=new WDriver();
  deskTop->execView(d);
  delete d;
}

void TApp::evBaseCar()
{
  WCar *c=new WCar();
  deskTop->execView(c);
  delete c;
}

void TApp::evBasePath()
{
  WPath*p=new WPath();
  deskTop->execView(p);
  delete p;
}

/*		WSettings		*/
WSettings::WSettings(TConfig cfg): TDialog(TRect(15,2,70,21),"����ன��"),
				TWindowInit( &WSettings::initFrame)
{
  iDBase=new TInputLine(TRect(35,2,50,3),13);
  iCBase=new TInputLine(TRect(35,4,50,5),13);
  iPBase=new TInputLine(TRect(35,6,50,7),13);
  iListFile=new TInputLine(TRect(35,8,50,9),13);
  iTempFile=new TInputLine(TRect(35,10,50,11),13);
  iPrnDevice=new TInputLine(TRect(35,12,50,13),13);
  cbIsAbout=new TCheckBoxes(TRect(2,14,50,15),new TSItem("�����뢠�� AboutBox �� ����᪥",0) );
  //inserts all these ctrls:
  insert(iDBase);
  insert(iCBase);
  insert(iPBase);
  insert(iListFile);
  insert(iTempFile);
  insert(iPrnDevice);
  //attach input lines to the labels & create buttons:
  insert(new TButton(TRect(19,16,32,18),"�⬥��",cmCancel,bfNormal));
  insert(cbIsAbout);
  TButton*b=new TButton(TRect(3,16,16,18),"OK",cmnSaveCFG,bfNormal);
  insert(b );
  insert(new TLabel(TRect(1,6,28,7),"��� 䠩�� ���� ������⮢:",iPBase));
  insert(new TLabel(TRect(1,4,28,5),"��� 䠩�� ���� ��設:",iCBase));
  insert(new TLabel(TRect(1,2,28,3),"��� 䠩�� ���� ����⥫��:",iDBase));
  insert(new TLabel(TRect(1,8,28,9),"��⠫�� ��� ��⥢�� ���⮢:",iListFile));
  insert(new TLabel(TRect(1,10,28,11),"�६���� 䠩�:",iTempFile));
  insert(new TLabel(TRect(1,12,28,13),"���ன�⢮ ����:",iPrnDevice));
  SetState(cfg);
}

WSettings::~WSettings()
{
}

void WSettings::GetState(TConfig&cfg)
{
  cfg.size=sizeof(cfg);
  strcpy(cfg.DriverBase,iDBase->data);
  strcpy(cfg.CarBase,iCBase->data);
  strcpy(cfg.PathBase,iPBase->data);
  strcpy(cfg.ListDir,iListFile->data);
  strcpy(cfg.TempFile,iTempFile->data);
  strcpy(cfg.PrnDevice,iPrnDevice->data);
  strcpy(cfg.STRING,"I need Nastya...");
  strcpy(cfg.Copyright,"Necro,2000");
  strcpy(cfg.GetOut,"���� ��⠢����?!");
  strcpy(cfg.OrgName,"��� ����஢᪮�� ���⠬� �.28-80-64");
  cfg.DisplayAboutBox=cbIsAbout->mark(0);
}

void WSettings::SetState(TConfig cfg)
{
  if (cfg.size!=sizeof(cfg)) messageBox(mfWarning|mfOKButton,"�訡�� � 䠩�� ����ன�� %s",cfgName);
  strcpy(iDBase->data,cfg.DriverBase);
  strcpy(iCBase->data,cfg.CarBase);
  strcpy(iPBase->data,cfg.PathBase);
  strcpy(iListFile->data,cfg.ListDir);
  strcpy(iTempFile->data,cfg.TempFile);
  strcpy(iPrnDevice->data,cfg.PrnDevice);
  if (cfg.DisplayAboutBox) cbIsAbout->press(0);
  redraw();
}

void WSettings::handleEvent(TEvent&event)
{
  TDialog::handleEvent(event); // act like base!
  if( event.what == evCommand )
  {
    switch( event.message.command )
    {
      case cmnSaveCFG:
	evSaveCFG();
	break;
      default:
	return;
    }//switch
    clearEvent( event );       // clear event after handling
  }//if
}

void WSettings::evSaveCFG()
{
  GetState(CFG);
  FILE*f=fopen(cfgName,"wb");
  fwrite(&CFG,sizeof CFG,1,f);
  fclose(f);
  messageBox("���䨣���� �뫠 �ᯥ譮 ��࠭���",mfOKButton|mfInformation);
  endModal(cmOK);//terminates dialog modal session
}

/*		WList		*/
WList::WList(char* listfile,char* Title) : TDialog(TRect(5,0,70,23),Title),
				TWindowInit( &WList::initFrame)
{
  strcpy(Path,listfile);
  ArtNum=IsSelect=0;
  eView=new TInputLine(TRect(2,20,50,21),63);
  eView->options=0;
  insert(eView);
  insert(new TStaticText(TRect(1,19,50,20),"� �롮�: ����������������������������������������"));
  //create new input lines:
  iPath=new TInputLine(TRect(15,1,55,2),50);
  // --- time ---
  iSAINum=new TInputLine(TRect(43,7,55,8),9);//<<
  iMark=new TInputLine(TRect(15,9,55,10),21);
  iMark->options=0;
  iFuel=new TInputLine(TRect(15,11,55,12),51);
  iListType=new TInputLine(TRect(15,7,20,8),2);//depends on Mark <<
  iDriver=new TInputLine(TRect(15,13,55,14),20);
  iManager=new TInputLine(TRect(15,15,55,16),20);

  iIMonth=new TInputLine(TRect(25,5,30,6),3);
  iIDay=new TInputLine(TRect(32,5,38,6),3);
  iIHour=new TInputLine(TRect(40,5,45,6),3);
  iIMinute=new TInputLine(TRect(47,5,52,6),3);

  iOMonth=new TInputLine(TRect(25,3,30,4),3);
  iODay=new TInputLine(TRect(32,3,38,4),3);
  iOHour=new TInputLine(TRect(40,3,45,4),3);
  iOMinute=new TInputLine(TRect(47,3,52,4),3);

  insert(new TStaticText(TRect(2,3,17,4),"�६� �륧��:"));
  insert(new TStaticText(TRect(2,5,17,6),"�६� �ꥧ��:"));
  insert(new TLabel(TRect(29,5,32,6),"���",iIMonth));
  insert(new TLabel(TRect(37,5,40,6),"��",iIDay));
  insert(new TLabel(TRect(44,5,48,6),"�",iIHour));
  insert(new TLabel(TRect(51,5,58,6),"���",iIMinute));

  insert(new TLabel(TRect(29,3,32,4),"���",iOMonth));
  insert(new TLabel(TRect(37,3,40,4),"��",iODay));
  insert(new TLabel(TRect(44,3,48,4),"�",iOHour));
  insert(new TLabel(TRect(51,3,58,4),"���",iOMinute));

  //inserts these lines:
  insert(iPath);
  insert(bSPath=new TButton( TRect( 55, 1, 62, 3 ), "...", cmnSelectPath, bfNormal));
  insert(iOMonth);
  insert(iODay);
  insert(iOHour);
  insert(iOMinute);

  insert(iIMonth);
  insert(iIDay);
  insert(iIHour);
  insert(iIMinute);

  insert(iListType);
  insert(iSAINum);
  insert(bSCar=new TButton( TRect( 55, 7, 62, 9 ), "...", cmnSelectCar, bfNormal));
  insert(iMark);
  insert(iFuel);
  insert(iDriver);
  insert(bSDriver=new TButton( TRect( 55, 13, 62, 15 ), "...", cmnSelectDriver, bfNormal));
  insert(iManager);
  //attaches input lines to labels:
  insert(new TLabel(TRect(1,1,12,2),"�������:",iPath));
  insert(new TLabel(TRect(30,7,43,8),"���.�����:",iSAINum));
  insert(new TLabel(TRect(1,9,12,10),"��ઠ:",iMark));
  insert(new TLabel(TRect(1,11,12,12),"����祥:",iFuel));
  insert(new TLabel(TRect(1,7,12,8),"��� ����:",iListType));
  insert(new TLabel(TRect(1,13,12,14),"����⥫�:",iDriver));
  insert(new TLabel(TRect(1,15,12,16),"��ᯥ���:",iManager));
  //and buttons:
  insert(new TButton( TRect( 50, 17, 62, 19 ), "��室", cmCancel, bfNormal));
  insert(new TButton( TRect( 1, 17, 13, 19 ), "�����", cmnPrintList, bfNormal));
  insert(new TButton( TRect( 13, 17, 25, 19 ), "���࠭���", cmnSaveList, bfNormal ));
  insert(new TButton( TRect( 25, 17, 37, 19 ), "����㧨��", cmnLoadList, bfNormal ));
  insert(new TButton( TRect( 37, 17, 47, 19 ), "�������", cmnArtDel, bfNormal ));
  insert(bPrev=new TButton( TRect( 49,20,55,22), "<<", cmnArtPrev, bfNormal));
  insert(bNext=new TButton( TRect( 55,20,61,22), ">>", cmnArtNext, bfNormal));
  bSPath->select();
}

WList::~WList()
{
}

void WList::handleEvent(TEvent&event)
{
  TDialog::handleEvent(event); // act like base!
  if ((event.what & evKeyDown) != 0)
  {
    switch(event.keyDown.keyCode)
    {
      case 0x4b00://left arrow
	switch (IsSelect)
	{
	  case PATH:
	    evSelectPath(cmnArtPrev);
	    break;
	  case CAR:
	    evSelectCar(cmnArtPrev);
	    break;
	  case DRIVER:
	    evSelectDriver(cmnArtPrev);
	    break;
	  case LIST:
	    evLoadList(cmnArtPrev);
	    break;
	  default://no default action now
	  ;
	}//switch IsSelect
	break;
      case 0x4d00://right arrow
	switch (IsSelect)
	{
	  case PATH:
	    evSelectPath(cmnArtNext);
	    break;
	  case CAR:
	    evSelectCar(cmnArtNext);
	    break;
	  case DRIVER:
	    evSelectDriver(cmnArtNext);
	    break;
	  case LIST:
	    evLoadList(cmnArtNext);
	    break;
	  default://no default action now
	  ;
	}//switch IsSelect
	break;
      default:
	switch (IsSelect)
	{
	  case PATH:
	    evSelectPath();
	    break;
	  case CAR:
	    evSelectCar();
	    break;
	  case DRIVER:
	    evSelectDriver();
	    break;
	  case LIST:
	    evLoadList();
	    break;
	  default://no default action now
	  ;
	}//switch IsSelect
    }//switch
  }//if
  if( event.what == evCommand )
  {
    switch( event.message.command )
    {
      //fundamental ctrls:
      case cmnPrintList:
	evPrintList();
	break;
      case cmnSaveList:
	evSaveList();
	break;
      case cmnLoadList:
	evLoadList();
	break;
      //additional ctrls:
      case cmnSelectCar:
	evSelectCar();
	break;
      case cmnSelectDriver:
	evSelectDriver();
	break;
      case cmnSelectPath:
	evSelectPath();
	break;
      case cmnArtDel:
	evArtDel();
	break;
      case cmnArtPrev:
	switch (IsSelect)
	{
	  case PATH:
	    evSelectPath(event.message.command);
	    break;
	  case CAR:
	    evSelectCar(event.message.command);
	    break;
	  case DRIVER:
	    evSelectDriver(event.message.command);
	    break;
	  case LIST:
	    evLoadList(event.message.command);
	    break;
	  default://no default action now
	  ;
	}//switch IsSelect
	break;
      case cmnArtNext:
	switch (IsSelect)
	{
	  case PATH:
	    evSelectPath(event.message.command);
	    break;
	  case CAR:
	    evSelectCar(event.message.command);
	    break;
	  case DRIVER:
	    evSelectDriver(event.message.command);
	    break;
	  case LIST:
	    evLoadList(event.message.command);
	    break;
	  default://no default action now
	  ;
	}//switch IsSelect
	break;
      default:
	;
	return;
    }//switch
    clearEvent( event );       // clear event after handling
  }//if
}

void WList::evArtDel()
{
  if (IsSelect!=LIST) return;
  messageBox(mfConfirmation|mfYesButton|mfNoButton,
	"�������� ���� '%s' �� %s",dList.Path,dList.sDateTime);
  fcopy(CFG.TempFile,Path);//create temporary copy
  unlink(Path);//destroy database... BO-O-O-OM!!!
  FILE *src=fopen(CFG.TempFile,"rb");
  FILE *dest=fopen(Path,"wb");
  for (int n=0;n<(fsize(src)/sizeof(dList)-1);n++) {
    if (n<ArtNum) fseek(src,n*sizeof(dList),SEEK_SET);
    else fseek(src,(n+1)*sizeof(dList),SEEK_SET);
    fread(&dList,sizeof(dList),1,src);
    fwrite(&dList,sizeof(dList),1,dest);
  }//for
  fclose(src);
  fclose(dest);
  unlink(CFG.TempFile);
  evLoadList(cmnArtPrev);
}

void WList::evLoadList(int button)
{
  if ((IsSelect==LIST)&&(button==0)) {
    IsSelect=0;
    iPath->select();
    UpdateScreen();
    SaveScreen();
    return;
  }//if IsSelect
  //previously close other sesions:
  if (IsSelect==DRIVER) evSelectDriver();
  if (IsSelect==PATH) evSelectPath();
  if (IsSelect==CAR) evSelectCar();
  //and now, all together! {selection mode itself}
  if (IsSelect!=LIST) {
    strcpy(eView->data,"�ᯮ���� [ << ] � [ >> ] ��� �롮� �����");
    eView->draw();
    ArtNum=0;
  }
  IsSelect=LIST;
  if ((button==cmnArtPrev)&&(ArtNum>0)) ArtNum--;//"<<" button pressed
  if ((button==cmnArtNext)&& (ArtNum<(ArtSize-1))) ArtNum++;//">>" button pressed
  FILE*file=fopen(Path,"rb");
  if (!file)
  {
    messageBox(mfError|mfOKButton,"���� � 㪠����� ������ (%s) �� ������",Path);
    iPath->select();
    IsSelect=0;
    return;//leave selection mode
  }
  ArtSize=fsize(file)/sizeof(dList);
  if (ArtSize==0) {
    fclose(file);
    IsSelect=0;
    return;
  }
  fseek(file,ArtNum*sizeof(dList),SEEK_SET);
  fread(&dList,sizeof(dList),1,file);
  if (dList.size!=sizeof(dList)) messageBox("������ �ଠ� ������",mfWarning|mfOKButton);
  char  buff[60];
  sprintf(buff,"%u/%u %s : %s",ArtNum+1,ArtSize,dList.sDateTime,dList.Path);
  strcpy(eView->data,buff);
  UpdateScreen();
  fclose(file);
}

void WList::evSaveList()
{
  SaveScreen();
  dostime_t t;
  dosdate_t d;
  _dos_getdate(&d);
  _dos_gettime(&t);
  sprintf(dList.sDateTime,"%02d.%02d.%02d %02d:%02d",d.day,d.month,d.year,t.hour,t.minute);
  dList.size=sizeof(dList);
  FILE* file=fopen(Path,"ab");
  fseek(file,0,SEEK_END);
  fwrite(&dList,sizeof(dList),1,file);
  ArtSize=fsize(file)/dList.size;
  fclose(file);
  messageBox(mfInformation|mfOKButton,"���� �� �ᯥ譮 ��࠭� � 䠩� %s (%u ��⥩)",Path,ArtSize);
}

void WList::evPrintList()
{
  char buf[20];
  switch(dList.Type) {
    case 1:
      strcpy(buf,"��㧮���");
      break;
    case 2:
      strcpy(buf,"��⮡���");
      break;
    case 3:
      strcpy(buf,"ᯥ樠���");
      break;
    default:
      strcpy(buf,"��������");
  }//switch
  messageBox(mfOKButton|mfInformation,"��⠢�� '%s' ����� (⨯ %u)",buf,dList.Type);
  FILE* file=fopen(CFG.DriverBase,"rb");
  fseek(file,sizeof(dDriver)*dList.idDriver,SEEK_SET);
  fread(&dDriver,sizeof(dDriver),1,file);
  fclose(file);

  file=fopen(CFG.CarBase,"rb");
  fseek(file,sizeof(dCar)*dList.idCar,SEEK_SET);
  fread(&dCar,sizeof(dCar),1,file);
  fclose(file);

  file=fopen(CFG.PathBase,"rb");
  fseek(file,sizeof(dPath)*dList.idPath,SEEK_SET);
  fread(&dPath,sizeof(dPath),1,file);
  fclose(file);

  CFG.Seed++;
  switch (dList.Type)  {
    case 1:
	Print_T1(dList,dDriver,dCar,dPath);
	break;
    case 2:
	Print_T2(dList,dDriver,dCar,dPath);
	break;
    case 3:
	Print_T3(dList,dDriver,dCar,dPath);
	break;
    default:
	Print_T0(dList,dDriver,dCar,dPath);
    }//switch
}

void WList::evSelectPath(int button)
{
  if ((IsSelect==PATH)&&(button==0)) {//already in selection mode & it's not a button event
    IsSelect=0;
    bSCar->select();
    //& all Path-linked dataunits must be set
    dList.idPath=ArtNum;
    ArtNum=0;
    strcpy(dList.Path,dPath.Name);
    strcpy(dList.StartTime.Month,dPath.StartTime.Month);
    strcpy(dList.StartTime.Day,dPath.StartTime.Day);
    strcpy(dList.StartTime.Hour,dPath.StartTime.Hour);
    strcpy(dList.StartTime.Min,dPath.StartTime.Min);
    strcpy(dList.EndTime.Month,dPath.EndTime.Month);
    strcpy(dList.EndTime.Day,dPath.EndTime.Day);
    strcpy(dList.EndTime.Hour,dPath.EndTime.Hour);
    strcpy(dList.EndTime.Min,dPath.EndTime.Min);
    UpdateScreen();//redraw window
    //messageBox("leave selection session of WList::evSelectDriver()",mfInformation|mfOKButton);
    return;
  }//if IsSelect
  //previous close another selection sessions:
  if (IsSelect==CAR) evSelectCar();
  if (IsSelect==DRIVER) evSelectDriver();
  if (IsSelect==LIST) evLoadList();
  //and now, all together! {selection mode itself}
  if (IsSelect!=PATH) {
    SaveScreen();//prevent non-PATH data to be changed
    ArtNum=0;
  }
  IsSelect=PATH;
  //messageBox(mfInformation|mfOKButton,"entered main part of WList::evSelectDriver(), ArtNum=%d",ArtNum);
  if ((button==cmnArtPrev)&&(ArtNum>0)) ArtNum--;//"<<" button pressed
  if ((button==cmnArtNext)&& (ArtNum<(ArtSize-1))) ArtNum++;//">>" button pressed
  FILE*file=fopen(CFG.PathBase,"rb");
  if (!file)
  {
    messageBox(mfError|mfOKButton,"���� � 㪠����� ������ (%s) �� ������",CFG.PathBase);
    iPath->select();
    IsSelect=0;
    return;//leave selection mode
  }
  fseek(file,0,SEEK_END);
  ArtSize=ftell(file)/sizeof(dPath);
  fseek(file,ArtNum*sizeof(dPath),SEEK_SET);
  fread(&dPath,sizeof(dPath),1,file);
  if (dPath.size!=sizeof(dPath)) messageBox("������ �ଠ� ������",mfWarning|mfOKButton);
  char  buff[60];
  sprintf(buff,"%s (%s.%s %s:%s - %s.%s %s:%s)",dPath.Name,dPath.StartTime.Day,
	dPath.StartTime.Month,dPath.StartTime.Hour,dPath.StartTime.Min,
	dPath.EndTime.Day,dPath.EndTime.Month,dPath.EndTime.Hour,dPath.EndTime.Min);
  strcpy(eView->data,buff);
  eView->draw();
  fclose(file);
}

void WList::evSelectCar(int button)
{
  if ((IsSelect==CAR)&&(button==0)) {
    IsSelect=0;
    bSDriver->select();
    dList.idCar=ArtNum;
    ArtNum=0;
    strcpy(dList.SAINum,dCar.SAINum);
    strcpy(dList.Mark,dCar.Mark);
    strcpy(dList.Fuel,dCar.Fuel);
    dList.Type=dCar.Type;
    UpdateScreen();
    return;
  }//if IsSelect
  //previously close other sesions:
  if (IsSelect==LIST) evLoadList();
  if (IsSelect==PATH) evSelectPath();
  if (IsSelect==DRIVER) evSelectDriver();
  //and now, all together! {selection mode itself}
  if (IsSelect!=CAR) {
    SaveScreen();//prevent non-CAR data to be changed
    ArtNum=0;
  }
  IsSelect=CAR;
  if ((button==cmnArtPrev)&&(ArtNum>0)) ArtNum--;//"<<" button pressed
  if ((button==cmnArtNext)&& (ArtNum<(ArtSize-1))) ArtNum++;//">>" button pressed
  FILE*file=fopen(CFG.CarBase,"rb");
  if (!file)
  {
    messageBox(mfError|mfOKButton,"���� � 㪠����� ������ (%s) �� ������",CFG.CarBase);
    iSAINum->select();
    IsSelect=0;
    return;//leave selection mode
  }
  fseek(file,0,SEEK_END);
  ArtSize=ftell(file)/sizeof(dCar);
  fseek(file,ArtNum*sizeof(dCar),SEEK_SET);
  fread(&dCar,sizeof(dCar),1,file);
  if (dCar.size!=sizeof(dCar)) messageBox("������ �ଠ� ������",mfWarning|mfOKButton);
  char  buff[60];
  sprintf(buff,"%s (%s)",dCar.SAINum,dCar.Mark);
  strcpy(eView->data,buff);
  eView->draw();
  fclose(file);
}

void WList::evSelectDriver(int button)
{
  if ((IsSelect==DRIVER)&&(button==0)) {
    IsSelect=0;
    iManager->select();
    dList.idDriver=ArtNum;
    ArtNum=0;
    strcpy(dList.Driver,dDriver.Name);
    UpdateScreen();
    return;
  }//if IsSelect
  //previously close other sesions:
  if (IsSelect==LIST) evLoadList();
  if (IsSelect==PATH) evSelectPath();
  if (IsSelect==CAR) evSelectCar();
  //and now, all together! {selection mode itself}
  if (IsSelect!=DRIVER) {
    SaveScreen();//prevent non-DRIVER data to be changed
    ArtNum=0;
  }
  IsSelect=DRIVER;
  if ((button==cmnArtPrev)&&(ArtNum>0)) ArtNum--;//"<<" button pressed
  if ((button==cmnArtNext)&& (ArtNum<(ArtSize-1))) ArtNum++;//">>" button pressed
  FILE*file=fopen(CFG.DriverBase,"rb");
  if (!file)
  {
    messageBox(mfError|mfOKButton,"���� � 㪠����� ������ (%s) �� ������",CFG.DriverBase);
    iDriver->select();
    IsSelect=0;
    return;//leave selection mode
  }
  fseek(file,0,SEEK_END);
  ArtSize=ftell(file)/sizeof(dDriver);
  fseek(file,ArtNum*sizeof(dDriver),SEEK_SET);
  fread(&dDriver,sizeof(dDriver),1,file);
  if (dDriver.size!=sizeof(dDriver)) messageBox("������ �ଠ� ������",mfWarning|mfOKButton);
  char  buff[60];
  sprintf(buff,"%s (������� %s, �ਣ��� %s)",dDriver.Name,dDriver.Column,dDriver.Square);
  strcpy(eView->data,buff);
  eView->draw();
  fclose(file);
}

void WList::SaveScreen()
{
  //memset(&dList,0,sizeof(dList));
  //path save:
  strcpy(dList.Path,iPath->data);
  strcpy(dList.StartTime.Month,iOMonth->data);
  strcpy(dList.StartTime.Day,iODay->data);
  strcpy(dList.StartTime.Hour,iOHour->data);
  strcpy(dList.StartTime.Min,iOMinute->data);
  strcpy(dList.EndTime.Month,iIMonth->data);
  strcpy(dList.EndTime.Day,iIDay->data);
  strcpy(dList.EndTime.Hour,iIHour->data);
  strcpy(dList.EndTime.Min,iIMinute->data);
  //car save:
  strcpy(dList.SAINum,iSAINum->data);
  strcpy(dList.Mark,iMark->data);
  strcpy(dList.Fuel,iFuel->data);
  char buff[5],byte;
  strcpy(buff,iListType->data);
  byte=buff[0];
  if ((byte<48) || (byte>51)) byte=48;
  byte-=48;
  dList.Type=byte;
  //driver save:
  strcpy(dList.Driver,iDriver->data);
  //misc save:
  strcpy(dList.Manager,iManager->data);
}

void WList::UpdateScreen()
{
  //path update:
  strcpy(iPath->data,dList.Path);
  strcpy(iOMonth->data,dList.StartTime.Month);
  strcpy(iODay->data,dList.StartTime.Day);
  strcpy(iOHour->data,dList.StartTime.Hour);
  strcpy(iOMinute->data,dList.StartTime.Min);
  strcpy(iIMonth->data,dList.EndTime.Month);
  strcpy(iIDay->data,dList.EndTime.Day);
  strcpy(iIHour->data,dList.EndTime.Hour);
  strcpy(iIMinute->data,dList.EndTime.Min);
  //car update:
  strcpy(iSAINum->data,dList.SAINum);
  strcpy(iMark->data,dList.Mark);
  strcpy(iFuel->data,dList.Fuel);
  sprintf(iListType->data,"%u",dList.Type);
  //driver update:
  strcpy(iDriver->data,dList.Driver);
  //misc update:
  strcpy(iManager->data,dList.Manager);
  redraw();
}


/*		WDriver database editor			*/
WDriver::WDriver(): TDialog(TRect(6,1,71,21),"����⥫�"),
				TWindowInit( &WDriver::initFrame)
{

  //create controls:
  tArtInfo=new TStaticText(TRect(5,1,55,2),"---");//article ## of ##
  iName= new TInputLine(TRect(15,3,55,4),31);
  iNum=  new TInputLine(TRect(15,5,55,6),11);
  iDoc=  new TInputLine(TRect(15,7,55,8),11);
  iClass=new TInputLine(TRect(15,9,55,10),2);
  iColumn=new TInputLine(TRect(15,11,55,12),2);
  iSquare=new TInputLine(TRect(15,13,55,14),2);

  //create & insert non-modifyable ctrls
  insert(new TButton(TRect( 1, 17, 7, 19 ), "<<", cmnArtPrev, bfNormal));
  insert(new TButton(TRect( 7, 17, 13, 19 ), ">>", cmnArtNext, bfNormal));
  insert(new TButton(TRect( 13, 17, 26, 19 ), "���࠭���", cmnArtSave, bfNormal));
  insert(new TButton(TRect( 26, 17, 36, 19 ), "����", cmnArtNew, bfNormal));
  insert(new TButton(TRect( 36, 17, 46, 19 ), "�������", cmnArtDel, bfNormal));
  insert(new TButton(TRect( 46, 17, 56, 19 ), "��室", cmCancel, bfNormal));

  //insert last time created ctrls
  insert(tArtInfo);

  insert(iName);
  insert(iNum);
  insert(iDoc);
  insert(iClass);
  insert(iColumn);
  insert(iSquare);

  insert(new TLabel(TRect(1,3,12,4),"�������:",iName));
  insert(new TLabel(TRect(1,5,12,6),"���.�����:",iNum));
  insert(new TLabel(TRect(1,7,12,8),"����⢥७��:",iDoc));
  insert(new TLabel(TRect(1,9,12,10),"�����:",iClass));
  insert(new TLabel(TRect(1,11,12,12),"�������:",iColumn));
  insert(new TLabel(TRect(1,13,12,14),"�ਣ���:",iSquare));

  file=fopen(CFG.DriverBase,"rb+");
  if (!file)
  {
    messageBox(mfError|mfOKButton,"���� � 㪠����� ������ (%s) �� ������ � �㤥� ᮧ���",CFG.DriverBase);
    file=fopen(CFG.DriverBase,"wb+");
  }
  fseek(file,0,SEEK_END);
  size=ftell(file)/sizeof(DDriver);
  ptr=0;
  ViewUpdate();
}

WDriver::~WDriver()
{
  fclose(file);
}

void WDriver::handleEvent(TEvent& event)
{
  TDialog::handleEvent(event); // act like base!
  if( event.what == evCommand )
  {
    switch( event.message.command )
    {
      case cmnArtNext:
	if (ptr<(size-1)) ptr++;
	else ptr=0;
	ViewUpdate();
	break;
      case cmnArtPrev:
	if (ptr>0) ptr--;
	else ptr=size-1;
	ViewUpdate();
	break;
      case cmnArtSave:
	fseek(file,ptr*sizeof(data),SEEK_SET);
	GetState(data);
	fwrite(&data,sizeof(data),1,file);
	break;
      case cmnArtDel:
	  messageBox(mfConfirmation|mfOKButton,
		"�������� ������ '%s'",data.Name);
	  fclose(file);
	  fcopy(CFG.TempFile,CFG.DriverBase);//create temporary copy
	  unlink(CFG.DriverBase);//destroy database... BO-O-O-OM!!!
	  FILE *src=fopen(CFG.TempFile,"rb");
	  FILE *dest=fopen(CFG.DriverBase,"wb");
	  for (int n=0;n<(fsize(src)/sizeof(data)-1);n++) {
	    if (n<ptr) fseek(src,n*sizeof(data),SEEK_SET);
	    else fseek(src,(n+1)*sizeof(data),SEEK_SET);
	    fread(&data,sizeof(data),1,src);
	    fwrite(&data,sizeof(data),1,dest);
	  }//for
	  fclose(src);
	  fclose(dest);
	  unlink(CFG.TempFile);
	  file=fopen(CFG.DriverBase,"rb+");
	  size=fsize(file)/sizeof(data);
	  if (ptr>0) ptr--;
	  else ptr=size-1;
	  ViewUpdate();
	break;
      case cmnArtNew:
	fseek(file,0,SEEK_END);
	GetState(data);
	fwrite(&data,sizeof(data),1,file);
	ptr=size;
	size++;
	break;
      default:
	return;
    }//switch
    clearEvent( event );       // clear event after handling
  }//if
}

void WDriver::GetState(DDriver&dest)
{
  memset(&dest,0,sizeof(dest));
  strcpy(dest.Name,iName->data);
  strcpy(dest.TabId,iNum->data);
  strcpy(dest.DocId,iDoc->data);
  strcpy(dest.Class,iClass->data);
  strcpy(dest.Column,iColumn->data);
  strcpy(dest.Square,iSquare->data);
  dest.size=sizeof(dest);
}

void WDriver::SetState(DDriver src)
{
  char buff[50];
  if (src.size!=sizeof(src)) messageBox("������ �ଠ� ������",mfWarning|mfOKButton);
  strcpy(iName->data,src.Name);
  strcpy(iNum->data,src.TabId);
  strcpy(iDoc->data,src.DocId);
  strcpy(iClass->data,src.Class);
  strcpy(iColumn->data,src.Column);
  strcpy(iSquare->data,src.Square);
  sprintf(buff,"����� %d �� %d",ptr+1,size);
  remove(tArtInfo);
  delete tArtInfo;
  tArtInfo=new TStaticText(TRect(5,1,55,2),buff);
  insert(tArtInfo);
  redraw();
}

void WDriver::ViewUpdate()//loads current article from file & calls SetState()
{
  fseek(file,ptr*sizeof(data),SEEK_SET);
  fread(&data,sizeof(data),1,file);
  SetState(data);
}

/*              WCar database editor		*/
WCar::WCar(): TDialog(TRect(7,1,72,21),"��設�"),
				TWindowInit( &WCar::initFrame)
{
  //create controls:
  tArtInfo=new TStaticText(TRect(5,1,55,2),"---");//article ## of ##

  iSAINum= new TInputLine(TRect(15,3,55,4),9);
  iMark=  new TInputLine(TRect(15,5,55,6),21);
  iLicenseReg=  new TInputLine(TRect(15,7,25,8),16);
  iLicenseSer=  new TInputLine(TRect(33,7,43,8),11);
  iLicenseNum=  new TInputLine(TRect(45,7,55,8),11);
  iFuel=new TInputLine(TRect(15,9,55,10),51);
  iType=new TInputLine(TRect(15,11,19,12),2);//   <<<--!!!!!!!!!!!!!
  iDriver=new TInputLine(TRect(15,13,55,14),51);//   <<<--!!!!!!!!!!!!!

  //create & insert non-modifyable ctrls
  insert(new TButton(TRect( 1, 17, 7, 19 ), "<<", cmnArtPrev, bfNormal));
  insert(new TButton(TRect( 7, 17, 13, 19 ), ">>", cmnArtNext, bfNormal));
  insert(new TButton(TRect( 13, 17, 26, 19 ), "���࠭���", cmnArtSave, bfNormal));
  insert(new TButton(TRect( 26, 17, 36, 19 ), "����", cmnArtNew, bfNormal));
  insert(new TButton(TRect( 36, 17, 46, 19 ), "�������", cmnArtDel, bfNormal));
  insert(new TButton(TRect( 46, 17, 56, 19 ), "��室", cmCancel, bfNormal));
  insert(new TStaticText(TRect(20,11,55,12),"0-���., 1-���., 2-��⮡��, 3-ᯥ�."));

  //insert last time created ctrls
  insert(tArtInfo);

  insert(iSAINum);
  insert(iMark);
  insert(iLicenseReg);
  insert(iLicenseSer);
  insert(iLicenseNum);
  insert(iFuel);
  insert(iType);
  insert(iDriver);

  insert(new TLabel(TRect(1,3,12,4),"���.�����:",iSAINum));
  insert(new TLabel(TRect(1,5,12,6),"��ઠ:",iMark));
  insert(new TLabel(TRect(1,7,12,8),"���. �����",iLicenseReg));
  insert(new TLabel(TRect(25,7,33,8),"����",iLicenseSer));
  insert(new TLabel(TRect(43,7,45,8),"�",iLicenseReg));
  insert(new TLabel(TRect(1,9,12,10),"����祥:",iFuel));
  insert(new TLabel(TRect(1,11,12,12),"��� ����:",iType));
  insert(new TLabel(TRect(1,13,12,14),"����⥫�:",iDriver));

  file=fopen(CFG.CarBase,"rb+");
  if (!file)
  {
    messageBox(mfError|mfOKButton,"���� � 㪠����� ������ (%s) �� ������ � �㤥� ᮧ���",CFG.CarBase);
    file=fopen(CFG.CarBase,"wb+");
  }
  fseek(file,0,SEEK_END);
  size=ftell(file)/sizeof(data);
  ptr=0;
  ViewUpdate();
}

WCar::~WCar()
{
  fclose(file);
}

void WCar::handleEvent(TEvent& event)
{
  TDialog::handleEvent(event); // act like base!
  if( event.what == evCommand )
  {
    switch( event.message.command )
    {
      case cmnArtNext:
	if (ptr<(size-1)) ptr++;
	else ptr=0;
	ViewUpdate();
	break;
      case cmnArtPrev:
	if (ptr>0) ptr--;
	else ptr=size-1;
	ViewUpdate();
	break;
      case cmnArtSave:
	fseek(file,ptr*sizeof(data),SEEK_SET);
	GetState(data);
	fwrite(&data,sizeof(data),1,file);
	break;
      case cmnArtDel:
	  messageBox(mfConfirmation|mfOKButton,
		"�������� ������ '%s (%s)'",data.SAINum,data.Mark);
	  fclose(file);
	  fcopy(CFG.TempFile,CFG.CarBase);//create temporary copy
	  unlink(CFG.CarBase);//destroy database... BO-O-O-OM!!!
	  FILE *src=fopen(CFG.TempFile,"rb");
	  FILE *dest=fopen(CFG.CarBase,"wb");
	  for (int n=0;n<(fsize(src)/sizeof(data)-1);n++) {
	    if (n<ptr) fseek(src,n*sizeof(data),SEEK_SET);
	    else fseek(src,(n+1)*sizeof(data),SEEK_SET);
	    fread(&data,sizeof(data),1,src);
	    fwrite(&data,sizeof(data),1,dest);
	  }//for
	  fclose(src);
	  fclose(dest);
	  unlink(CFG.TempFile);
	  file=fopen(CFG.CarBase,"rb+");
	  size=fsize(file)/sizeof(data);
	  if (ptr>0) ptr--;
	  else ptr=size-1;
	  ViewUpdate();
	break;
      case cmnArtNew:
	fseek(file,0,SEEK_END);
	GetState(data);
	fwrite(&data,sizeof(data),1,file);
	ptr=size;
	size++;
	break;
      default:
	return;
    }//switch
    clearEvent( event );       // clear event after handling
  }//if
}

void WCar::GetState(DCar& dest)
{
  char byte=0;
  memset(&dest,0,sizeof(dest));
  strcpy(dest.SAINum,iSAINum->data);
  strcpy(dest.Mark,iMark->data);
  strcpy(dest.Fuel,iFuel->data);
  strcpy(dest.LicenseReg,iLicenseReg->data);
  strcpy(dest.LicenseSer,iLicenseSer->data);
  strcpy(dest.LicenseNum,iLicenseNum->data);
  char buff[5];
  strcpy(buff,iType->data);
  byte=buff[0];
  if ((byte<48) || (byte>51)) byte=48;
  byte-=48;
//  sscanf(buff,"%d",&byte);//�� ࠡ�⠥�
//  if ((byte<0) || (byte>3)) byte=0;
  dest.Type=byte;
  dest.size=sizeof(dest);
  //must be driver search...
}

void WCar::SetState(DCar src)
{
  char buff[50];
  if (src.size!=sizeof(src)) messageBox("������ �ଠ� ������",mfWarning|mfOKButton);
  strcpy(iSAINum->data,src.SAINum);
  strcpy(iMark->data,src.Mark);
  strcpy(iFuel->data,src.Fuel);
  strcpy(iLicenseReg->data,src.LicenseReg);
  strcpy(iLicenseSer->data,src.LicenseSer);
  strcpy(iLicenseNum->data,src.LicenseNum);

  sprintf(iType->data,"%u",src.Type);
  //here must be driver search...

  sprintf(buff,"����� %d �� %d",ptr+1,size);
  remove(tArtInfo);
//  delete tArtInfo; // HERE IS BUG
  tArtInfo=new TStaticText(TRect(5,1,55,2),buff);
  insert(tArtInfo);
  redraw();
}

void WCar::ViewUpdate()//reloads current article from file & calls SetState()
{
  fseek(file,ptr*sizeof(data),SEEK_SET);
  fread(&data,sizeof(data),1,file);
  SetState(data);
}

/*              WPath database editor		*/
WPath::WPath(): TDialog(TRect(6,1,71,21),"��������"),
				TWindowInit( &WPath::initFrame)
{

  //create controls:
  tArtInfo=new TStaticText(TRect(5,1,55,2),"---");//article ## of ##

  iName= new TInputLine(TRect(15,3,55,4),51);
  iNullPath=  new TInputLine(TRect(20,5,55,6),7);
  iQNorm=  new TInputLine(TRect(28,7,55,8),4);

  iIMonth=new TInputLine(TRect(25,9,30,10),3);
  iIDay=new TInputLine(TRect(32,9,38,10),3);
  iIHour=new TInputLine(TRect(40,9,45,10),3);
  iIMinute=new TInputLine(TRect(47,9,52,10),3);

  iOMonth=new TInputLine(TRect(25,11,30,12),3);
  iODay=new TInputLine(TRect(32,11,38,12),3);
  iOHour=new TInputLine(TRect(40,11,45,12),3);
  iOMinute=new TInputLine(TRect(47,11,52,12),3);

  //create & insert non-modifyable ctrls
  insert(new TButton(TRect( 1, 17, 7, 19 ), "<<", cmnArtPrev, bfNormal));
  insert(new TButton(TRect( 7, 17, 13, 19 ), ">>", cmnArtNext, bfNormal));
  insert(new TButton(TRect( 13, 17, 26, 19 ), "���࠭���", cmnArtSave, bfNormal));
  insert(new TButton(TRect( 26, 17, 36, 19 ), "����", cmnArtNew, bfNormal));
  insert(new TButton(TRect( 36, 17, 46, 19 ), "�������", cmnArtDel, bfNormal));
  insert(new TButton(TRect( 46, 17, 56, 19 ), "��室", cmCancel, bfNormal));

  //insert last time created ctrls
  insert(tArtInfo);

  insert(iName);
  insert(iNullPath);
  insert(iQNorm);

  insert(iIMonth);
  insert(iIDay);
  insert(iIHour);
  insert(iIMinute);

  insert(iOMonth);
  insert(iODay);
  insert(iOHour);
  insert(iOMinute);

  insert(new TLabel(TRect(1,3,12,4),"��������:",iName));
  insert(new TLabel(TRect(1,5,17,6),"�㫥��� �஡��:",iNullPath));
  insert(new TLabel(TRect(1,7,25,8),"����. ��������� ����:",iQNorm));

  insert(new TStaticText(TRect(2,9,17,10),"�६� �ꥧ��:"));
  insert(new TStaticText(TRect(2,11,17,12),"�६� �륧��:"));
  insert(new TLabel(TRect(29,9,32,10),"���",iIMonth));
  insert(new TLabel(TRect(37,9,40,10),"��",iIDay));
  insert(new TLabel(TRect(44,9,48,10),"�",iIHour));
  insert(new TLabel(TRect(51,9,58,10),"���",iIMinute));

  insert(new TLabel(TRect(29,11,32,12),"���",iOMonth));
  insert(new TLabel(TRect(37,11,40,12),"��",iODay));
  insert(new TLabel(TRect(44,11,48,12),"�",iOHour));
  insert(new TLabel(TRect(51,11,58,12),"���",iOMinute));

  file=fopen(CFG.PathBase,"rb+");
  if (!file)
  {
    messageBox(mfError|mfOKButton,"���� � 㪠����� ������ (%s) �� ������ � �㤥� ᮧ���",CFG.PathBase);
    file=fopen(CFG.PathBase,"wb+");
  }
  fseek(file,0,SEEK_END);
/* �����, �����?: */  size=ftell(file)/sizeof(data);
  ptr=0;
  ViewUpdate();
}

WPath::~WPath()
{
  fclose(file);
}

void WPath::handleEvent(TEvent&event)
{
  TDialog::handleEvent(event); // act like base!
  if( event.what == evCommand )
  {
    switch( event.message.command )
    {
      case cmnArtNext:
	if (ptr<(size-1)) ptr++;
	else ptr=0;
	ViewUpdate();
	break;
      case cmnArtPrev:
	if (ptr>0) ptr--;
	else ptr=size-1;
	ViewUpdate();
	break;
      case cmnArtSave:
	fseek(file,ptr*sizeof(data),SEEK_SET);
	GetState(data);
	fwrite(&data,sizeof(data),1,file);
	break;
      case cmnArtDel:
	  messageBox(mfConfirmation|mfOKButton,
		"�������� ������ '%s'",data.Name);
	  fclose(file);
	  fcopy(CFG.TempFile,CFG.PathBase);//create temporary copy
	  unlink(CFG.PathBase);//destroy database... BO-O-O-OM!!!
	  FILE *src=fopen(CFG.TempFile,"rb");
	  FILE *dest=fopen(CFG.PathBase,"wb");
	  for (int n=0;n<(fsize(src)/sizeof(data)-1);n++) {
	    if (n<ptr) fseek(src,n*sizeof(data),SEEK_SET);
	    else fseek(src,(n+1)*sizeof(data),SEEK_SET);
	    fread(&data,sizeof(data),1,src);
	    fwrite(&data,sizeof(data),1,dest);
	  }//for
	  fclose(src);
	  fclose(dest);
	  unlink(CFG.TempFile);
	  file=fopen(CFG.PathBase,"rb+");
	  size=fsize(file)/sizeof(data);
	  if (ptr>0) ptr--;
	  else ptr=size-1;
	  ViewUpdate();
	break;
      case cmnArtNew:
	fseek(file,0,SEEK_END);
	GetState(data);
	fwrite(&data,sizeof(data),1,file);
	ptr=size;
	size++;
	break;
      default:
	return;
    }//switch
    clearEvent( event );       // clear event after handling
  }//if
}

void WPath::ViewUpdate()//reloads current article from file & calls SetState()
{
  fseek(file,ptr*sizeof(data),SEEK_SET);
  fread(&data,sizeof(data),1,file);
  SetState(data);
}

void WPath::GetState(DPath& dest)
{
  memset(&dest,0,sizeof(dest));
  strcpy(dest.Name,iName->data);
  strcpy(dest.ZeroPath,iNullPath->data);
  strcpy(dest.QNorm,iQNorm->data);

  strcpy(dest.EndTime.Month,iIMonth->data);
  strcpy(dest.EndTime.Day,iIDay->data);
  strcpy(dest.EndTime.Hour,iIHour->data);
  strcpy(dest.EndTime.Min,iIMinute->data);
  strcpy(dest.StartTime.Month,iOMonth->data);
  strcpy(dest.StartTime.Day,iODay->data);
  strcpy(dest.StartTime.Hour,iOHour->data);
  strcpy(dest.StartTime.Min,iOMinute->data);
  dest.size=sizeof(dest);
}

void WPath::SetState(DPath src)
{
  char buff[50];
  if (src.size!=sizeof(src)) messageBox("������ �ଠ� ������",mfWarning|mfOKButton);
  sprintf(buff,"����� %d �� %d",ptr+1,size);
  remove(tArtInfo);
//  delete tArtInfo;
  tArtInfo=new TStaticText(TRect(5,1,55,2),buff);
  insert(tArtInfo);

  strcpy(iName->data,src.Name);
  strcpy(iNullPath->data,src.ZeroPath);
  strcpy(iQNorm->data,src.QNorm);
  strcpy(iIMonth->data,src.EndTime.Month);
  strcpy(iIDay->data,src.EndTime.Day);
  strcpy(iIHour->data,src.EndTime.Hour);
  strcpy(iIMinute->data,src.EndTime.Min);
  strcpy(iOMonth->data,src.StartTime.Month);
  strcpy(iODay->data,src.StartTime.Day);
  strcpy(iOHour->data,src.StartTime.Hour);
  strcpy(iOMinute->data,src.StartTime.Min);
  redraw();
}

WPrint::WPrint(char* listfile): TDialog(TRect(6,1,71,21),"����� ��࠭��� ���⮢"),
				TWindowInit( &WPrint::initFrame)
{
  ERROR=0;
  strcpy(Path,listfile);
  insert(iView[0]=new TInputLine(TRect(2,1,57,2),100));
  insert(iView[1]=new TInputLine(TRect(2,2,57,3),100));
  insert(iView[2]=new TInputLine(TRect(2,3,57,4),100));
  insert(iView[3]=new TInputLine(TRect(2,4,57,5),100));
  insert(iView[4]=new TInputLine(TRect(2,5,57,6),100));
  insert(iView[5]=new TInputLine(TRect(2,6,57,7),100));
  insert(iView[6]=new TInputLine(TRect(2,7,57,8),100));
  insert(iView[7]=new TInputLine(TRect(2,8,57,9),100));
  insert(iView[8]=new TInputLine(TRect(2,9,57,10),100));
  insert(iView[9]=new TInputLine(TRect(2,10,57,11),100));
  insert(iView[10]=new TInputLine(TRect(2,11,57,12),100));
  insert(iView[11]=new TInputLine(TRect(2,12,57,13),100));
  insert(iView[12]=new TInputLine(TRect(2,13,57,14),100));
  insert(iView[13]=new TInputLine(TRect(2,14,57,15),100));
  insert(iView[14]=new TInputLine(TRect(2,15,57,16),100));
  iView[0]->options=0;
  iView[1]->options=0;
  iView[2]->options=0;
  iView[3]->options=0;
  iView[4]->options=0;
  iView[5]->options=0;
  iView[6]->options=0;
  iView[7]->options=0;
  iView[8]->options=0;
  iView[9]->options=0;
  iView[10]->options=0;
  iView[11]->options=0;
  iView[12]->options=0;
  iView[13]->options=0;
  iView[14]->options=0;
  cbSelection=new TCheckBoxes(TRect(57,1,63,16),
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",
	new TSItem("",0
	))))))))))))))) );
  insert(new TButton(TRect( 1, 17, 22, 19 ), "������ ��࠭��", cmnPrintSelected, bfNormal));
  insert(new TButton(TRect( 22, 17, 32, 19 ), "�������", cmnArtDel, bfNormal));
  insert(new TButton(TRect( 32, 17, 38, 19 ), "<<", cmnArtPrev, bfNormal));
  insert(new TButton(TRect( 38, 17, 44, 19 ), ">>", cmnArtNext, bfNormal));
  insert(cbSelection);
  insert(new TButton(TRect( 53, 17, 63, 19 ), "��室", cmCancel, bfNormal));

  //file ops:
  FILE*file=fopen(Path,"rb");
  if (!file) {
    messageBox(mfError|mfOKButton,"���� �� ������ (%s). ���� � �� ��⠩���...",Path);
    ERROR=1;
  }
  ArtSize=fsize(file)/sizeof(dList);
  FirstArt=0;
  messageBox(mfOKButton,"���� %s, ��⥩ %d",Path,ArtSize);
  fclose(file);
  UpdateScreen();
}

WPrint::~WPrint()
{
}

void WPrint::handleEvent(TEvent&event)
{
TDialog::handleEvent(event); // act like base!
  if (FirstArt>=ArtSize) FirstArt=ArtSize-1;
  if (FirstArt<0) FirstArt=0;
  if( event.what == evCommand )
  {
    switch( event.message.command )
    {
      case cmnPrintSelected:
	evPrintScope();
	break;
      case cmnArtPrev:
	if (FirstArt>0) FirstArt--;
	UpdateScreen();
	break;
      case cmnArtNext:
	if (FirstArt<(ArtSize-1)) FirstArt++;
	UpdateScreen();
	break;
      case cmnArtDel:
	evArtDel();
	break;
      default:
	return;
    }//switch
    clearEvent( event );       // clear event after handling
  }//if
  if ((event.what & evKeyDown) != 0)
  {
    switch(event.keyDown.keyCode)
    {
      case 0x5100://pagedown
	if (FirstArt>9) FirstArt-=10;
	UpdateScreen();
	break;
      case 0x4900://pageup
	if (FirstArt<(ArtSize-10)) FirstArt+=10;
	UpdateScreen();
	break;
      case 0x4800://up arrow
	if (FirstArt<(ArtSize-1)) FirstArt++;
	UpdateScreen();
	break;
      case 0x5000://down arrow
	if (FirstArt>0) FirstArt--;
	UpdateScreen();
	break;
      default:
	;
    }//switch
    clearEvent( event );       // clear event after handling
  }
}

void WPrint::UpdateScreen()
{
  FILE*file=fopen(Path,"rb");
  if (!file) {
    messageBox(mfError|mfOKButton,"���� �� ������ (%s). �� ��祧.",Path);
    return;
  }
  char buf[100];
  char* ptr;
  char a=15;
  if ((ArtSize-FirstArt)<14) a=ArtSize-FirstArt;
  for (int i=0;i<a;i++) {
    fseek(file,sizeof(dList)*(FirstArt+i),SEEK_SET);
    fread(&dList,sizeof(dList),1,file);
    sprintf(buf,"[%s] %s",dList.sDateTime,dList.Path);
    strcpy(iView[i]->data,buf);
    iView[i]->draw();
  }
  for (i=a;i<15;i++) {
    strcpy(iView[i]->data,"");
    iView[i]->draw();
  }
  fclose(file);
//  redraw();
}

void WPrint::evPrintScope()
{
  int summ=0,num=0;
  for (int i=0;i<15;i++)
    if (cbSelection->mark(i) && (FirstArt+i)<ArtSize) summ++;
  messageBox(mfOKButton,"��࠭� ���⮢: %u �� %u",summ,ArtSize);
  for (i=0;i<15;i++) {
    if ((FirstArt+i)>ArtSize) continue;
    if (cbSelection->mark(i)) {
      CFG.Seed++;
      num++;
      FILE* file=fopen(Path,"rb");
      fseek(file,sizeof(dList)*(FirstArt+i),SEEK_SET);
      fread(&dList,sizeof(dList),1,file);
      fclose(file);

      file=fopen(CFG.DriverBase,"rb");
      fseek(file,sizeof(dDriver)*dList.idDriver,SEEK_SET);
      fread(&dDriver,sizeof(dDriver),1,file);
      fclose(file);

      file=fopen(CFG.CarBase,"rb");
      fseek(file,sizeof(dCar)*dList.idCar,SEEK_SET);
      fread(&dCar,sizeof(dCar),1,file);
      fclose(file);

      file=fopen(CFG.PathBase,"rb");
      fseek(file,sizeof(dPath)*dList.idPath,SEEK_SET);
      fread(&dPath,sizeof(dPath),1,file);
      fclose(file);
      messageBox(mfOKButton,"%u) [%s] %s (���:%s;���:%s;���:%s;���:%u;�%06u)",
		num,dList.sDateTime,dList.Path,dList.Driver,dList.SAINum,
		dList.Manager,dList.Type,CFG.Seed);
      switch (dList.Type)  {
	case 1:
	  Print_T1(dList,dDriver,dCar,dPath);
	  break;
	case 2:
	  Print_T2(dList,dDriver,dCar,dPath);
	  break;
	case 3:
	  Print_T3(dList,dDriver,dCar,dPath);
	  break;
	default:
	  Print_T0(dList,dDriver,dCar,dPath);
      }//switch
    }//if
  }//for
}

void WPrint::evArtDel()
{
  int summ=0,num=0;
  for (int i=0;i<15;i++)
    if (cbSelection->mark(i) && (FirstArt+i)<ArtSize) summ++;
  messageBox(mfOKButton,"�㤥� 㤠���� ���⮢: %u �� %u",summ,ArtSize);
  for (i=0;i<15;i++) {
    if ((!cbSelection->mark(i)) || (FirstArt+i)>ArtSize) continue;
    num++;
    fcopy(CFG.TempFile,Path);//create temporary copy
    unlink(Path);//destroy database... BO-O-O-OM!!!
    FILE *src=fopen(CFG.TempFile,"rb");
    FILE *dest=fopen(Path,"wb");
    for (int n=0;n<(fsize(src)/sizeof(dList)-1);n++) {
      if (n<(FirstArt+i)) fseek(src,n*sizeof(dList),SEEK_SET);
      else fseek(src,(n+1)*sizeof(dList),SEEK_SET);
      fread(&dList,sizeof(dList),1,src);
      fwrite(&dList,sizeof(dList),1,dest);
    }//for n
    ArtSize=fsize(dest)/sizeof(dList);
    UpdateScreen();
    fclose(src);
    fclose(dest);
    unlink(CFG.TempFile);
  }//for i
}

/*		main()		*/
int main()
{
  TApp app;
  app.run();
  return 0;
}