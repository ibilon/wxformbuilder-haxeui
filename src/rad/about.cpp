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

#include "about.h"
#include <wx/html/htmlwin.h>
#include <wx/mimetype.h>
#include <wx/filename.h>
#include "rad/appdata.h"
#include <wx/utils.h>

#define ID_DEFAULT -1 // Default
#define ID_OK 1000

BEGIN_EVENT_TABLE(AboutDialog,wxDialog)
  EVT_BUTTON(ID_OK,AboutDialog::OnButtonEvent)
END_EVENT_TABLE()

class HtmlWindow : public wxHtmlWindow
{
  public:
    HtmlWindow(wxWindow *parent) : wxHtmlWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
      wxHW_SCROLLBAR_NEVER | wxHW_NO_SELECTION | wxRAISED_BORDER)
    {
    }

    void LaunchBrowser(const wxString& url)
    {
      wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT("html"));
      if (!ft) {
        wxLogError(wxT("Impossible to determine the file type for extension html.\nPlease edit your MIME types."));
        return;
      }

      wxString cmd;
      bool ok = ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(url, wxT("")));
      delete ft;

      if (ok)
          wxExecute(cmd, wxEXEC_ASYNC);
    }

    void OnLinkClicked(const wxHtmlLinkInfo& link)
    {
    	#if wxCHECK_VERSION( 2, 8, 0 )
    	   	::wxLaunchDefaultBrowser( link.GetHref() );
		#else
			LaunchBrowser(link.GetHref());
		#endif
    }
};

AboutDialog::AboutDialog(wxWindow *parent, int id) : wxDialog(parent,id,wxT("About..."),wxDefaultPosition,wxSize(485,470))//wxSize(308,248))
{
  wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
  wxHtmlWindow *htmlWin = new HtmlWindow(this);

  // I don't know where is the problem, but if you call SetBorders(b) with
  // 'b' between 0..6 it works, but if you use a bigger border, it doesn't fit
  // correctly.
  htmlWin->SetBorders(5);

  htmlWin->LoadFile(wxFileName(AppData()->GetApplicationPath() + wxFILE_SEP_PATH + wxT("resources/about.html")));

#ifdef __WXMAC__
  // work around a wxMac bug
  htmlWin->SetSize(400, 600);
#else
  htmlWin->SetSize(htmlWin->GetInternalRepresentation()->GetWidth(),
                   htmlWin->GetInternalRepresentation()->GetHeight());
#endif


  mainSizer->Add(htmlWin, 1, wxEXPAND | wxALL, 5);
  mainSizer->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxALIGN_CENTER | wxBOTTOM, 5);

  SetSizerAndFit(mainSizer);
}

void AboutDialog::OnButtonEvent (wxCommandEvent &)
{
  Close();
}
