#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "log_generator.h"

FILE* LG_logFile     = NULL;
bool  LG_initialized = false;

void LG_Write(const char* format, va_list args);

//-----------------------------------------------------------------------------
//! Initializes the log - opens the log file and writes the header.
//!
//! @return whether or not initialization has been successful. Returns false if
//!         log has already been initialized.
//-----------------------------------------------------------------------------
bool LG_Init()
{
    if (LG_IsInitialized()) { return false; }

    LG_logFile = fopen(LG_FILE_NAME, "w");
    if (LG_logFile == NULL) { return false; }

    LG_initialized = true;

    LG_Write("<!DOCTYPE html>\n<html>\n<head><link rel=\"stylesheet\" href=\"%s\"></head>\n<body>\n", LG_STLE_NAME);

    return true;
}

//-----------------------------------------------------------------------------
//! Closes the log - closes the log file and writes the footer.
//!
//! @return whether or not closing has been successful. Returns false if
//!         log hasn't been initialized yet.
//-----------------------------------------------------------------------------
bool LG_Close()
{
    if (!LG_IsInitialized()) { return false; }

    LG_Write("\n</body>\n</html>");

    if (fclose(LG_logFile) == EOF) { return false; }

    LG_initialized = false;
    return true;
}

//-----------------------------------------------------------------------------
//! @return whether or not log has been initialized.
//-----------------------------------------------------------------------------
bool LG_IsInitialized()
{
    return LG_initialized;
}

//-----------------------------------------------------------------------------
//! Logs image.
//!
//! @param [in] fileName file name of the image with extension 
//!                      (e.g. .png/.jpg/etc)
//! @param [in] htmlStyle style of the image (e.g. "width:auto; height:auto"). 
//!                       If htmlStyle is NULL then no img style will be added
//!
//! @note image should be in the same folder as the log.html file (by default 
//!       log/) to be shown correctly.
//-----------------------------------------------------------------------------
void LG_AddImage(const char* fileName, const char* htmlStyle)
{
    assert(fileName != NULL);

    if (!LG_IsInitialized()) { return; }

    if (htmlStyle == NULL) { LG_Write("<img src=\"%s\" alt=\"No image\">", fileName); }
    else                   { LG_Write("<img style=\"%s\" src=\"%s\" alt=\"No image\">", htmlStyle, fileName); }
}

//-----------------------------------------------------------------------------
//! Logs message as one paragraph.
//!
//! @param [in] format
//! @param [in] styleClass specifies view of the message 
//!             (see @ref LG_STYLE_CLASSES).
//! @param [in] ...
//! 
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_LogMessage(const char* format, LG_StyleClass styleClass, ...)
{
    assert(format          != NULL);
    assert(styleClass.name != NULL);

    if (!LG_IsInitialized()) { return; }

    va_list args;
    va_start(args, styleClass);

    LG_WriteMessageStart(styleClass);
    LG_Write(format, args);
    LG_WriteMessageEnd();

    va_end(args);
}

//-----------------------------------------------------------------------------
//! Logs message as one paragraph.
//!
//! @param [in] format
//! @param [in] color color of the entire message to be logged 
//!             (see @ref LG_COLORS).
//! @param [in] ...
//! 
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_LogMessage(const char* format, LG_Color color, ...)
{
    assert(format != NULL);

    if (!LG_IsInitialized()) { return; }

    va_list args;
    va_start(args, color);

    LG_WriteMessageStart(color);
    LG_Write(format, args);
    LG_WriteMessageEnd();

    va_end(args);
}

//-----------------------------------------------------------------------------
//! Starts a new paragraph inside which to write next.
//! 
//! @param [in] styleClass style of the entire paragraph, though can be changed 
//!             for individual paragraph parts by calling LG_Write with 
//!             specified style class (see @ref LG_STYLE_CLASSES).
//!
//! @warning Supposed to be followed by a sequence of LG_Write calls and then 
//!          by LG_WriteMessageEnd.
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_WriteMessageStart(LG_StyleClass styleClass)
{
    if (!LG_IsInitialized()) { return; }

    LG_Write("\n<pre class=\"%s\">\n", styleClass.name);
}

//-----------------------------------------------------------------------------
//! Starts a new paragraph inside which to write next.
//! 
//! @param [in] color color of the entire paragraph, though can be changed for
//!             individual paragraph parts by calling LG_Write with specified 
//!             color (see @ref LG_COLORS).
//!
//! @warning Supposed to be followed by a sequence of LG_Write calls and then 
//!          by LG_WriteMessageEnd.
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_WriteMessageStart(LG_Color color)
{
    if (!LG_IsInitialized()) { return; }

    LG_Write("\n<pre style=\"color: rgb(%u, %u, %u);\">\n", color.r, color.g, color.b);
}

//-----------------------------------------------------------------------------
//! Ends the current paragraph. 
//!
//! @warning Supposed to be called after LG_WriteMessageStart and some 
//!          sequence of LG_Write calls.
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_WriteMessageEnd()
{
    if (!LG_IsInitialized()) { return; }

    LG_Write("\n</pre>\n");
}

//-----------------------------------------------------------------------------
//! Logs string as a continuation of what has been logged already. 
//!
//! @param [in] format
//! @param [in] styleClass style class of the entire string to be logged 
//!             (see @ref LG_STYLE_CLASSES).
//! @param [in] ...
//!
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_Write(const char* format, LG_StyleClass styleClass, ...)
{
    assert(format          != NULL);
    assert(styleClass.name != NULL);

    if (!LG_IsInitialized()) { return; }

    va_list args;
    va_start(args, styleClass);

    LG_Write("<span class=\"%s\">", styleClass.name);
    LG_Write(format, args);
    LG_Write("</span>");

    va_end(args);
}

//-----------------------------------------------------------------------------
//! Logs string as a continuation of what has been logged already. 
//!
//! @param [in] format
//! @param [in] color color of the entire string to be logged 
//!             (see @ref LG_COLORS).
//! @param [in] ...
//!
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_Write(const char* format, LG_Color color, ...)
{
    assert(format != NULL);

    if (!LG_IsInitialized()) { return; }

    va_list args;
    va_start(args, color);

    LG_Write("<span style=\"color: rgb(%u, %u, %u);\">", color.r, color.g, color.b);
    LG_Write(format, args);
    LG_Write("</span>");

    va_end(args);
}

//-----------------------------------------------------------------------------
//! Logs string as a continuation of what has been logged already. Default 
//! color is black.
//!
//! @param [in] format
//! @param [in] ...
//!
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_Write(const char* format, ...)
{
    assert(format != NULL);

    if (!LG_IsInitialized()) { return; }

    va_list args;
    va_start(args, format);

    LG_Write(format, args);
    
    va_end(args);
}

//-----------------------------------------------------------------------------
//! Logs string as a continuation of what has been logged already.
//!
//! @param [in] format
//! @param [in] ...
//!
//! @warning Logs nothing if log hasn't been initialized.
//-----------------------------------------------------------------------------
void LG_Write(const char* format, va_list args)
{
    assert(format != NULL);

    if (!LG_IsInitialized()) { return; }

    vfprintf(LG_logFile, format, args);
}