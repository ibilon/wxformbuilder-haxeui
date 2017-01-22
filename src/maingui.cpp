///////////////////////////////////////////////////////////////////////////////
//
// HaxeUI-editor - A visual UI editor for HaxeUI.
// Copyright (C) 2016 Valentin Lemière
// 
// Based on code from wxFormBuilder by José Antonio Hurtado
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////
#include "rad/mainframe.h"
#include "rad/appdata.h"
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/sysopt.h>
#include <wx/cmdline.h>
#include <wx/config.h>
#include <wx/stdpaths.h>
#include <wx/xrc/xmlres.h>
#include <wx/clipbrd.h>
#include <wx/msgout.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include "utils/wxfbexception.h"
#include <memory>
#include "maingui.h"

#include "utils/debug.h"
#include "utils/typeconv.h"
#include "model/objectbase.h"

void LogStack();

IMPLEMENT_APP(MyApp)

int MyApp::OnRun ()
{
	wxApp::SetAppName(wxT("HaxeUI-editor"));

	// Get the data directory
	wxStandardPathsBase& stdPaths = wxStandardPaths::Get();
	wxString dataDir = stdPaths.GetDataDir();
	dataDir.Replace(GetAppName().c_str(), wxT("haxeui-editor"));

	delete wxLog::SetActiveTarget(new wxLogGui());

	// Create singleton AppData - wait to initialize until sure that this is not the second
	// instance of a project file.
	AppDataCreate(dataDir);

	// Init handlers
	wxInitAllImageHandlers();
	wxXmlResource::Get()->InitAllHandlers();

	// Init AppData
	try
	{
		AppDataInit();
	}
	catch (wxFBException& ex)
	{
		wxLogError(_("Error loading application: %s\nhaxeui-editor cannot continue."), ex.what());
		wxLog::FlushActive();
		return 5;
	}

	wxSystemOptions::SetOption(wxT("msw.remap"), 0);
	wxSystemOptions::SetOption(wxT("msw.staticbox.optimized-paint"), 0);

	wxYield();

	// Read size and position from config file
	wxConfigBase* config = wxConfigBase::Get();
	config->SetPath(wxT("/mainframe"));
	int x, y, w, h;
	x = y = w = h = -1;
	config->Read(wxT("PosX"), &x);
	config->Read(wxT("PosY"), &y);
	config->Read(wxT("SizeW"), &w);
	config->Read(wxT("SizeH"), &h);

	config->SetPath(wxT("/"));

	m_frame = new MainFrame(NULL, -1, wxPoint(x, y), wxSize(w, h));
	m_frame->Show( TRUE );
	SetTopWindow( m_frame );

	// This is not necessary for wxFB to work. However, Windows sets the Current Working Directory
	// to the directory from which a .fbp file was opened, if opened from Windows Explorer.
	// This puts an unneccessary lock on the directory.
	// This changes the CWD to the already locked app directory as a workaround
	#ifdef __WXMSW__
	::wxSetWorkingDirectory(dataDir);
	#endif

	AppData()->NewProject();

#ifdef __WXMAC__
    // document to open on startup
    if (!m_mac_file_name.IsEmpty())
    {
		if (AppData()->LoadProject(m_mac_file_name))
		{
            m_frame->InsertRecentProject(m_mac_file_name);
		}
    }
#endif

	return wxApp::OnRun();
}

bool MyApp::OnInit ()
{
	// Initialization is done in OnRun, so MinGW SEH works for all code (it needs a local variable, OnInit is called before OnRun)
	return true;
}

int MyApp::OnExit ()
{
	MacroDictionary::Destroy();
	wxFlatNotebook::CleanUp();
	AppDataDestroy();

	if (!wxTheClipboard->IsOpened())
	{
        if (!wxTheClipboard->Open())
        {
            return wxApp::OnExit();
        }
	}

    // Allow clipboard data to persist after close
    wxTheClipboard->Flush();
    wxTheClipboard->Close();

	return wxApp::OnExit();
}

MyApp::~MyApp ()
{
}

#ifdef __WXMAC__
void MyApp::MacOpenFile (const wxString &fileName)
{
    if (m_frame == NULL)
	{
		m_mac_file_name = fileName;
	}
    else
    {
        if (!m_frame->SaveWarning())
		{
			return;
		}

        if (AppData()->LoadProject(fileName))
		{
            m_frame->InsertRecentProject(fileName);
		}
    }
}
#endif
