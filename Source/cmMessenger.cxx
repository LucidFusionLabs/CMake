/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#include "cmMessenger.h"
#include "cmMessenger.h"
#include "cmDocumentationFormatter.h"
#include "cmOutputConverter.h"

#if defined(CMAKE_BUILD_WITH_CMAKE)
# include <cmsys/SystemInformation.hxx>
#endif

cmake::MessageType cmMessenger::ConvertMessageType(cmake::MessageType t) const
{
  bool warningsAsErrors;

  if (t == cmake::AUTHOR_WARNING || t == cmake::AUTHOR_ERROR)
    {
    warningsAsErrors = this->GetDevWarningsAsErrors();
    if (warningsAsErrors && t == cmake::AUTHOR_WARNING)
      {
      t = cmake::AUTHOR_ERROR;
      }
    else if (!warningsAsErrors && t == cmake::AUTHOR_ERROR)
      {
      t = cmake::AUTHOR_WARNING;
      }
    }
  else if (t == cmake::DEPRECATION_WARNING || t == cmake::DEPRECATION_ERROR)
    {
    warningsAsErrors = this->GetDeprecatedWarningsAsErrors();
    if (warningsAsErrors && t == cmake::DEPRECATION_WARNING)
      {
      t = cmake::DEPRECATION_ERROR;
      }
    else if (!warningsAsErrors && t == cmake::DEPRECATION_ERROR)
      {
      t = cmake::DEPRECATION_WARNING;
      }
    }

  return t;
}

bool cmMessenger::IsMessageTypeVisible(cmake::MessageType t) const
{
  bool isVisible = true;

  if(t == cmake::DEPRECATION_ERROR)
    {
    if(!this->GetDeprecatedWarningsAsErrors())
      {
      isVisible = false;
      }
    }
  else if (t == cmake::DEPRECATION_WARNING)
    {
    if (this->GetSuppressDeprecatedWarnings())
      {
      isVisible = false;
      }
    }
  else if (t == cmake::AUTHOR_ERROR)
    {
    if (!this->GetDevWarningsAsErrors())
      {
      isVisible = false;
      }
    }
  else if (t == cmake::AUTHOR_WARNING)
    {
    if (this->GetSuppressDevWarnings())
      {
      isVisible = false;
      }
    }

  return isVisible;
}

bool printMessagePreamble(cmake::MessageType t, std::ostream& msg)
{
  // Construct the message header.
  if(t == cmake::FATAL_ERROR)
    {
    msg << "CMake Error";
    }
  else if(t == cmake::INTERNAL_ERROR)
    {
    msg << "CMake Internal Error (please report a bug)";
    }
  else if(t == cmake::LOG)
    {
    msg << "CMake Debug Log";
    }
  else if(t == cmake::DEPRECATION_ERROR)
    {
    msg << "CMake Deprecation Error";
    }
  else if (t == cmake::DEPRECATION_WARNING)
    {
    msg << "CMake Deprecation Warning";
    }
  else if (t == cmake::AUTHOR_WARNING)
    {
    msg << "CMake Warning (dev)";
    }
  else if (t == cmake::AUTHOR_ERROR)
    {
    msg << "CMake Error (dev)";
    }
  else
    {
    msg << "CMake Warning";
    }
  return true;
}

void printMessageText(std::ostream& msg, std::string const& text)
{
   msg << ":\n";
   cmDocumentationFormatter formatter;
   formatter.SetIndent("  ");
   formatter.PrintFormatted(msg, text.c_str());
}

void displayMessage(cmake::MessageType t, std::ostringstream& msg)
{
  // Add a note about warning suppression.
  if(t == cmake::AUTHOR_WARNING)
    {
    msg <<
      "This warning is for project developers.  Use -Wno-dev to suppress it.";
    }
  else if (t == cmake::AUTHOR_ERROR)
    {
    msg <<
      "This error is for project developers. Use -Wno-error=dev to suppress "
      "it.";
    }

  // Add a terminating blank line.
  msg << "\n";

#if defined(CMAKE_BUILD_WITH_CMAKE)
  // Add a C++ stack trace to internal errors.
  if(t == cmake::INTERNAL_ERROR)
    {
    std::string stack = cmsys::SystemInformation::GetProgramStack(0,0);
    if(!stack.empty())
      {
      if(cmHasLiteralPrefix(stack, "WARNING:"))
        {
        stack = "Note:" + stack.substr(8);
        }
      msg << stack << "\n";
      }
    }
#endif

  // Output the message.
  if(t == cmake::FATAL_ERROR
     || t == cmake::INTERNAL_ERROR
     || t == cmake::DEPRECATION_ERROR
     || t == cmake::AUTHOR_ERROR)
    {
    cmSystemTools::SetErrorOccured();
    cmSystemTools::Message(msg.str().c_str(), "Error");
    }
  else
    {
    cmSystemTools::Message(msg.str().c_str(), "Warning");
    }
}

cmMessenger::cmMessenger(cmState* state)
  : State(state)
{

}

void cmMessenger::IssueMessage(cmake::MessageType t, const std::string& text,
                               const cmListFileBacktrace& backtrace,
                               bool force) const
{
  if (!force)
    {
    // override the message type, if needed, for warnings and errors
    cmake::MessageType override = this->ConvertMessageType(t);
    if (override != t)
      {
      t = override;
      force = true;
      }
    }

  if (!force && !this->IsMessageTypeVisible(t))
    {
    return;
    }

  std::ostringstream msg;
  if (!printMessagePreamble(t, msg))
    {
    return;
    }

  // Add the immediate context.
  backtrace.PrintTitle(msg);

  printMessageText(msg, text);

  // Add the rest of the context.
  backtrace.PrintCallStack(msg);

  displayMessage(t, msg);
}

void cmMessenger::IssueMessage(cmake::MessageType t,
                               const std::string& text,
                               cmState::Snapshot snp, bool force) const
{
  cmListFileContext lfc;
  lfc.FilePath = snp.GetExecutionListFile();

  if(!this->State->GetIsInTryCompile())
    {
    cmOutputConverter converter(snp);
    lfc.FilePath = converter.Convert(lfc.FilePath, cmOutputConverter::HOME);
    }
  lfc.Line = 0;
  this->IssueMessage(t, text, lfc, force);
}

void cmMessenger::IssueMessage(cmake::MessageType t, const std::string& text,
                               const cmListFileContext& lfc, bool force) const
{
  if (!force)
    {
    // override the message type, if needed, for warnings and errors
    cmake::MessageType override = this->ConvertMessageType(t);
    if (override != t)
      {
      t = override;
      force = true;
      }
    }

  if (!force && !this->IsMessageTypeVisible(t))
    {
    return;
    }

  std::ostringstream msg;
  if (!printMessagePreamble(t, msg))
    {
    return;
    }

  // Add the immediate context.
  msg << (lfc.Line ? " at " : " in ") << lfc;

  printMessageText(msg, text);

  displayMessage(t, msg);
}

bool cmMessenger::GetSuppressDevWarnings() const
{
  const char* cacheEntryValue = this->State->GetCacheEntryValue(
      "CMAKE_SUPPRESS_DEVELOPER_WARNINGS");
  return cmSystemTools::IsOn(cacheEntryValue);
}

bool cmMessenger::GetSuppressDeprecatedWarnings() const
{
  const char* cacheEntryValue = this->State->GetCacheEntryValue(
    "CMAKE_WARN_DEPRECATED");
  return cacheEntryValue && cmSystemTools::IsOff(cacheEntryValue);
}

bool cmMessenger::GetDevWarningsAsErrors() const
{
  const char* cacheEntryValue = this->State->GetCacheEntryValue(
    "CMAKE_SUPPRESS_DEVELOPER_ERRORS");
  return cacheEntryValue && cmSystemTools::IsOff(cacheEntryValue);
}

bool cmMessenger::GetDeprecatedWarningsAsErrors() const
{
  const char* cacheEntryValue = this->State->GetCacheEntryValue(
    "CMAKE_ERROR_DEPRECATED");
  return cmSystemTools::IsOn(cacheEntryValue);
}
