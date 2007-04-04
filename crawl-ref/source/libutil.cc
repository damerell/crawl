/*
 *  File:       libutil.cc
 *  Summary:    Functions that may be missing from some systems
 *  Written by: ?
 *
 *  Modified for Crawl Reference by $Author$ on $Date$
 *
 *  Change History (most recent first):
 *
 *      <1> 2001/Nov/01        BWR     Created
 *
 */

#include "AppHdr.h"
#include "defines.h"
#include "initfile.h"
#include "libutil.h"
#include "externs.h"
#include "macro.h"
#include "stuff.h"
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#ifdef WIN32CONSOLE
    #include <windows.h>

    #ifdef WINMM_PLAY_SOUNDS
        #include <mmsystem.h>
    #endif
#endif

#ifdef REGEX_PCRE
    // Statically link pcre on Windows
    #ifdef WIN32CONSOLE
        #define PCRE_STATIC
    #endif

    #include <pcre.h>
#endif

#ifdef REGEX_POSIX
    // Do we still need to include sys/types.h?
    #include <sys/types.h>
    #include <regex.h>
#endif

#ifdef UNIX
static keycode_type numpad2vi(keycode_type key)
{
    if (key >= '1' && key <= '9')
    {
        const char *vikeys = "bjnh.lyku";
        return keycode_type(vikeys[key - '1']);
    }
    return (key);
}
#endif

int unmangle_direction_keys(int keyin, int km, bool fake_ctrl, bool fake_shift)
{
    const KeymapContext keymap = static_cast<KeymapContext>(km);
#ifdef UNIX
    // Kludging running and opening as two character sequences
    // for Unix systems.  This is an easy way out... all the
    // player has to do is find a termcap and numlock setting
    // that will get curses the numbers from the keypad.  This
    // will hopefully be easy.

    /* can we say yuck? -- haranp */
    if (fake_ctrl && keyin == '*')
    {
        keyin = getchm(keymap);
        // return control-key
        keyin = CONTROL(toupper(numpad2vi(keyin)));
    }
    else if (fake_shift && keyin == '/')
    {
        keyin = getchm(keymap);
        // return shift-key
        keyin = toupper(numpad2vi(keyin));
    }
#else
    // Old DOS keypad support
    if (keyin == 0)
    {
        /* FIXME haranp - hackiness */
        const char DOSidiocy[10]     = { "OPQKSMGHI" };
        const char DOSunidiocy[10]   = { "bjnh.lyku" };
        const int DOScontrolidiocy[9] =
        {
            117, 145, 118, 115, 76, 116, 119, 141, 132
        };
        keyin = getchm(keymap);
        for (int j = 0; j < 9; ++j )
        {
            if (keyin == DOSidiocy[j])
            {
                keyin = DOSunidiocy[j];
                break;
            }
            if (keyin == DOScontrolidiocy[j])
            {
                keyin = CONTROL(toupper(DOSunidiocy[j]));
                break;
            }
        }
    }
#endif

    // [dshaligram] More lovely keypad mangling.
    switch (keyin)
    {
#ifdef UNIX
    case '1': return 'b';
    case '2': return 'j';
    case '3': return 'n';
    case '4': return 'h';
    case '6': return 'l';
    case '7': return 'y';
    case '8': return 'k';
    case '9': return 'u';
#else
    case '1': return 'B';
    case '2': return 'J';
    case '3': return 'N';
    case '4': return 'H';
    case '6': return 'L';
    case '7': return 'Y';
    case '8': return 'K';
    case '9': return 'U';
#endif
    }

    return (keyin);
}

// Should return true if the filename contains nothing that
// the shell can do damage with.
bool shell_safe(const char *file)
{
    int match = strcspn(file, "\\`$*?|><&\n!");
    return !(match >= 0 && file[match]);
}

void play_sound( const char *file )
{
#if defined(WINMM_PLAY_SOUNDS)
    // Check whether file exists, is readable, etc.?
    if (file && *file)
        sndPlaySound(file, SND_ASYNC | SND_NODEFAULT);
#elif defined(SOUND_PLAY_COMMAND)
    char command[255];
    command[0] = 0;
    if (file && *file && (strlen(file) + strlen(SOUND_PLAY_COMMAND) < 255)
            && shell_safe(file))
    {
        snprintf(command, sizeof command, SOUND_PLAY_COMMAND, file);
        system(command);
    }
#endif
}

std::string strip_filename_unsafe_chars(const std::string &s)
{
    return replace_all_of(s, " .", "");
}

std::string make_stringf(const char *s, ...)
{
    va_list args;
    va_start(args, s);

    char buf[400];
    vsnprintf(buf, sizeof buf, s, args);

    va_end(args);

    return (buf);
}

void uppercase(std::string &s)
{
    /* yes, this is bad, but std::transform() has its own problems */
    for (unsigned int i = 0; i < s.size(); ++i)
        s[i] = toupper(s[i]);
}

void lowercase(std::string &s)
{
    for (unsigned int i = 0; i < s.size(); ++i)
        s[i] = tolower(s[i]);
}

std::string replace_all(std::string s,
                        const std::string &find,
                        const std::string &repl)
{
    std::string::size_type start = 0;
    std::string::size_type found;
   
    while ((found = s.find(find, start)) != std::string::npos)
    {
        s.replace( found, find.length(), repl );
        start = found + repl.length();
    }

    return (s);
}

// Replaces all occurrences of any of the characters in tofind with the
// replacement string.
std::string replace_all_of(std::string s,
                           const std::string &tofind,
                           const std::string &replacement)
{
    std::string::size_type start = 0;
    std::string::size_type found;
   
    while ((found = s.find_first_of(tofind, start)) != std::string::npos)
    {
        s.replace( found, 1, replacement );
        start = found + replacement.length();
    }

    return (s);
}

int count_occurrences(const std::string &text, const std::string &s)
{
    int nfound = 0;
    std::string::size_type pos = 0;

    while ((pos = text.find(s, pos)) != std::string::npos)
    {
        ++nfound;
        pos += s.length();
    }

    return (nfound);
}

void get_input_line( char *const buff, int len )
{
    buff[0] = 0;         // just in case

#if defined(UNIX)
    get_input_line_from_curses( buff, len ); // implemented in libunix.cc
#elif defined(WIN32CONSOLE)
    getstr( buff, len );
#else

    // [dshaligram] Turn on the cursor for DOS.
#ifdef DOS
    _setcursortype(_NORMALCURSOR);
#endif

    fgets( buff, len, stdin );  // much safer than gets()
#endif

    buff[ len - 1 ] = 0;  // just in case 

    // Removing white space from the end in order to get rid of any
    // newlines or carriage returns that any of the above might have 
    // left there (ie fgets especially).  -- bwr
    const int end = strlen( buff ); 
    int i; 

    for (i = end - 1; i >= 0; i++) 
    {
        if (isspace( buff[i] ))
            buff[i] = 0;
        else
            break;
    }
}

#ifdef DOS
static int getch_ck()
{
    int c = getch();
    if (!c)
    {
        switch (c = getch())
        {
        case 'O':  return CK_END;
        case 'P':  return CK_DOWN;
        case 'I':  return CK_PGUP;
        case 'H':  return CK_UP;
        case 'G':  return CK_HOME;
        case 'K':  return CK_LEFT;
        case 'Q':  return CK_PGDN;
        case 'M':  return CK_RIGHT;
        case 119:  return CK_CTRL_HOME;
        case 141:  return CK_CTRL_UP;
        case 132:  return CK_CTRL_PGUP;
        case 116:  return CK_CTRL_RIGHT;
        case 118:  return CK_CTRL_PGDN;
        case 145:  return CK_CTRL_DOWN;
        case 117:  return CK_CTRL_END;
        case 115:  return CK_CTRL_LEFT;
        case 'S':  return CK_DELETE;
        }
    }
    return c;
}
#endif

// Hacky wrapper around getch() that returns CK_ codes for keys
// we want to use in cancelable_get_line() and menus.
int c_getch()
{
#if defined(DOS) || defined(UNIX) || defined(WIN32CONSOLE)
    return getch_ck();
#else
    return getch();
#endif
}

// Wrapper around gotoxy that can draw a fake cursor for Unix terms where
// cursoring over darkgray or black causes problems.
void cursorxy(int x, int y)
{
#ifdef UNIX
    if (Options.use_fake_cursor)
        fakecursorxy(x, y);
    else
        gotoxy(x, y);
#else
    gotoxy(x, y);
#endif
}

// cprintf that knows how to wrap down lines (primitive, but what the heck)
int wrapcprintf( int wrapcol, const char *s, ... )
{
    char buf[1000]; // Hard max
    va_list args;
    va_start(args, s);

    // XXX: If snprintf isn't available, vsnprintf probably isn't, either.
    int len = vsnprintf(buf, sizeof buf, s, args);
    int olen = len;
    va_end(args);

    char *run = buf;
    while (len > 0)
    {
        int x = wherex(), y = wherey();

        if (x > wrapcol) // Somebody messed up!
            return 0;
        
        int avail = wrapcol - x + 1;
        int c = 0;
        if (len > avail)
        {
            c = run[avail];
            run[avail] = 0;
        }
        cprintf("%s", run);
        
        if (len > avail)
            run[avail] = c;

        if ((len -= avail) > 0)
            gotoxy(1, y + 1);
        run += avail;
    }
    return (olen);
}

int cancelable_get_line( char *buf, int len, int maxcol,
                         input_history *mh, int (*keyproc)(int &ch) )
{
    line_reader reader(buf, len, maxcol);
    reader.set_input_history(mh);
    reader.set_keyproc(keyproc);

    return reader.read_line();
}

std::string trimmed_string( std::string s )
{
    trim_string(s);
    return (s);
}

// also used with macros
std::string & trim_string( std::string &str )
{
    str.erase( 0, str.find_first_not_of( " \t\n\r" ) );
    str.erase( str.find_last_not_of( " \t\n\r" ) + 1 );

    return (str);
}

static void add_segment(std::vector<std::string> &segs,
                        std::string s,
                        bool trim,
                        bool accept_empty)
{
    if (trim && !s.empty())
        trim_string(s);

    if (accept_empty || !s.empty())
        segs.push_back(s);
}

std::vector<std::string> split_string(
        const char *sep, 
        std::string s,
        bool trim_segments,
        bool accept_empty_segments,
        int nsplits)
{
    std::vector<std::string> segments;
    int separator_length = strlen(sep);

    std::string::size_type pos;
    while (nsplits && (pos = s.find(sep)) != std::string::npos)
    {
        add_segment(segments, s.substr(0, pos),
                    trim_segments, accept_empty_segments);
        
        s.erase(0, pos + separator_length);
        
        if (nsplits > 0)
            --nsplits;
    }

    if (!s.empty())
        add_segment(segments, s, trim_segments, accept_empty_segments);

    return segments;
}

// The old school way of doing short delays via low level I/O sync.  
// Good for systems like old versions of Solaris that don't have usleep.
#ifdef NEED_USLEEP

#include <sys/time.h>
#include <sys/types.h>
#include <sys/unistd.h>

void usleep(unsigned long time)
{
    struct timeval timer;

    timer.tv_sec = (time / 1000000L);
    timer.tv_usec = (time % 1000000L);

    select(0, NULL, NULL, NULL, &timer);
}
#endif

// Not the greatest version of snprintf, but a functional one that's 
// a bit safer than raw sprintf().  Note that this doesn't do the
// special behaviour for size == 0, largely because the return value 
// in that case varies depending on which standard is being used (SUSv2 
// returns an unspecified value < 1, whereas C99 allows str == NULL
// and returns the number of characters that would have been written). -- bwr
#ifdef NEED_SNPRINTF

#include <string.h>

int snprintf( char *str, size_t size, const char *format, ... )
{
    va_list argp;
    va_start( argp, format );

    char *buff = new char [ 10 * size ];  // hopefully enough 
    if (!buff)
        end(1, false, "Out of memory\n");

    vsprintf( buff, format, argp );
    strncpy( str, buff, size );
    str[ size - 1 ] = 0;

    int ret = strlen( str );  
    if ((unsigned int) ret == size - 1 && strlen( buff ) >= size)
        ret = -1;

    delete [] buff;

    va_end( argp );

    return (ret);
}

#endif

///////////////////////////////////////////////////////////////////////
// Pattern matching

inline int pm_lower(int ch, bool icase)
{
    return icase? tolower(ch) : ch;
}

// Determines whether the pattern specified by 'pattern' matches the given
// text. A pattern is a simple glob, with the traditional * and ? wildcards.
static bool glob_match( const char *pattern, const char *text, bool icase )
{
    char p, t;
    bool special;

    for (;;)
    {
        p = pm_lower(*pattern++, icase);
        t = pm_lower(*text++, icase);
        special = true;

        if (!p) return t == 0;
        if (p == '\\' && *pattern)
        {
            p       = pm_lower(*pattern++, icase);
            special = false;
        }

        if (p == '*' && special)
            // Try to match exactly at the current text position...
            return !*pattern || glob_match(pattern, text - 1, icase)? true :
                // Or skip one character in the text and try the wildcard
                // match again. If this is the end of the text, the match has
                // failed.
                t? glob_match(pattern - 1, text, icase) : false;
        else if (!t || (p != t && (p != '?' || !special)))
            return false;
    }
}

#if defined(REGEX_PCRE)
////////////////////////////////////////////////////////////////////
// Perl Compatible Regular Expressions

void *compile_pattern(const char *pattern, bool icase)
{
    const char *error;
    int erroffset;
    int flags = icase? PCRE_CASELESS : 0;
    return pcre_compile(pattern, 
                        flags,
                        &error,
                        &erroffset,
                        NULL);
}

void free_compiled_pattern(void *cp)
{
    if (cp)
        pcre_free(cp);
}

bool pattern_match(void *compiled_pattern, const char *text, int length)
{
    int ovector[42];
    int pcre_rc = pcre_exec(static_cast<pcre *>(compiled_pattern),
              NULL,
              text,
              length,
              0,
              0,
              ovector,
              sizeof(ovector) / sizeof(*ovector));
    return (pcre_rc >= 0);
}

////////////////////////////////////////////////////////////////////
#elif defined(REGEX_POSIX)
////////////////////////////////////////////////////////////////////
// POSIX regular expressions

void *compile_pattern(const char *pattern, bool icase)
{
    regex_t *re = new regex_t;
    if (!re)
        return NULL;

    int flags = REG_EXTENDED | REG_NOSUB;
    if (icase)
        flags |= REG_ICASE;
    int rc = regcomp(re, pattern, flags);
    // Nonzero return code == failure
    if (rc)
    {
        delete re;
        return NULL;
    }
    return re;
}

void free_compiled_pattern(void *cp)
{
    if (cp)
    {
        regex_t *re = static_cast<regex_t *>( cp );
        regfree(re);
        delete re;
    }
}

bool pattern_match(void *compiled_pattern, const char *text, int length)
{
    regex_t *re = static_cast<regex_t *>( compiled_pattern );
    return !regexec(re, text, 0, NULL, 0);
}

////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////
// Basic glob

struct glob_info
{
    std::string s;
    bool ignore_case;
};

void *compile_pattern(const char *pattern, bool icase) 
{
    // If we're using simple globs, we need to box the pattern with '*'
    std::string s = std::string("*") + pattern + "*";
    glob_info *gi = new glob_info;
    if (gi)
    {
        gi->s = s;
        gi->ignore_case = icase;
    }
    return gi;
}

void free_compiled_pattern(void *compiled_pattern)
{
    delete static_cast<glob_info *>( compiled_pattern );
}

bool pattern_match(void *compiled_pattern, const char *text, int length)
{
    glob_info *gi = static_cast<glob_info *>( compiled_pattern );
    return glob_match(gi->s.c_str(), text, gi->ignore_case);
}
////////////////////////////////////////////////////////////////////

#endif


/////////////////////////////////////////////////////////////
// input_history
//

input_history::input_history(size_t size)
    : history(), pos(), maxsize(size)
{
    if (maxsize < 2)
        maxsize = 2;

    pos = history.end();
}

void input_history::new_input(const std::string &s)
{
    history.remove(s);

    if (history.size() == maxsize)
        history.pop_front();

    history.push_back(s);

    // Force the iterator to the end (also revalidates it)
    go_end();
}

const std::string *input_history::prev()
{
    if (history.empty())
        return NULL;

    if (pos == history.begin())
        pos = history.end();

    return &*--pos;
}

const std::string *input_history::next()
{
    if (history.empty())
        return NULL;

    if (pos == history.end() || ++pos == history.end())
        pos = history.begin();

    return &*pos;
}

void input_history::go_end()
{
    pos = history.end();
}

void input_history::clear()
{
    history.clear();
    go_end();
}

/////////////////////////////////////////////////////////////////////////
// line_reader

line_reader::line_reader(char *buf, size_t sz, int wrap)
    : buffer(buf), bufsz(sz), history(NULL), start_x(0),
      start_y(0), keyfn(NULL), wrapcol(wrap), cur(NULL),
      length(0), pos(-1)
{
}

std::string line_reader::get_text() const
{
    return (buffer);
}

void line_reader::set_input_history(input_history *i)
{
    history = i;
}

void line_reader::set_keyproc(keyproc fn)
{
    keyfn = fn;
}

void line_reader::cursorto(int ncx)
{
    int x = (start_x + ncx - 1) % wrapcol + 1;
    int y = start_y + (start_x + ncx - 1) / wrapcol;
    ::gotoxy(x, y);
}

int line_reader::read_line(bool clear_previous)
{
    if (bufsz <= 0) return false;

    cursor_control coff(true);

    if (clear_previous)
        *buffer = 0;

    start_x = wherex();
    start_y = wherey();

    length = strlen(buffer);

    // Remember the previous cursor position, if valid.
    if (pos < 0 || pos > length)
        pos = length;

    cur = buffer + pos;

    if (length)
        wrapcprintf(wrapcol, "%s", buffer);

    if (pos != length)
        cursorto(pos);

    if (history)
        history->go_end();

    for ( ; ; )
    {
        int ch = c_getch();

        if (keyfn)
        {
            int whattodo = (*keyfn)(ch);
            if (whattodo == 0)
            {
                buffer[length] = 0;
                if (history && length)
                    history->new_input(buffer);
                return (0);
            }
            else if (whattodo == -1)
            {
                buffer[length] = 0;                
                return (ch);
            }
        }

        int ret = process_key(ch);
        if (ret != -1)
            return (ret);
    }
}

void line_reader::backspace()
{
    if (pos)
    {
        --cur;
        char *c = cur;
        while (*c)
        {
            *c = c[1];
            c++;
        }
        --pos;
        --length;

        cursorto(pos);
        buffer[length] = 0;
        wrapcprintf( wrapcol, "%s ", cur );
        cursorto(pos);
    }
}

bool line_reader::is_wordchar(int c)
{
    return isalnum(c) || c == '_' || c == '-';
}

void line_reader::killword()
{
    if (!pos || cur == buffer)
        return;

    bool foundwc = false;
    while (pos)
    {
        if (is_wordchar(cur[-1]))
            foundwc = true;
        else if (foundwc)
            break;

        backspace();
    }
}

int line_reader::process_key(int ch)
{
    switch (ch)
    {
    case CK_ESCAPE:
        return (ch);
    case CK_UP:
    case CK_DOWN:
    {
        if (!history)
            break;

        const std::string *text = 
                    ch == CK_UP? history->prev() : history->next();

        if (text)
        {
            int olen = length;
            length = text->length();
            if (length >= (int) bufsz)
                length = bufsz - 1;
            memcpy(buffer, text->c_str(), length);
            buffer[length] = 0;
            cursorto(0);

            int clear = length < olen? olen - length : 0;
            wrapcprintf(wrapcol, "%s%*s", buffer, clear, "");

            pos = length;
            cur = buffer + pos;
            cursorto(pos);
        }
        break;
    }
    case CK_ENTER:
        buffer[length] = 0;
        if (history && length)
            history->new_input(buffer);
        return (0);

    case CONTROL('K'):
    {
        // Kill to end of line
        int erase = length - pos;
        if (erase)
        {
            length = pos;
            buffer[length] = 0;
            wrapcprintf( wrapcol, "%*s", erase, "" );
            cursorto(pos);
        }
        break;
    }
    case CK_DELETE:
        if (pos < length)
        {
            char *c = cur;
            while (c - buffer < length)
            {
                *c = c[1];
                c++;
            }
            --length;

            cursorto(pos);
            buffer[length] = 0;
            wrapcprintf( wrapcol, "%s ", cur );
            cursorto(pos);
        }
        break;

    case CK_BKSP:
        backspace();
        break;

    case CONTROL('W'):
        killword();
        break;

    case CK_LEFT:
        if (pos)
        {
            --pos;
            cur = buffer + pos;
            cursorto(pos);
        }
        break;
    case CK_RIGHT:
        if (pos < length)
        {
            ++pos;
            cur = buffer + pos;
            cursorto(pos);
        }
        break;
    case CK_HOME:
    case CONTROL('A'):
        pos = 0;
        cur = buffer + pos;
        cursorto(pos);
        break;
    case CK_END:
    case CONTROL('E'):
        pos = length;
        cur = buffer + pos;
        cursorto(pos);
        break;
    default:
        if (isprint(ch) && length < (int) bufsz - 1)
        {
            if (pos < length)
            {
                char *c = buffer + length - 1;
                while (c >= cur)
                {
                    c[1] = *c;
                    c--;
                }
            }
            *cur++ = (char) ch;
            ++length;
            ++pos;
            putch(ch);
            if (pos < length)
            {
                buffer[length] = 0;
                wrapcprintf( wrapcol, "%s", cur );
            }
            cursorto(pos);
        }
        break;
    }

    return (-1);
}
