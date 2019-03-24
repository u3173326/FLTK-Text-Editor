// GUI code sure is ugly. I'm pretty sure I'm doing this all correctly too.
// And this is just FLTK, imagine how bad it would be if it were a different toolkit.

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>

char *filename = "untitled";

void open_cb(Fl_Widget* w, void* v)
{
    filename = fl_file_chooser("Open File?", "*", filename);
    ((Fl_Text_Buffer*)v)->loadfile(filename, sizeof(filename));
}

void save_cb(Fl_Widget* w, void* v)
{
    filename = fl_file_chooser("Save File?", "*", filename);
    ((Fl_Text_Buffer*)v)->savefile(filename, sizeof(filename));
}

class zoomInfo
{
    public:
    Fl_Text_Editor* texteditor;
    int zoom;

    zoomInfo(Fl_Text_Editor* texteditor, int zoom) {this->texteditor = texteditor; this->zoom = zoom;}
};

void zoom_cb(Fl_Widget* w, void* v)
{
    Fl_Text_Editor* texteditor = ((zoomInfo*)v)->texteditor;
    int zoom = ((zoomInfo*)v)->zoom;

    (texteditor->textsize(texteditor->textsize() + zoom));
    (texteditor->linenumber_size(texteditor->linenumber_size() + zoom));

    texteditor->redraw();
}

void wrap_cb(Fl_Widget* w, void* v)
{
    if( (( Fl_Toggle_Button*)w)->value() == 1 )
        (( Fl_Text_Editor*)v)->wrap_mode(3, 512);
    else
        (( Fl_Text_Editor*)v)->wrap_mode(0, 0);
}

void copy_cb(Fl_Widget* w, void* v)
{
    Fl_Text_Editor::kf_copy(0, (( Fl_Text_Editor*)v));
}

void paste_cb(Fl_Widget* w, void* v)
{
    Fl_Text_Editor::kf_paste(0, (( Fl_Text_Editor*)v));
}

static void finderWindowToggle_cb(Fl_Widget* w, void* v)
{
    if (((Fl_Window*)v)->shown() == 1)
        ((Fl_Window*)v)->hide();
    else
        ((Fl_Window*)v)->show();
}

class findReplaceInfo
{
    public:
    Fl_Text_Editor* texteditor;
    Fl_Input* findinput;
    Fl_Input* replaceinput;

    findReplaceInfo(Fl_Text_Editor* texteditor, Fl_Input* findinput, Fl_Input* replaceinput)
    { this->texteditor = texteditor; this->findinput = findinput; this->replaceinput = replaceinput; }
};

static void findOrReplace_cb(Fl_Widget* w, void* v)
{
    int position = ((findReplaceInfo*)v)->texteditor->insert_position();
    Fl_Text_Editor* texteditor = ((findReplaceInfo*)v)->texteditor;
    Fl_Input* findinput = ((findReplaceInfo*)v)->findinput;
    Fl_Input* replaceinput = ((findReplaceInfo*)v)->replaceinput;

    if (replaceinput->active() == 0)    // just find
    {
        if (texteditor->buffer()->search_forward(position, findinput->value(), &position))
        {
            texteditor->buffer()->select(position, position+strlen(findinput->value()));
            texteditor->insert_position(position+strlen(findinput->value()));
            texteditor->show_insert_position();
        }
        else fl_alert("Could not find!");
    }
    else                                // find and replace
    {
        while(texteditor->buffer()->search_forward(position, findinput->value(), &position))
        {
            texteditor->buffer()->select(position, position+strlen(findinput->value()));
            texteditor->buffer()->remove_selection();
            texteditor->buffer()->insert(position, replaceinput->value());
            texteditor->buffer()->select(position, position+strlen(findinput->value()));
            texteditor->insert_position(position+strlen(findinput->value()));
            texteditor->show_insert_position();
        }
    }
}

static void toggleReplace_cb(Fl_Widget* w, void* v)
{
    if (((Fl_Check_Button*)w)->value() == 0)
        ((Fl_Input*)v)->deactivate();
    else
        ((Fl_Input*)v)->activate();
}

void linenumber_cb(Fl_Widget* w, void* v)
{
    if( (( Fl_Toggle_Button*)w)->value() == 1 )
        (( Fl_Text_Editor*)v)->linenumber_width(32);
    else
        (( Fl_Text_Editor*)v)->linenumber_width(0);

    (( Fl_Text_Editor*)v)->redraw();
}

int main(int argc, char **argv)
{
    Fl_Double_Window* mainwindow;
    Fl_Window* finderwindow;
    Fl_Text_Buffer *textbuf;
    Fl_Text_Editor *texteditor;
    Fl_Check_Button* replacecheck;
    Fl_Button* searchbtn;
    Fl_Input* findinput;
    Fl_Input* replaceinput;

    Fl_Double_Window* o = new Fl_Double_Window(430, 330);
    mainwindow = o;

    textbuf = new Fl_Text_Buffer(64, 64);
    texteditor = new Fl_Text_Editor(5, 35, 420, 290);
    texteditor->buffer(textbuf);
    texteditor->textfont(FL_SCREEN);

    searchbtn = new Fl_Button(155, 6, 35, 24, "@search");
    searchbtn->labelsize(10);

    {   Fl_Button* o = new Fl_Button(5, 6, 32, 24, "@fileopen");
        o->labelsize(10);
        o->callback(open_cb, textbuf);
    }
    {   Fl_Button* o = new Fl_Button(40, 6, 32, 24, "@filesave");
        o->labelsize(10);
        o->callback(save_cb, textbuf);
    }
    {   Fl_Button* o = new Fl_Button(355, 6, 32, 24, "@line");
        o->labelsize(10);
        o->callback(zoom_cb, new zoomInfo(texteditor, -1));
    }
    {   Fl_Button* o = new Fl_Button(390, 6, 32, 24, "@+");
        o->labelsize(10);
        o->callback(zoom_cb, new zoomInfo(texteditor, 1));
    }
    {   Fl_Toggle_Button* o = new Fl_Toggle_Button(320, 6, 32, 24, "Wrap");
        o->labelsize(10);
        o->callback(wrap_cb, texteditor);
    }
    {   Fl_Toggle_Button* o = new Fl_Toggle_Button(285, 6, 32, 24, "Lines");
        o->labelsize(10);
        o->callback(linenumber_cb, texteditor);
    }
    {   Fl_Button* o = new Fl_Button(85, 6, 32, 24, "Copy");
        o->labelsize(10);
        o->callback(copy_cb, texteditor);
    }
    {   Fl_Button* o = new Fl_Button(120, 6, 32, 24, "Paste");
        o->labelsize(10);
        o->callback(paste_cb, texteditor);
    }
    Fl_Group::current()->resizable(texteditor);
    o->size_range(384, 192);
    o->end();


    Fl_Window* p = new Fl_Window(400, 130);
    finderwindow = p;

    searchbtn->callback(finderWindowToggle_cb, finderwindow);

    replaceinput = new Fl_Input(5, 71, 390, 24);
    replaceinput->labeltype(FL_NO_LABEL);
    replaceinput->deactivate();

    findinput = new Fl_Input(5, 22, 390, 23, "Find:");
    findinput->align(Fl_Align(FL_ALIGN_TOP_LEFT));

    replacecheck = new Fl_Check_Button(5, 50, 390, 15, "Replace:");
    replacecheck->down_box(FL_DOWN_BOX);
    replacecheck->callback(toggleReplace_cb, replaceinput);

    {   Fl_Button* p = new Fl_Button(5, 100, 190, 25, "Execute");
        p->callback(findOrReplace_cb, new findReplaceInfo(texteditor, findinput, replaceinput));
    }
    {   Fl_Button* p = new Fl_Button(205, 100, 190, 25, "Cancel");
        p->callback(finderWindowToggle_cb, finderwindow);
    }

    p->end();

    mainwindow->show();
    return Fl::run();
}
