// Copyright 2013 Larry Bruce Daniel Atlanta Georgia
// All Rights Reserved
// State machine watches patterns in digit stream sourced via rdrand asm.
// Detects 268 sequence then 186 sequence to trigger actions.
#include <thread>
#include <unistd.h>
#include <iostream>
#include <wx/wx.h>
#include <wx/event.h>

#define IRON 71
#define ADV(s,e,v,n) if(s==e)if(v==n)
#define WHEN(c) if(c)
#define RET(v) return v

using namespace std;

static bool finished;
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
};
class MyDialog : public wxDialog
{
public:
    MyDialog();
    void textHandler(wxCommandEvent& event);
    void startHandler(wxCommandEvent& event);
    void stopHandler(wxCommandEvent& event);
    void closeHandler(wxCloseEvent& event);

    wxDECLARE_EVENT_TABLE();
    wxTextCtrl  * statusBar;
    wxButton * startButton;
    wxButton * stopButton;
};
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
	static MyDialog *dialog;
	dialog= new MyDialog();
    finished =true;
    dialog->Show();

    RET(true);
}
int MyApp::OnExit(){
	RET(0);
}



/* TESTED */
unsigned long reduce(unsigned long l) {
        WHEN (l<=0) l = 0;
        else {
                l %= 9;
                WHEN (l==0) l = 9;
        }
        RET(l);
}
/* Transmit */

void pause110() { usleep(110); }
void pause1s() { usleep(1000000); }
void pauseAsm() { asm("PAUSE"); }

void transmit(int t) {
static int A [[maybe_unused]];
    A=0;
    pause110();
    A=t;
    pause110( );
    A=0;
    pause1s();
}

long rnd(void) 
{
        static unsigned long a;
        a=0;
        asm("1: pause; rdrandq %0; jae 1b":"=r"(a));
        RET(a);
}
void worker(wxTextCtrl  * statusbar) {

    static int a=0, b=0, c=0;
    static int state=0; // 0-2: watch 268 seq ; 3-5: watch 186 seq
    statusbar->SetValue("Thread Started");
	statusbar->GetParent()->Refresh();
    pauseAsm();
loop:
            int value = reduce(rnd());
            ADV(state,0,value,2){ state=1; a=2; }
            ADV(state,1,value,6){ state=2; b=6; }
            ADV(state,2,value,8){
                    c=8;
                    statusbar->SetValue("CANDLESTICK");
		    statusbar->GetParent()->Refresh();
                    pauseAsm( );
                    state=3;
                    a=b=c=0;
            }
            ADV(state,3,value,1){ state=4; a=1; }
            ADV(state,4,value,8){ state=5; b=8; }
            ADV(state,5,value,6){
                    c=6;
                    statusbar->SetValue("ENDANGERMENT");
		    statusbar->GetParent()->Refresh();
                    pauseAsm () ;
                    transmit(IRON);
                    pause1s();
                    state=3;
            }
            pauseAsm() ;
            usleep(110000);
            WHEN(!finished) goto loop;
}

void MyDialog::startHandler(wxCommandEvent& event){
        WHEN(finished){
                static thread *t1;
                finished = false;
                statusBar->SetValue("first work started");
                this->SetDefaultItem(stopButton);
                startButton->SetLabel("start");
                stopButton->SetLabel("STOP");
                t1 = new thread(worker, statusBar);
                t1->detach();
        }
}
void MyDialog::stopHandler(wxCommandEvent& event){
		statusBar->SetValue("first work stopped");
		this->SetDefaultItem(startButton);
		startButton->SetLabel("START");
		stopButton->SetLabel("stop");
		finished = true;

}
void MyDialog::textHandler(wxCommandEvent& event){
}


void MyDialog::closeHandler(wxCloseEvent& event){
	finished= true;
	Hide();
	Destroy();
}
MyDialog::MyDialog()
    : wxDialog(NULL, wxID_ANY, "First Work")
{
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    // create text ctrl with minimal size 100x60
    statusBar = new wxTextCtrl(this, 2, "", wxDefaultPosition, wxDefaultSize, 0x0010);
    startButton = new wxButton( this, 3, "Start" );
    stopButton = new wxButton( this, 4, "stop" );
    // set border width to 10
    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    auto makeItem = [button_sizer](wxButton *btn) {
        button_sizer->Add(btn, 0, wxALL, 3);
    };
    makeItem(startButton);
    makeItem(stopButton);
    topsizer->Add(
        button_sizer,
        1,                // make vertically unstretchable
        wxALIGN_CENTER ); // no border and centre horizontally
    topsizer->Add(statusBar, 1, wxEXPAND);

    SetSizerAndFit(topsizer); // configure sizer layout and lock minimum size

}
wxBEGIN_EVENT_TABLE(MyDialog, wxDialog)
	EVT_TEXT(1,MyDialog::textHandler)
	EVT_BUTTON(3,MyDialog::startHandler)
	EVT_BUTTON(4,MyDialog::stopHandler)
	EVT_CLOSE(MyDialog::closeHandler)
wxEND_EVENT_TABLE()
